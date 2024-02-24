@echo off
set HexViewPath="..\Tool\Hexview\hexview.exe"
set boot_path="..\Boot\MDK-ARM\Boot\Boot.hex"
set app1_path="..\Bootloader\MDK-ARM\Bootloader\APP_AREA1.hex"
set app2_path="..\Bootloader\MDK-ARM\Bootloader\APP_AREA2.hex"

%HexViewPath% /S /MT:%boot_path%+%app1_path%+%app2_path% /XI:32 -o "firmware.hex"
