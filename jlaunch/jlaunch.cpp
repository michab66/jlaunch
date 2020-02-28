/*
 * Java Launcher.
 *
 * Copyright (c) 2020 Michael G. Binz
 */

#include <array>

#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <jni.h>
#include <iostream>
#include <string>

 // Get our resource definitions.
#include "resource.h"

// Pick a supported Java version
#if defined JNI_VERSION_10
#define JNI_VERSION JNI_VERSION_10
#elif defined JNI_VERSION_9
#define JNI_VERSION JNI_VERSION_9
#elif defined JNI_VERSION_1_8
#define JNI_VERSION JNI_VERSION_1_8
#else
#error No supported JNI_VERSION found.
#endif

using std::string;

namespace
{

int dieWithMessage(const TCHAR* msg)
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
string getStringResource(HINSTANCE instance, UINT id)
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
    string result(rcw, '\0');

    // LoadStringA gets the actual result converted into the
    // 8bit platform encoding.
    int rca = LoadStringA(
        instance,
        id,
        // We directly write into the result string.
        &result[0],
        rcw + 1);

    if (rcw == rca)
        return result;

    // TODO(micbinz) implement the error path.
    DWORD error =
        GetLastError();

    // Return the empty string in case of an error.
    return string{};
}

} // namespace

/*
 * Windows entry point.
 */
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    string mainModule = getStringResource(
        hInstance,
        IDS_JAVA_MAIN_MODULE);

    if (mainModule.empty())
        return dieWithMessage(_T("IDS_JAVA_MAIN_MODULE not in resources."));
    // Add the reqired option prefix to set the main module.
    mainModule =
        "--add-modules=" +
        mainModule;

    string mainClassName = getStringResource(
        hInstance,
        IDS_JAVA_MAIN_CLASS);
    if (mainClassName.empty())
        return dieWithMessage(_T("IDS_JAVA_MAIN_CLASS not in resources."));

    std::array<JavaVMOption, 1>jvmopt{
        const_cast<char*>(mainModule.c_str()),
        nullptr
    };

    JavaVMInitArgs vmArgs{
        JNI_VERSION,
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
        &vmArgs);
    if (rc == JNI_ERR) {
        return dieWithMessage(_T("Error creating VM\n. Exiting ..."));
    }

    jclass stringClass =
        env->FindClass("java/lang/String");
    jclass mainClass =
        env->FindClass(mainClassName.c_str());
    if (!mainClass)
        return dieWithMessage(_T("IDS_JAVA_MAIN_CLASS not found."));

    jmethodID mainMethod = env->GetStaticMethodID(
        mainClass,
        "main",
        "([Ljava/lang/String;)V");
    if (!mainMethod)
        return dieWithMessage(_T("No main() found."));

    jobjectArray argv =
        env->NewObjectArray(0, stringClass, 0);

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
