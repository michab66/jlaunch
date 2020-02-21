/*
 * Simplest Java Launcher.
 *
 * Copyright (c) 2019 Michael G. Binz
 *
 * LGPL
 */

// Use extendedGDI+.
#define GDIPVER     0x0110

#include <tchar.h>
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Gdiplus.h>
#include "Shlwapi.h"

#include <jni.h>

#include <iostream>
#include <string>
#include <map>
#include <sstream>

// Get our resource definitions.
#include "resource.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace
{
    std::wstring GetSuffix(std::wstring& path)
    {
        auto lastIdx = path.find_last_of(L".");
        if (lastIdx == std::wstring::npos)
            return std::wstring{};

        return path.substr(lastIdx);
    }

    std::wstring GetPath(std::wstring& path)
    {
        auto lastIdx = path.find_last_of(L".");
        if (lastIdx == std::wstring::npos)
            return std::wstring{};

        return path.substr(0,lastIdx);
    }

    /**
     * Compute the passed bitmap's mime type.
     */
    std::wstring GetMimeType(Gdiplus::Bitmap* bitmap)
    {
        GUID raw;
        bitmap->GetRawFormat(&raw);

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
            Gdiplus::ImageCodecInfo* current = &tbuffer[i];

            std::wcout << i << " : " << current->MimeType << std::endl;

            if (current->FormatID == raw)
            {
                std::wstring result = current->MimeType;
                return result;
            }
        }

        std::wstring empty;
        return empty;
    }

    /**
     * Compute the passed bitmap's Clsid.
     */
    CLSID GetClsid(Gdiplus::Bitmap* bitmap)
    {
        GUID raw;
        bitmap->GetRawFormat(&raw);

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

            std::wcout <<
                i <<
                " : "
                << c->MimeType
                << std::endl;

            if (IsEqualCLSID(c->FormatID, raw))
                return c->Clsid;
        }

        return CLSID_NULL;
    }

    void writeImageFile(
        std::wstring name,
        std::wstring suffix,
        uint32_t dimension,
        CLSID& clsid,
        Gdiplus::Bitmap* bitMap)
    {
        Gdiplus::Bitmap* sqeezed = 
            new Gdiplus::Bitmap(
                dimension,
                dimension,
                PixelFormat32bppARGB);

        Gdiplus::Graphics* graphic = 
            Gdiplus::Graphics::FromImage(
                sqeezed);
        graphic->SetCompositingQuality(
            Gdiplus::CompositingQuality::CompositingQualityHighQuality);
        graphic->SetInterpolationMode(
            Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear);
        graphic->SetSmoothingMode(
            Gdiplus::SmoothingMode::SmoothingModeHighQuality);
        graphic->DrawImage(
            bitMap,
            0,
            0,
            dimension,
            dimension);

        std::wostringstream collector;
        collector <<
            name <<
            L"-" <<
            dimension <<
            suffix;

        sqeezed->Save(
            collector.str().c_str(), 
            &clsid);

        delete sqeezed;
    }

} // unnamed namespace.

// https://devblogs.microsoft.com/oldnewthing/20101022-00/?p=12473
// https://stackoverflow.com/questions/51383896/c-gdibitmap-to-png-image-in-memory

int wmain(int argc, _TCHAR* argv[])
{
    using std::wstring;

    Gdiplus::GdiplusStartupInputEx gdiStartupInput;
    ULONG_PTR gdiplustoken;
    Gdiplus::GdiplusStartup(&gdiplustoken, &gdiStartupInput, nullptr);

    std::wstring strfilePath =
        L"mmt-icon-1024.png";
    std::wstring strdup =
        L"mmt-icon-128.png";

    if (! PathFileExistsW(strfilePath.c_str()))
        // File not found.
        return 1;

    Gdiplus::Bitmap* bitMap =
        new Gdiplus::Bitmap(strfilePath.c_str());

    CLSID fileClsid =
        GetClsid(bitMap);
    if (IsEqualCLSID(fileClsid, CLSID_NULL))
        // Unknown file type.
        return 1;

    wstring baseName =
        GetPath(strfilePath);
    wstring suffix =
        GetSuffix(strfilePath);
    writeImageFile(baseName, suffix,  16, fileClsid, bitMap);
    writeImageFile(baseName, suffix,  32, fileClsid, bitMap);
    writeImageFile(baseName, suffix,  64, fileClsid, bitMap);
    writeImageFile(baseName, suffix, 128, fileClsid, bitMap);
    writeImageFile(baseName, suffix, 256, fileClsid, bitMap);

    Gdiplus::GdiplusShutdown(gdiplustoken);
    return 0;
}
