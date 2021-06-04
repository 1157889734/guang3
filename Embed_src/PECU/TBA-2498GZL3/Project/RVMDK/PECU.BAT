SET PATH=C:\Keil\ARM\BIN40\;C:\Program Files\ARM\ADSv1_2\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\lib;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\bin;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\lib;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;C:\Program Files\Windows7Master;D:\Program Files\TortoiseSVN\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\CAMCAD;C:\Program Files\ATMEL Corporation\AT91-ISP v1.13\Library\;C:\Program Files\ATMEL Corporation\AT91-ISP v1.13\sam-ba 2.9
SET ARMCC41INC=C:\Keil\ARM\RV31\INC
SET ARMCC41LIB=C:\Keil\ARM\RV31\LIB
SET CPU_TYPE=ISD9160
SET CPU_VENDOR=Nuvoton
SET UV2_TARGET=PECU
SET CPU_CLOCK=0x02EE0000
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\main.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\uart.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\hw_config.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\timer.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\playpcm.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\play_ctrl.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\audio_file.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\pecu.__i"
"C:\Keil\ARM\BIN40\ArmAsm" --Via "..\output\startup_isd91xx._ia"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\core_cm0.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\system_isd9xx.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\retarget.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvgpio.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvsys.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvuart.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvtimer.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvpdma.__i"
"C:\Keil\ARM\BIN40\ArmCC" --Via "..\output\drvdpwm.__i"
"C:\Keil\ARM\BIN40\ArmLink" --Via "..\output\TBA-2498GZL3.lnp"
C:\Keil\ARM\BIN40\fromelf.exe "..\output\TBA-2498GZL3.axf" --i32combined --output "..\output\TBA-2498GZL3.hex"
fromelf --bin --output ..\out\TBA-2498GZL3.bin ..\out\TBA-2498GZL3.axf
