set buildDir=.\build

rem Below the rm command is used, requires an installed cygwin.
rem Found no way to delete a dir tree on windows. rmdir rd del
rem are not cool

IF EXIST %buildDir% rm -rf %buildDir%
md %buildDir%
cd %buildDir%
"%programfiles%"\CMake\bin\cmake.exe -DCMAKE_BUILD_TYPE:STRING=Release -G "Visual Studio 16 2019" ..

cd ..
