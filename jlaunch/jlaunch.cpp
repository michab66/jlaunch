/*
 * Java Launcher.
 * $Id$
 *
 * Copyright (c) 2020 Michael G. Binz
 *
 * This is the actual Java process launcher.  A binary version of this
 * executable is part of JLgen.  JLgen populates the neccessary resources
 * when it creates the launcher.
 *
 * Note that the generated launcher does deliberately only work if its
 * in the same directory as the jvm.dll to prevent catching a random
 * jvm.dll in an existing java installation.
 */

#include <array>
#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

 // Define JAVA_HOME in project include paths if this is not found.
#include <jni.h>

 // Get our resource definitions.
#include "jlaunch_resource_ids.h"

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

    int dieWithMessage(const string& msg)
    {
        char szExeFileName[MAX_PATH];

        GetModuleFileNameA(
            nullptr,
            szExeFileName,
            sizeof(szExeFileName));

        MessageBoxA(
            nullptr,
            msg.c_str(),
            szExeFileName,
            MB_OK | MB_APPLMODAL);

        return EXIT_FAILURE;
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
        wchar_t* resourceString =
            nullptr;
        void* castHelper =
            &resourceString;

        // Get the size of the string. We do not use the
        // returned pointer, since it points to wide
        // character data that we do not want to convert.
        int resourceSizeW = LoadStringW(
            instance,
            id,
            static_cast<LPWSTR>(castHelper),
            0);
        if (resourceSizeW == 0)
            throw std::invalid_argument(
                "Resource stringW( " + std::to_string(id) + " ) is missing.");
        // Add one for the terminating NUL.
        resourceSizeW++;
        // Create our result string in the required size.
        std::string result(resourceSizeW, '\0');

        // Now read the actual string converted into the 8bit platform
        // encoding.
        int resourceSizeA = LoadStringA(
            instance,
            id,
            &result[0],
            resourceSizeW);
        if (resourceSizeA == 0)
            throw std::invalid_argument(
                "Unexpected: Resource stringA( " + std::to_string(id) + " ) is missing.");

        return result;
    }

    int launch(HINSTANCE hInstance)
    {
        string mainModule = getStringResource(
            hInstance,
            IDS_JAVA_MAIN_MODULE);

        // Add the reqired option prefix to set the main module.
        mainModule =
            "--add-modules=" +
            mainModule;

        string mainClassName = getStringResource(
            hInstance,
            IDS_JAVA_MAIN_CLASS);

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
            throw std::invalid_argument("Error creating VM.");
        }

        jclass stringClass =
            env->FindClass("java/lang/String");
        jclass mainClass =
            env->FindClass(mainClassName.c_str());
        if (!mainClass) {
            string message =
                "IDS_JAVA_MAIN_CLASS not found: " +
                mainClassName;
            throw std::invalid_argument(message);
        }

        jmethodID mainMethod = env->GetStaticMethodID(
            mainClass,
            "main",
            "([Ljava/lang/String;)V");
        if (!mainMethod)
            throw std::invalid_argument("No main() found.");

        jobjectArray argv =
            env->NewObjectArray(0, stringClass, 0);

        env->CallStaticObjectMethod(
            mainClass,
            mainMethod,
            argv);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            throw std::invalid_argument("No good, got exception from main.");
        }

        // Waits until the VM terminates.
        javaVM->DestroyJavaVM();

        return EXIT_SUCCESS;
    }

} // namespace

/*
 * Windows entry point.
 */
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        return launch(hInstance);
    }
    catch (std::invalid_argument& e) 
    {
        return dieWithMessage(e.what());
    }
}
