/*
 * Simplest Java Launcher.
 *
 * Copyright (c) 2019 Michael G. Binz
 *
 * LGPL
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

static JavaVMOption jvmopt[1];
static JavaVMInitArgs vmArgs;

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
    WCHAR* notUsed;

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
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

    jvmopt[0].optionString = 
        const_cast<char *>(mainModule.c_str());

    vmArgs.version = JNI_VERSION_10;
    vmArgs.nOptions = 1;
    vmArgs.options = jvmopt;
    vmArgs.ignoreUnrecognized = JNI_TRUE;

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

    jobjectArray str = env->NewObjectArray(0,stringClass,0);

    env->CallStaticObjectMethod(mainClass, mainMethod, str);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }

    // Waits until the VM terminates.
    javaVM->DestroyJavaVM();

    return 0;
}
