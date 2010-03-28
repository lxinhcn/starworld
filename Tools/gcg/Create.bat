@ECHO OFF

if defined LOKI_TMP (
	goto STARTCOMPILING
)

:: Viual C++ 8.0
if defined VS80COMNTOOLS (
	if exist "%VS80COMNTOOLS%\vsvars32.bat" (
		echo -
		echo - Visual C++ 8.0 found.
		echo -
		call "%VS80COMNTOOLS%\vsvars32.bat"
		set LOKI_TMP=true
		goto STARTCOMPILING
	)
)

echo -
echo - No Visual C++ found, please set the enviroment variable 
echo - 
echo - VCToolkitInstallDir  or  VS71COMNTOOLS or VS80COMNTOOLS 
echo - 
echo - to your Visual Studio folder which contains vsvars32.bat.
echo - 
echo - Or call the vsvars32.bat.
echo -

goto ERROR
		


:STARTCOMPILING

:: buid process
mkdir .\inc
mkdir .\lib
del .\temp\*.* /y

if ""%1"" == """" (
	echo debug version
	mkdir .\lib\debug
	set version=-d
	set libdir=.\lib\debug

	) else ( if ""%1"" == ""-r"" (
		echo release version
		mkdir .\lib\release
		set version=-r
		set libdir=.\lib\release
		)
	)

if defined LOKI_TMP (
@ECHO ON
echo - start make library...
..\Debug\MessageCreator.exe %version% -unicode -lib -md -prefix:gcg_ -implib:"gcg.lib" -incdir:".\inc" -libdir:"%libdir%" -I"..\..\Server\xgcBase\Common" -I"..\..\Public" gcg.txt 

goto LEAVE

)

:ERROR
echo -
echo -
echo - An error occured. Compiling aborted.
echo - 
pause



:LEAVE

