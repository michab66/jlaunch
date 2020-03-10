# jlgen

Yet another Java launcher generator for Windows. Unique features are:
* The generated launcher starts an application from a [JLink](https://docs.oracle.com/en/java/javase/11/tools/jlink.html)-generated application image.
* Knows about (jigsaw) modules.
* Works with Java 10+.
* Automatically creates the necessary icon resources from a single .png image.

Many other fine launchers exist.  But after a long search I did not find one that was able to work with Java 11 and application images generated by JLink.

## How to generate a launcher

Get the latest version of the jlgen executable from the project's [release page](https://github.com/michab66/jlaunch/releases).

`jlgen.exe` is a command line application that supports different commands. The command *`MakeLauncher`* represents the central functionality of the software.  It generates a native Windows launcher for a JLink-generated Java application image.

The required parameters are:
* The name of the target file, e.g. 'Farboo.exe'.
* The name of an icon file that is used to generate the required icon resources in the executable, e.g. 'farboo.png'.  Note that a *.png* file is required, no hassle with generating an .ico file.  It is recommended to offer a square high resolution image, though all sizes and resolutions will do.  This image gets scaled and resized for the resolutions 16, 32, 64, 128, 256 pixels.
* The name of the target module. That is, the name of the module that holds the main Java application. For example `app.mmt`.
* Finally, the name of the Java class representing the entry point to the application.  This class has to offer the Java-application's `public static void main( String[] argv )` operation.  An example is `de/michab/app/mmt/Mmt`, note that you have to use `/` in this name instead of the `.`s used for example in `package` references.

A complete sample call may look like 

    `jlgen MakeLauncher C:\cygwin64\tmp\Farboo.exe ..\mmt-icon-1024.png app.mmt de/michab/app/mmt/Mmt`.
    
## I have the launcher executable, what now?
Note that the generated launcher--in our example 'Farboo.exe'--has to be placed in the existing jlink image directory hierarchy at the same position where the file `jvm.dll` is located.  This is currently `{jlink-app-root}/bin/server` but this may change coming versions of the Jdk. (It is a deliberate decision not to look-up the jvm.dll dynamically to prevent to start the target application using a wrong Jdk/Jre that non-deterministically happened to be found.)

In Windows Explorer the launcher is displayed with the application icon that was passed to `jlgen.exe` above.  On a double click your application opens, joy starts :)

The next step for professional application packing is to create a native Windows installer based on the JLink file system including the `jlgen`-generated launcher that has to be configured as the start application.  We use the [WiX toolset](https://wixtoolset.org/) for this purpose, but this is a different story.

See the [project Wiki](https://github.com/michab66/jlaunch/wiki) for more information on the remaining `jlgen` commands and if you want more info on `jlgen` development.

