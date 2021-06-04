#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/statfs.h>
#include <semaphore.h>
#include "sys/termios.h"
#include "sys/msg.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "sys/soundcard.h"
#include "sample_comm.h"
#include "hifb.h"
#include "nvp1918.h"
#include "inifile.h"

#include "crc16.h"
#include "gpio.h"
#include "hi3520a_driver.h"
#include "rtspex.h"
#include "socket_driver.h"
#include "uart.h"
#include "ftp.h"

#include "my_tslib.h"
#include "my_rtc.h"
#include "bh_malloc.h"
#include "tftp_Client.h"

#include "process_tcms.h"
#include "process_vvs.h"

#include "log.h"
#include "LcdUart.h"
//#include "errorLog.h"

#include "hwdog.h"
#include "def.h"
#include "process_update.h"

#endif
