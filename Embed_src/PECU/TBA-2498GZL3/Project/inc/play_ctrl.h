#ifndef __PLAY_CTRL_H
#define __PLAY_CTRL_H

#include "ISD9XX.h"

#define PLAY_CTRL_PIN			0
#define PLAY_CTRL_PORT			GPB
#define	PLAY_STATUS_PIN			7
#define PLAY_STATUS_PORT		GPA

void play_port_init(void);
void play_ctrl(void);


#endif	/* __PLAY_CTRL_H	*/
