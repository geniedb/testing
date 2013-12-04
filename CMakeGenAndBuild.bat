@echo off
if not "%BUILD_TAG%" == "" (
	echo --------------------
	echo Build Tag %BUILD_TAG%
	echo on
) else (
	echo --------------------
	echo Non-Jenkins Build
)

rem If REPO_ROOT is not defined, then the default behavior is to
rem set REPO_ROOT to be the parent directory of the directory
rem containing this script.
set SAVE_REPO_ROOT=%REPO_ROOT%
if "%REPO_ROOT%" == "" (
	set REPO_ROOT=%~dp0%\..\
)
rem if "%REPO_ROOT%" == "" (
rem 	set REPO_ROOT=%CD%\..
rem )
set PROJ_ROOT=%~dp0%\

set BOOST_ROOT=C:/Program Files/boost/boost_1_51
set BUILD_DEST=C:/GenieBuilds/Build64/
set THIRD_PARTY_ROOT=C:/3rdParty/
set PROTOBUF_SRC_ROOT_FOLDER=C:/3rdPartySource/protobuf-2.3.0/protobuf-2.3.0

if "%FrameworkDIR64%" == "" (
	if exist "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\amd64\vcvars64.bat" (
		call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin\amd64\vcvars64.bat"
		echo --------------------
		echo Using VS2012 Compiler
	) else (
		if exist "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat" (
			call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat"
			echo --------------------
			echo Using VS2010 Compiler
		) else (
			pause
			echo --------------------
			echo ERROR - Could not find VC++ v10 or v11 (2010 or 2012)
			exit /B -1
		)
	)
) else (
	echo --------------------
	echo Already selected VS Compiler
)
cd %PROJ_ROOT%
echo ----------------------------------------------
echo Project directory is %PROJ_ROOT%

goto SkipOver

:cmake
	if exist CMakeGenerated (
		rd /s/q CMakeGenerated
	)
	if exist CMakeGenerated (
		echo ERROR - Could not destroy ./CMakeGenerated/
		exit /B -1
	)
	mkdir CMakeGenerated

	pushd CMakeGenerated
	cmake -G "Visual Studio 10 Win64" -DCMAKE_INCLUDE_PATH="/3rdParty/include" -DCMAKE_LIBRARY_PATH="/3rdParty/lib64" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" %PROJ_ROOT%
	popd
goto :eof

:clean
	devenv CMakeGenerated\Testing.sln /Clean
goto :eof

:build
	devenv CMakeGenerated\Testing.sln /Build %1%
goto :eof

:SkipOver
set doAll=
if "%~1%" == "" (
	set doAll=true
)

if /I "%~1%" == "cmake" (
	call :cmake
	shift
) else (
	if "%doAll%" NEQ "" (
		call :cmake
	)
)

if /I "%~1%" == "clean" (
	call :clean
	shift
) else (
	if "%doAll%" NEQ "" (
		call :clean
	)
)

if "%doAll%" NEQ "" (
	call :build Release
	call :build Debug
) else (
	if /I "%~1%" == "debug" (
		call :build Debug
		shift
	)
	if /I "%~1%" == "release" (
		call :build Release
		shift
	)
	if /I "%~1%" == "debug" (
		call :build Debug
		shift
	)
)

set REPO_ROOT=%SAVE_REPO_ROOT%
set SAVE_REPO_ROOT=


