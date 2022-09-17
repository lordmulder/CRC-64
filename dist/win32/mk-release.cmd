@echo off
setlocal enabledelayedexpansion
cd "%~dp0\..\..\win32"

if "%MSVC_PATH%"=="" (
	set "MSVC_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC"
)

if not exist "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" (
	echo MSVC not found. Please check MSVC_PATH and try again ^^!^^!^^!
	pause
	goto:eof
)

for %%p in (x86,x64,ARM64) do (
	echo ------------------------------------------------------------------------------
	echo [%%p]
	echo ------------------------------------------------------------------------------
	call "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" %%~p
	for %%t in (Clean,Rebuild) do (
		MSBuild.exe /property:Configuration=Release /property:Platform=%%p /target:%%t /verbosity:normal "%CD%\crc64.sln"
		if not "!ERRORLEVEL!"=="0" goto:BuildFailed
	)
	echo.
)

echo Build completed successfully.
pause
goto:eof

:BuildFailed
echo.
echo Build has failed ^^!^^!^^!
pause
