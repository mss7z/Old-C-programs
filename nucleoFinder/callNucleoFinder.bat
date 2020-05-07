@echo off
cd /d %~dp0

for /f "usebackq tokens=1-2 delims= " %%a in (`nucleoFinder.exe -e`) do (
	if %%a==TARGET (
		echo target is %%b
	) else if %%a==DRIVE (
		echo drive is %%b
	)
	rem echo %%a
)

cmd /k