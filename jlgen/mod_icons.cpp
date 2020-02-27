/*
 * $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

// Resources
// https://devblogs.microsoft.com/oldnewthing/20101022-00/?p=12473
// https://stackoverflow.com/questions/51383896/c-gdibitmap-to-png-image-in-memory

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
#include <sstream>
#include <vector>

#include "winicon.h"

#include "mod_util.hpp"
#include "mod_icons.hpp"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

using std::string;
using Gdiplus::Graphics;
using Gdiplus::Bitmap;

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
        HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &istream);

        resized.Save(
            istream,
            &clsid);

        //get memory handle associated with istream
        HGLOBAL hg = NULL;
        GetHGlobalFromStream(istream, &hg);

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

        std::ostringstream collector;
        collector <<
            name <<
            "-" <<
            dimension <<
            suffix;

        string targetName = collector.str();

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
void WriteImageSet(const string& sourceFile)
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
    WriteImageFile(baseName, suffix, 16, fileClsid, bitmap);
    WriteImageFile(baseName, suffix, 32, fileClsid, bitmap);
    WriteImageFile(baseName, suffix, 64, fileClsid, bitmap);
    WriteImageFile(baseName, suffix, 128, fileClsid, bitmap);
    WriteImageFile(baseName, suffix, 256, fileClsid, bitmap);
}

/**
 * Use the passed image to create an icon file.
 */
void WriteIconFile(const string& sourceFile)
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

    auto scaled = GetImageBinary(16, fileClsid, bitmap);

    std::cout << "Size is: " << scaled.size() << std::endl;

    baseName.append("-icn");
    baseName.append(suffix);

    std::ofstream fout(
        baseName.c_str(), 
        std::ios::binary);
    fout.write(
        (char*)scaled.data(), 
        scaled.size() );
    // Trigger write error here, not in destructor.
    fout.close();
}

}
}
}
