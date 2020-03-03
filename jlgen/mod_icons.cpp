/*
 * $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

// See https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/use-utf8-code-page
#undef UNICODE

// Use extendedGDI+.
#define GDIPVER     0x0110

#include <tchar.h>
#include <windows.h>
#include <Gdiplus.h>
#include "Shlwapi.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "winicon.h"

#include "mod_util.hpp"
#include "RtIcon.h"

#include "mod_icons.hpp"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

using std::string;
using std::vector;
using Gdiplus::Graphics;
using Gdiplus::Bitmap;
using mob::windows::RtIcon;

namespace
{
    class InitGdiPlus
    {
        Gdiplus::GdiplusStartupInputEx gdiStartupInput_{};
        ULONG_PTR gdiplustoken_{};

    public:
        InitGdiPlus()
        {
            Gdiplus::GdiplusStartup(
                &gdiplustoken_,
                &gdiStartupInput_,
                nullptr);
        }
        ~InitGdiPlus()
        {
            Gdiplus::GdiplusShutdown(
                gdiplustoken_);
        }
    };

    string GetSuffix(const string& path)
    {
        auto lastIdx = path.find_last_of(".");
        if (lastIdx == string::npos)
            return string{};

        return path.substr(lastIdx);
    }

    string GetPath(const string& path)
    {
        auto lastIdx = path.find_last_of(".");
        if (lastIdx == string::npos)
            return string{};

        return path.substr(0,lastIdx);
    }

    /**
     * Compute the passed bitmap's Clsid.
     */
    CLSID GetClsid(Bitmap& bitmap)
    {
        GUID raw;
        bitmap.GetRawFormat(&raw);

        UINT numDecoders;
        UINT size;
        Gdiplus::GetImageDecodersSize(&numDecoders, &size);

        std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);

        Gdiplus::GetImageDecoders(
            numDecoders,
            size,
            (Gdiplus::ImageCodecInfo*) & buffer[0]);

        Gdiplus::ImageCodecInfo* tbuffer =
            (Gdiplus::ImageCodecInfo*) & buffer[0];

        for (size_t i = 0; i < numDecoders; ++i)
        {
            Gdiplus::ImageCodecInfo* c =
                &tbuffer[i];

            std::cout <<
                i <<
                " : "
                << smack::util::convert( c->MimeType )
                << std::endl;

            if (IsEqualCLSID(c->FormatID, raw))
                return c->Clsid;
        }

        return CLSID_NULL;
    }

    std::vector<std::uint8_t> GetImageBinary(
        INT dimension,
        CLSID& clsid,
        Bitmap& bitmap)
    {
        Bitmap resized{
                dimension,
                dimension,
                PixelFormat32bppARGB };

        std::unique_ptr<Graphics> graphic =
            std::unique_ptr<Graphics>(Graphics::FromImage(&resized));

        graphic->SetCompositingQuality(
            Gdiplus::CompositingQuality::CompositingQualityHighQuality);
        graphic->SetInterpolationMode(
            Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear);
        graphic->SetSmoothingMode(
            Gdiplus::SmoothingMode::SmoothingModeHighQuality);
        graphic->DrawImage(
            &bitmap,
            0,
            0,
            dimension,
            dimension);

        //write to IStream
        IStream* istream = nullptr;
        HRESULT rc = CreateStreamOnHGlobal(NULL, TRUE, &istream);
        if ( rc != NOERROR )
            throw std::invalid_argument("CreateStreamOnHGlobal");

        resized.Save(
            istream,
            &clsid);

        //get memory handle associated with istream
        HGLOBAL hg = NULL;
        rc = GetHGlobalFromStream(istream, &hg);
        if (rc != NOERROR)
            throw std::invalid_argument("GetHGlobalFromStream");

        //copy IStream to buffer
        size_t bufsize = GlobalSize(hg);

        std::vector<uint8_t> result( bufsize );

        //lock & unlock memory
        LPVOID pimage = GlobalLock(hg);
        memcpy(&result[0], pimage, bufsize);
        GlobalUnlock(hg);

        istream->Release();

        return result;
    }

    void WriteImageFile(
        const string& name,
        const string& suffix,
        INT dimension,
        CLSID& clsid,
        Bitmap& bitmap)
    {
        auto scaled = GetImageBinary(
            dimension,
            clsid,
            bitmap);

        string targetName =
            name +
            "-" +
            std::to_string(dimension) +
            suffix;

        // Write to file.
        std::ofstream fout(
            targetName,
            std::ios::binary);
        fout.write(
            (char*)scaled.data(),
            scaled.size());
        // Trigger write error here, not in destructor.
        fout.close();
    }
} // unnamed namespace.

namespace smack {
namespace util {
namespace icons {

/**
 * Use the passed image to create a set of scaled, square images in the
 * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
 * image though all image sizes will do.
 */
void WriteImageSet(
    const string& sourceFile, 
    const std::initializer_list<uint16_t> sizes)
{
    InitGdiPlus init;

    if (!PathFileExistsA(sourceFile.c_str()))
        throw std::invalid_argument("File not found.");

    std::wstring wideName = 
        smack::util::convert(sourceFile);

    Gdiplus::Bitmap bitmap{ wideName.c_str() };

    CLSID fileClsid =
        GetClsid(bitmap);
    if (IsEqualCLSID(fileClsid, CLSID_NULL))
        throw std::invalid_argument("Unknown file type.");

    string baseName =
        GetPath(sourceFile);
    string suffix =
        GetSuffix(sourceFile);

    for (auto c : sizes)
        WriteImageFile(baseName, suffix, c, fileClsid, bitmap);
}

/**
 *
 */
void CreateIcons(
    vector<std::unique_ptr<RtIcon>>& outHolder,
    const std::initializer_list<uint16_t> sizes,
    const string& sourcePng)
{
    if (!PathFileExistsA(sourcePng.c_str()))
        throw std::invalid_argument("File not found.");

    InitGdiPlus init;

    std::wstring wideName =
        smack::util::convert(sourcePng);

    Gdiplus::Bitmap bitmap{ wideName.c_str() };

    CLSID fileClsid =
        GetClsid(bitmap);
    if (IsEqualCLSID(fileClsid, CLSID_NULL))
        throw std::invalid_argument("Unknown file type.");

    for (auto wdimension : sizes)
    {
        if (wdimension > 256 || wdimension == 0)
            throw std::invalid_argument(
                "Unsupported size " + std::to_string(wdimension) );
        auto binary =
            GetImageBinary(
                wdimension, 
                fileClsid, 
                bitmap);
        // For a size of 256 this results in a passed dimension of 
        // zero below which happens to be the way to specify a
        // dimension of 256 pixels.
        BYTE dimension =
            static_cast<BYTE>(wdimension);
        auto icon =
            std::unique_ptr<RtIcon>(
                new RtIcon(dimension, dimension, 32, binary));
        outHolder.push_back(std::move(icon));
    }
}

}
}
}
