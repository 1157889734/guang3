//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ����ģ�� &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*�ļ�����:debug.h

//*�ļ�����:����ģ��

//*�ļ�����:��  ��

//*��������:2007��7��
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DEBUG_H
#define DEBUG_H



//�����ʽ
#define DEBUG_COMMAND_HEAD	0
#define	DEBUG_COMMAND_TAB	' '
#define	DEBUG_COMMAND_END	"\r\n"

//����������
#define DEBUG_ARGV_MAXNUM	32



extern void debug_proc(uint8 *cmd_line);



#endif


