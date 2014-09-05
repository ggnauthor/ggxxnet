call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat"

copy contrib\vstudio\vc9\zlibvc.def contrib\vstudio\vc8\zlibvc.def
copy contrib\vstudio\vc9\zlib.rc contrib\vstudio\vc8\zlib.rc

cd contrib\masmx86
call bld_ml32.bat

cd ..\..\
devenv contrib\vstudio\vc8\zlibstat.vcproj /Build "Release"
devenv contrib\vstudio\vc8\zlibstat.vcproj /Build "Debug"

pause