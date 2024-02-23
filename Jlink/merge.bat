@echo off
set HexViewPath="..\Tool\Hexview\hexview.exe"
set boot_path="..\Boot\MDK-ARM\Boot\Boot.hex"
set app_path="..\Bootloader\MDK-ARM\Bootloader\Bootloader.hex"

%HexViewPath% /S /MT:%boot_path%+%app_path% /XI:32 -o "firmware.hex"
