//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆基本数据类型声明◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
//◆文件名称:type.h
//◆文件说明:基本数据类型声明
//◆文件作者:翟  鹏
//◆创建日期:2007年5月
//◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆



#ifndef TYPE_H
#define TYPE_H



#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL	(void *)0
#endif



//KEILC51编译器兼容定义
#define xdata
#define data
#define idata
#define code const

//基本数据类型
typedef unsigned char  uint8;                   /* 无符号8位整型变量                        */
typedef signed   char  int8;                    /* 有符号8位整型变量                        */
typedef unsigned short uint16;                  /* 无符号16位整型变量                       */
typedef signed   short int16;                   /* 有符号16位整型变量                       */
typedef unsigned int   uint32;                  /* 无符号32位整型变量                       */
typedef signed   int   int32;                   /* 有符号32位整型变量                       */
typedef float          fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         fp64;                    /* 双精度浮点数（64位长度）                 */


//在预处理时自动生成变量名称 packet_##fn---把不同的fn替换到packet_后面
#define __packetlist(nm, fn) \
static packetlist_t packet_##fn={nm, fn}



#endif


