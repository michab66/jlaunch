// JavaLaunch2.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <jni.h>
#include <iostream>
#include <string>


using namespace std;

static char opt0[] =
"--add-modules=app.mmt";
//"-m app.mmt/de.michab.app.mmt.Mmt";

static char opt1[] =
"app.mmt/de.michab.app.mmt.Mmt";

static JavaVMOption jvmopt[2];

static JavaVMInitArgs vmArgs;

int main() 
{
    jvmopt[0].optionString = opt0;
    jvmopt[1].optionString = opt1;

    vmArgs.version = JNI_VERSION_1_8;
    vmArgs.nOptions = 1;
    vmArgs.options = jvmopt;
    vmArgs.ignoreUnrecognized = JNI_TRUE;

    // Create the JVM
    JavaVM* javaVM = nullptr;
    JNIEnv* env = nullptr;
    long flag = JNI_CreateJavaVM(&javaVM, (void**)
        & env, &vmArgs);
    if (flag == JNI_ERR) {
        cout << "Error creating VM. Exiting...\n";
        return 1;
    }

    jint v = env->GetVersion();

    std::cout << v << std::endl;

    jclass jcls = env->FindClass("java/lang/System");
    if (jcls == NULL) {
        env->ExceptionDescribe();
        javaVM->DestroyJavaVM();
        return 1;
    }
    if (jcls != NULL) {
        jmethodID methodId = env->GetStaticMethodID(
            jcls,
            "getProperty",
            "(Ljava/lang/String;)Ljava/lang/String;");
        if (methodId != NULL) {
            jstring str = env->NewStringUTF("java.home");
            jstring got = (jstring) env->CallStaticObjectMethod(jcls, methodId, str);
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
            }
            // Now convert the Java String to C++ char array 
            const char* cstr = env->GetStringUTFChars(got, 0);
            std::cout << cstr << std::endl;
            env->ReleaseStringUTFChars(got, cstr);
            env->DeleteLocalRef(got);        
        }
    }

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

            env->CallStaticObjectMethod(jcls, methodId, str);
            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
            }
        }
    }

    javaVM->DestroyJavaVM();
    return 0;
}
