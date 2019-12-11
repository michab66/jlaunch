# jlaunch

Yet another Java launcher for Windows. Unique features are:
* Starts an application from a [JLink](https://docs.oracle.com/en/java/javase/11/tools/jlink.html)-generated application image.
* Knows about (jigsaw) modules.
* Works with Java 10+.
* Is small and simple. Can be easily adapted to special needs.

Many other fine launchers exist.  But after a long search I did not find one that was able to work with Java 11 and application images generated by JLink.

## How to use
The project consists of a Visual Studio project that defines the launcher implementation.  The launcher generated by this project is a small Windows executable that has an icon as well as in its string resources the start class name and the module name of the Java application. The executable launches the referred Java application.

To customize add the icon and the necessary text resource definitions in the project:

* Set the text resource key `IDS_JAVA_MAIN_MODULE` to the name of your module.  An example module name is `app.lumumba`, you can find your application's name in its module-info.java file.
* Set the text resource key `IDS_JAVA_MAIN_CLASS` to the main class of your project.  An example class name is `com/cool/app/Lumumba`, note the slashes instead of the dots in the class name.  The main class has to offer a standard `public static main( String[] )` operation that can be called by the launcher.
* Add your application icon to the resources.

The executable the build generates should be renamed to the name of your application and has to be placed inside the JLink-generated application image into the directory where jvm.dll is located, this is commonly bin/server.

Double clicking the launcher starts the application.

Next step in the journey is to create an installer now that you have a complete Windows application. The [WiX toolset](https://wixtoolset.org/) is a good choice.

## Preconditions
* Requires Microsoft Visual Studio.  Version 19 was used for the implementation.
* Requires a JLink-generated, ready-rolled Java application image.  That is, you need to have a module name and the name of the main class at hand.

## Project config
* For debugging, set the location of the jvm.dll to use.  This is typically the jvm.dll inside your JLink-generated application image.
* For debugging [disable stop on 0xc0000005 Illegal access](https://stackoverflow.com/questions/36250235/exception-0xc0000005-from-jni-createjavavm-jvm-dll).  The Java VM performs deliberate null pointer accesses on startup.
* Make sure the environment variable `JAVA_HOME` is defined since it is used by the project to refer to includes and libraries.

## Links used
* https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/intro.html
* [Dynamic resource replacement on Windows](https://docs.microsoft.com/en-us/windows/win32/menurc/adding-deleting-and-replacing-resources)
* https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagea
* https://social.msdn.microsoft.com/Forums/vstudio/en-US/8da318b3-1c14-4225-859c-138f9b9c749f/resource-injection?forum=winforms

