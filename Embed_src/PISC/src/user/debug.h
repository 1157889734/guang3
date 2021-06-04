//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 调试模块 &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//*文件名称:debug.h

//*文件作用:调试模块

//*文件作者:翟  鹏

//*创建日期:2007年7月
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&



#ifndef DEBUG_H
#define DEBUG_H



//命令格式
#define DEBUG_COMMAND_HEAD	0
#define	DEBUG_COMMAND_TAB	' '
#define	DEBUG_COMMAND_END	"\r\n"

//最多参数个数
#define DEBUG_ARGV_MAXNUM	32



extern void debug_proc(uint8 *cmd_line);



#endif


