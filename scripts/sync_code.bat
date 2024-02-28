set ROOT=".."
cd %ROOT%

copy Bootloader\User\Driver\Inc\bsp_flash.h Boot\User\Driver\Inc\bsp_flash.h 
copy Bootloader\User\Driver\Src\bsp_flash.c Boot\User\Driver\Src\bsp_flash.c

pause