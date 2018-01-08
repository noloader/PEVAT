
robocopy "%~dp0\..\pevat\res" "%~dp0\common" pevat.ico

robocopy "%~dp0\..\Release\x86" "%~dp0\x86" pevat.exe
robocopy "%~dp0\..\Release\x64" "%~dp0\x64" pevat.exe

PAUSE

exit 0