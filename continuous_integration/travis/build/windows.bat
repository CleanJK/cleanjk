echo off

SET ARCH=%1
SET BUILD_TYPE=%2

mkdir build
cd build

if "%ARCH%" == "x64" (
    cmake -G "Visual Studio 15" -A x64 -T host=x64 ..
) else (
    cmake -G "Visual Studio 15" -A Win32 -T host=x86 ..
)

MSBuild.exe ALL_BUILD.vcxproj -p:Configuration=%BUILD_TYPE%