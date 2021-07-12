/* $Id$
 *
 * Icon routines.
 *
 * Copyright (c) 2019-2020 Michael G. Binz
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <intrin.h>

 // See https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/use-utf8-code-page
#undef UNICODE

// Use extendedGDI+.
#define GDIPVER     0x0110

#include <tchar.h>
#include <windows.h>
#include <Gdiplus.h>
#include "Shlwapi.h"

#include "winicon.h"

#include "mod_util.hpp"
#include "RtIcon.h"

#include "mod_icons.hpp"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

using std::filesystem::path;
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

        // Write to IStream
        IStream* istream = nullptr;
        HRESULT rc = CreateStreamOnHGlobal(NULL, TRUE, &istream);
        if ( rc != NOERROR )
            throw std::invalid_argument("CreateStreamOnHGlobal");

        resized.Save(
            istream,
            &clsid);

        // Get the memory handle associated with istream.
        HGLOBAL hg = NULL;
        rc = GetHGlobalFromStream(istream, &hg);
        if (rc != NOERROR)
            throw std::invalid_argument("GetHGlobalFromStream");

        //copy IStream to buffer
        size_t bufsize = GlobalSize(hg);

        std::vector<uint8_t> result( bufsize );

        //lock & unlock memory
        LPVOID pimage = GlobalLock(hg);
        if ( ! pimage )
            throw std::bad_alloc();
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
 *
 */
void CreateIcons(
    vector<std::unique_ptr<RtIcon>>& outHolder,
    const std::initializer_list<uint16_t> sizes,
    const path& sourcePng)
{
    if (!exists(sourcePng))
        throw std::invalid_argument("File not found.");

    InitGdiPlus init;

    Gdiplus::Bitmap bitmap{ sourcePng.c_str() };

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

/**
 * See header.
 */
void CreateWindowsIcon(
    const path& sourcePng,
    const std::initializer_list<uint16_t> sizes,
    const path& targetIco)
{
    std::vector<std::unique_ptr<RtIcon>> outHolder;
    smack::util::icons::CreateIcons(
        outHolder,
        sizes,
        sourcePng);

    std::vector<std::uint8_t> fileContent;

    // Add the prefix icondir structure.
    ICONDIR icondir{
        0,
        1,
        static_cast<WORD>(outHolder.size()) };
    rawAppend(
        fileContent,
        &icondir,
        sizeof(icondir) - sizeof(ICONDIRENTRY));

    // Add the directory entry structures.
    size_t currentOffset{
        fileContent.size() + (outHolder.size() * sizeof(ICONDIRENTRY) )
    };

    for (const std::unique_ptr<RtIcon>& c : outHolder)
    {
        auto grpDirEntry =
            c->GetDirectoryEntry();

        ICONDIRENTRY dirEntry;
        dirEntry.bWidth = 
            grpDirEntry.bWidth;
        dirEntry.bHeight = 
            grpDirEntry.bHeight;
        dirEntry.bColorCount =
            grpDirEntry.bColorCount;
        dirEntry.bReserved =
            grpDirEntry.bReserved;
        dirEntry.wPlanes =
            grpDirEntry.wPlanes;
        dirEntry.wBitCount =
            grpDirEntry.wBitCount;
        dirEntry.dwBytesInRes =
            grpDirEntry.dwBytesInRes;

        dirEntry.dwImageOffset =
            static_cast<DWORD>(currentOffset);
        currentOffset += c->raw_png().size();

        rawAppend(
            fileContent,
            &dirEntry);
    }

    // Add the actual entries.
    for (const std::unique_ptr<RtIcon>& c : outHolder)
    {
        auto pngData =
            c->raw_png();
        rawAppend(
            fileContent,
            pngData.data(),
            pngData.size()
        );
    }

    // Write to file.
    std::ofstream fout(
        targetIco,
        std::ios::binary);
    void* data =
        fileContent.data();
    fout.write(
        static_cast<const char*>(data),
        fileContent.size());
    // Trigger write error here, not in destructor.
    fout.close();
}

/**
 * See header.
 *
 * The implementation uses info from
 * https://en.wikipedia.org/wiki/Apple_Icon_Image_format
 */
void CreateAppleIcon(
    const path& sourcePng,
    const std::initializer_list<uint16_t> sizes,
    const path& targetIco)
{
    std::vector<std::unique_ptr<RtIcon>> outHolder;
    smack::util::icons::CreateIcons(
        outHolder,
        sizes,
        sourcePng);

    std::vector<std::uint8_t> fileContent;

    // Write the lead-in magic literal.
    rawAppend(
        fileContent,
        "icns",
        4 );

    // Compute the size of the file we generate.
    {
        uint32_t fileSize = static_cast<uint32_t>(
            // Sizeof magic literal.
            fileContent.size() +
            sizeof(fileSize) +
            // 8 = sizeof( IconType ) + sizeof( LengthOfData ).
            (outHolder.size() * 8) );
        for (const std::unique_ptr<RtIcon>& c : outHolder)
            fileSize += static_cast<uint32_t>(c->raw_png().size());
        fileContent.reserve(fileSize);

        // MSVC intrinsic.
        fileSize = _byteswap_ulong(fileSize);

        rawAppend(
            fileContent,
            &fileSize
        );
    }

    for (const std::unique_ptr<RtIcon>& c : outHolder)
    {
        auto grpDirEntry =
            c->GetDirectoryEntry();

        const char* OSType;
        switch (grpDirEntry.bWidth)
        {
        case 16:
            OSType = "icp4";
            break;
        case 32:
            OSType = "icp5";
            break;
        case 64:
            OSType = "icp6";
            break;
        case 128:
            OSType = "ic07";
            break;
        // Zero means actually 256.
        case 0:
            OSType = "ic08";
            break;
        default:
            throw std::invalid_argument("Unexpected size.");
        }

        // Icon type.
        rawAppend(
            fileContent,
            OSType,
            4 );

        auto data = 
            c->raw_png();
        uint32_t size =
            static_cast<uint32_t>(data.size());
        // Size is 'including type and length'.
        size += 
            (sizeof(size) + 4);
        size =
            _byteswap_ulong(size);

        // Length of data, big endian.
        rawAppend(
            fileContent,
            &size
        );
        // Icon data.
        rawAppend(
            fileContent,
            data.data(),
            data.size()
        );
    }

    // Write to file.
    std::ofstream fout(
        targetIco,
        std::ios::binary);
    void* data =
        fileContent.data();
    fout.write(
        static_cast<const char*>(data),
        fileContent.size());
    // Trigger write error here, not in destructor.
    fout.close();
}

}
}
}
