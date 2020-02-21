/*
 * Simplest Java Launcher.
 *
 * Copyright (c) 2019 Michael G. Binz
 *
 * LGPL
 */

#include <array>

#define GDIPVER     0x0110

#include <tchar.h>
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Gdiplus.h>

#include <jni.h>
#include <iostream>
#include <string>
#include <map>

// Get our resource definitions.
#include "resource.h"

static int dieWithMessage( const TCHAR* msg )
{
    TCHAR szExeFileName[MAX_PATH];
    GetModuleFileName(NULL, szExeFileName, MAX_PATH);

    MessageBox(NULL, msg, szExeFileName, MB_OK | MB_APPLMODAL);

    return 1;
}

/**
 * Read a string resource for a resource id.
 *
 * @param instance The application's instance handle.
 * @param id The resource id.
 * @return The resource string converted to 8bit platform encoding. The 
 * string is empty if the resource was not found.
 */
static std::string getStringResource( HINSTANCE instance, UINT id )
{
    WCHAR* notUsed{};

    // Get the size of the configured resource string.
    int rcw = LoadStringW(
        instance, 
        id,
        // We do not use this since it points to the wide character
        // version of the resource.
        (WCHAR*)(&notUsed), 
        0);

    // Create our result string in the required size.
    std::string result(rcw , '\0');

    // LoadStringA gets the actual result converted into the
    // 8bit platform encoding.
    int rca = LoadStringA(
        instance, 
        id,
        // We directly write into the result string.
        &result[0],
        rcw+1 );

    if (rcw == rca)
        return result;

    // TODO(micbinz) implement the error path.
    DWORD error = 
        GetLastError();

    // Return the empty string in case of an error.
    return std::string{};
}

/*
 * Windows entry point.
 */
int APIENTRY micbinzwWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    std::string mainModule = getStringResource(
        hInstance, 
        IDS_JAVA_MAIN_MODULE );

    if ( mainModule.empty() )
        return dieWithMessage(_T("IDS_JAVA_MAIN_MODULE not in resources."));
    // Add the reqired option prefix to set the main module.
    mainModule = 
        "--add-modules=" + 
        mainModule;

    std::string mainClassName = getStringResource(
        hInstance,
        IDS_JAVA_MAIN_CLASS);
    if ( mainClassName.empty() )
        return dieWithMessage(_T("IDS_JAVA_MAIN_CLASS not in resources."));

    std::array<JavaVMOption,1>jvmopt{ 
        const_cast<char*>(mainModule.c_str()),
        nullptr 
    };

    JavaVMInitArgs vmArgs{
        JNI_VERSION_1_8,
        static_cast<jint>(jvmopt.size()),
        jvmopt.data(),
        JNI_FALSE 
    };

    // Create the JVM.
    JavaVM* javaVM = nullptr;
    JNIEnv* env = nullptr;
    long rc = JNI_CreateJavaVM(
        &javaVM,
        (void**)&env, 
        &vmArgs );
    if (rc == JNI_ERR) {        
        return dieWithMessage( _T("Error creating VM\n. Exiting ...") );
    }

    // TODO(michab) Decide if we keep this.
    // Reads a system property.
    //jclass jcls = env->FindClass("java/lang/System");
    //if (jcls == NULL) {
    //    env->ExceptionDescribe();
    //    javaVM->DestroyJavaVM();
    //    return 1;
    //}
    //if (jcls != NULL) {
    //    jmethodID methodId = env->GetStaticMethodID(
    //        jcls,
    //        "getProperty",
    //        "(Ljava/lang/String;)Ljava/lang/String;");
    //    if (methodId != NULL) {
    //        jstring str = env->NewStringUTF("java.home");
    //        jstring got = (jstring) env->CallStaticObjectMethod(jcls, methodId, str);
    //        if (env->ExceptionCheck()) {
    //            env->ExceptionDescribe();
    //            env->ExceptionClear();
    //        }
    //        const char* cstr = env->GetStringUTFChars(got, 0);
    //        std::cout << cstr << std::endl;
    //        env->ReleaseStringUTFChars(got, cstr);
    //        env->DeleteLocalRef(got);        
    //    }
    //}

    jclass stringClass = 
        env->FindClass("java/lang/String");
    jclass mainClass = 
        env->FindClass(mainClassName.c_str());
    if ( ! mainClass )
        return dieWithMessage(_T("IDS_JAVA_MAIN_CLASS not found."));

    jmethodID mainMethod = env->GetStaticMethodID(
        mainClass,
        "main",
        "([Ljava/lang/String;)V");
    if ( ! mainMethod )
        return dieWithMessage(_T("No main() found."));

    jobjectArray argv = 
        env->NewObjectArray(0,stringClass,0);

    env->CallStaticObjectMethod(
        mainClass,
        mainMethod,
        argv);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return dieWithMessage(_T("No good."));
    }

    // Waits until the VM terminates.
    javaVM->DestroyJavaVM();

    return 0;
}
#pragma comment(lib, "gdiplus.lib")

