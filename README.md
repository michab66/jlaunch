# LaunchJava

Yet another Java launcher for Windows. Unique features are:
* Starts an application from a JLink generated installation.
* Works with Java 10+.
* Is small and so simple, that it can be easily adapted to special needs.

# Preconditions
* Requires Microsoft Visual Studio.  Version 19 was used for the implementation.
* Requires a JLink-generated, ready-rolled Java application.  That is, you need to hava a module name and the name of the main class at hand.

# Project config
* For debugging, set the location of the jvm.dll to use.
* Link against the jvm.lib that comes with your Jdk.
* Add the include directory and the include/win32 directory of your Jdk installation to the include path of the Visual Studio project.

# Links that were helpful
https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/intro.html