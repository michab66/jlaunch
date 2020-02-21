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

// Use extendedGDI+.
#define GDIPVER     0x0110

#include <tchar.h>
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

using std::wstring;

namespace
{
    wstring GetSuffix(const wstring& path)
    {
        auto lastIdx = path.find_last_of(L".");
        if (lastIdx == wstring::npos)
            return wstring{};

        return path.substr(lastIdx);
    }

    wstring GetPath(const wstring& path)
    {
        auto lastIdx = path.find_last_of(L".");
        if (lastIdx == wstring::npos)
            return wstring{};

        return path.substr(0,lastIdx);
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

    void WriteImageFile(
        const wstring& name,
        const wstring& suffix,
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

/**
 * Use the passed image to create a set of scaled, square images in the
 * dimensions 16, 32, 64, 128, 256.  It is recommended to pass a square
 * image though all image sizes will do.
 */
void WriteImageSet(const wstring& sourceFile)
{
    Gdiplus::GdiplusStartupInputEx gdiStartupInput;
    ULONG_PTR gdiplustoken;
    Gdiplus::GdiplusStartup(&gdiplustoken, &gdiStartupInput, nullptr);

    if (!PathFileExistsW(sourceFile.c_str()))
        throw std::invalid_argument("File not found.");

    Gdiplus::Bitmap* bitMap =
        new Gdiplus::Bitmap(sourceFile.c_str());

    CLSID fileClsid =
        GetClsid(bitMap);
    if (IsEqualCLSID(fileClsid, CLSID_NULL))
        throw std::invalid_argument("Unknown file type.");

    wstring baseName =
        GetPath(sourceFile);
    wstring suffix =
        GetSuffix(sourceFile);
    WriteImageFile(baseName, suffix, 16, fileClsid, bitMap);
    WriteImageFile(baseName, suffix, 32, fileClsid, bitMap);
    WriteImageFile(baseName, suffix, 64, fileClsid, bitMap);
    WriteImageFile(baseName, suffix, 128, fileClsid, bitMap);
    WriteImageFile(baseName, suffix, 256, fileClsid, bitMap);
}

int wmain(int argc, _TCHAR* argv[])
{
    Gdiplus::GdiplusStartupInputEx gdiStartupInput;
    ULONG_PTR gdiplustoken;
    Gdiplus::GdiplusStartup(&gdiplustoken, &gdiStartupInput, nullptr);

    wstring strfilePath =
        L"mmt-icon-1024.png";

    WriteImageSet(strfilePath);

    Gdiplus::GdiplusShutdown(gdiplustoken);
    return 0;
}