// Internet hacked.  The original code is from microsoft.
// Switch to MS solution.
// https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use
static size_t GetImageEncoder(const wchar_t* form, CLSID* clsID)
{
    size_t nRet = -1;
    unsigned int encodersCount = 0;
    unsigned int encodersSize = 0;
    Gdiplus::ImageCodecInfo* imageCodecs = nullptr;

    Gdiplus::GetImageEncodersSize(&encodersCount, &encodersSize);

    imageCodecs = (Gdiplus::ImageCodecInfo*)malloc(encodersSize);

    Gdiplus::GetImageEncoders(encodersCount, encodersSize, imageCodecs);
    for (size_t index = 0; index < encodersCount; index++)
    {
        if (wcscmp(imageCodecs[index].MimeType, form) == 0)
        {
            Gdiplus::ImageCodecInfo* current = &imageCodecs[index];
            *clsID = imageCodecs[index].Clsid;
            nRet = index;
            break;
        }
    }

    if (imageCodecs)
        free(imageCodecs);
    return nRet;
}

/**
 * Compute the passed bitmap's mime type.
 */
static std::wstring GetMimeType(Gdiplus::Bitmap* bitmap)
{
    GUID raw;
    bitmap->GetRawFormat(&raw);

    UINT numDecoders;
    UINT size;
    Gdiplus::GetImageDecodersSize( &numDecoders, &size );
    
    uint8_t* buffer = new uint8_t[ size ];

    Gdiplus::GetImageDecoders(
        numDecoders, 
        size,
        (Gdiplus::ImageCodecInfo*)buffer);

    Gdiplus::ImageCodecInfo* tbuffer = 
        (Gdiplus::ImageCodecInfo*)buffer;

    for (size_t i = 0; i < numDecoders; ++i)
    {
        Gdiplus::ImageCodecInfo* current = &tbuffer[i];

        std::wcout << i << " : " << current->MimeType << std::endl;

        if (current->FormatID == raw)
        {
            std::wstring result = current->MimeType;
            delete[] buffer;
            return result;
        }
    }

    delete[] buffer;
    std::wstring empty;
    return empty;
}

static void writeImage(
    std::wstring strdup,
    uint32_t width, 
    uint32_t height,
    Gdiplus::Bitmap* bitMap)
{
    std::map<std::wstring, std::wstring> m_mtMap;
    m_mtMap[L".jpeg"] = L"image/jpeg";
    m_mtMap[L".jpe"] = L"image/jpeg";
    m_mtMap[L".jpg"] = L"image/jpeg";
    m_mtMap[L".png"] = L"image/png";
    m_mtMap[L".gif"] = L"image/gif";
    m_mtMap[L".tiff"] = L"image/tiff";
    m_mtMap[L".tif"] = L"image/tiff";
    m_mtMap[L".bmp"] = L"image/bmp";

    Gdiplus::Bitmap* sqeezed  = new Gdiplus::Bitmap(128, 128, PixelFormat32bppARGB);

    Gdiplus::Graphics* graphic = Gdiplus::Graphics::FromImage(sqeezed);
    graphic->SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);
    graphic->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBilinear);
    graphic->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
    graphic->DrawImage(bitMap, 0, 0, 128, 128);
    CLSID encoderCLSID;
    GetImageEncoder(m_mtMap[L".jpg"].c_str(), &encoderCLSID);
    sqeezed->Save(strdup.c_str(), &encoderCLSID);

    delete sqeezed;
}

// https://devblogs.microsoft.com/oldnewthing/20101022-00/?p=12473
// https://stackoverflow.com/questions/51383896/c-gdibitmap-to-png-image-in-memory

int wmain(int argc, _TCHAR* argv[])
{
    Gdiplus::GdiplusStartupInputEx gdiStartupInput;
    ULONG_PTR gdiplustoken;
    Gdiplus::GdiplusStartup(&gdiplustoken, &gdiStartupInput, NULL);

    std::wstring strfilePath = L"C:\\cygwin64\\tmp\\800px-Sunflower_from_Silesia2.png";
    std::wstring strdup = L"C:\\cygwin64\\tmp\\cpp.png";
    Gdiplus::Bitmap* bitMap = new Gdiplus::Bitmap(strfilePath.c_str());

    std::wstring mimeType = GetMimeType(bitMap);

    std::wcout << "micbinz: guid=" << mimeType << std::endl;

    Gdiplus::Bitmap* sqeezed = new Gdiplus::Bitmap(128, 128, PixelFormat32bppRGB);

    Gdiplus::Graphics* graphic = Gdiplus::Graphics::FromImage(sqeezed);
    graphic->SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);
    graphic->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeBilinear);
    graphic->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
    graphic->DrawImage(bitMap, 0, 0, 128, 128);
    CLSID encoderCLSID;
    GetImageEncoder(mimeType.c_str(), &encoderCLSID);
    sqeezed->Save(strdup.c_str(), &encoderCLSID);

    delete bitMap;
    delete sqeezed;

    Gdiplus::GdiplusShutdown(gdiplustoken);
    return 0;
}
