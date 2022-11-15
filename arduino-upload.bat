@echo off
rem This script MUST be run in the same directory as your project code (also known in Mbed Studio as a workspace)
echo.

IF "%2" EQU "" (

	echo Missing arguments. Command syntax: 
	echo.
	echo arduino-upload.bat [COM Port] [Binary_file_to_upload].bin 
	EXIT /b 1 
)

set com_port=
set bin_to_load=

rem Batch files require weird syntax for command expansions...
for /f %%i in ('echo %1 ^| findstr "COM[1-9]"') do (
	set com_port=%%i
)

IF "%com_port%" EQU "" (
	echo The first argument must be a valid COM port string.
	echo Exiting...
	EXIT /b 2
)

for /f %%i in ('echo %2 ^| findstr "bin"') do (
	set bin_to_load=%%i
)

IF "%bin_to_load%" EQU "" (
	echo The second argument must be a binary ^(^.bin^) file.
	echo Exiting...	
	EXIT /b 3
)



for /f %%i in ('dir /s /b BUILD ^| findstr "ARDUINO_NANO33BLE_SENSE" ^| findstr /v /i "mbed-os" ^| findstr "%2"')do (
	set bin_to_load=%%i
)

echo Found binary file: %bin_to_load%
echo Uploading...
arduino-cli.exe upload -p %com_port% -b arduino:mbed_nano:nano33ble -i %bin_to_load% -t -v