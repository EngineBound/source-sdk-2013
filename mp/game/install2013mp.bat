@echo off
echo == Resonance Source SDK Base 2013 Multiplayer Installer ==
echo This batch file will install Source SDK 2013 MP into the folder it was ran from.
echo It will talk directly with Steam and nothing else.
echo If Source SDK 2013 MP does not install the first time after logging in, run the program again.
echo If you cannot see your password while typing that, good! You're not supposed to!
echo.
set /p username="Enter Steam Username: "

IF EXIST "steamcmd\steamcmd.exe" (
	echo Steamcmd found!
) ELSE (
	echo Steamcmd not found! installing...
	mkdir steamcmd
	powershell Invoke-WebRequest -Uri https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip -OutFile steamcmd\steamcmd.zip
	powershell Expand-Archive steamcmd\steamcmd.zip -DestinationPath steamcmd
	rm steamcmd\steamcmd.zip
)

"steamcmd\steamcmd.exe" +login %username% +force_install_dir "%~dp0" +app_update 243750 validate +quit
pause