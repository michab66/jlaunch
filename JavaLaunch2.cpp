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


#define MAX_LOADSTRING 100

WCHAR szModuleName[MAX_LOADSTRING];
WCHAR szMainClass[MAX_LOADSTRING];

static JavaVMOption jvmopt[2];
static JavaVMInitArgs vmArgs;

static int dieWithMessage( const TCHAR* msg )
{
    TCHAR szExeFileName[MAX_PATH];
    GetModuleFileName(NULL, szExeFileName, MAX_PATH);

    MessageBox(NULL, msg, szExeFileName, MB_OK | MB_APPLMODAL);

    return 1;
}

static std::string getStringResource( HINSTANCE instance, UINT id )
{
    CHAR buffer[MAX_LOADSTRING]{};
    WCHAR* buffer2;

    // Deliberately use LoadStringA to get a result in
    // byte wide platform encoding as required by the
    // JNI invocation API.
    int rcw = LoadStringW(
        instance, 
        id,
        (WCHAR*)(&buffer2), 
        0);

    rcw++;

    std::string result2(rcw , '\0');

    // Deliberately use LoadStringA to get a result in
    // byte wide platform encoding as required by the
    // JNI invocation API.
    int rca = LoadStringA(
        instance, 
        id,
        &result2[0],
        result2.size() );

    if (rcw == rca)
        return std::string{};

    DWORD error = 
        GetLastError();

    // Convert to ASCII.
    std::string result{ 
        buffer };

    return result;
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

    if ( mainModule.length() == 0 )
        return dieWithMessage(_T("IDS_JAVA_MAIN_MODULE not in resources."));
    mainModule = 
        "--add-modules=" + 
        mainModule;

    std::string mainClass = getStringResource(
        hInstance,
        IDS_JAVA_MAIN_CLASS);
    if (mainClass.length() == 0)
        return dieWithMessage(_T("IDS_JAVA_MAIN_CLASS not in resources."));

    jvmopt[0].optionString = 
        (char *)mainModule.c_str();

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
    jclass jcl2s = env->FindClass("de/michab/app/mmt/Mmt");
    if (jcl2s) {
        jmethodID methodId = env->GetStaticMethodID(
            jcl2s,
            "main",
            "([Ljava/lang/String;)V");
        if (methodId != NULL) {
            jobjectArray str = env->NewObjectArray(0,stringClass,0);

            env->CallStaticObjectMethod(jcl2s, methodId, str);
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
            }
        }
    }

    javaVM->DestroyJavaVM();
    return 0;
}
