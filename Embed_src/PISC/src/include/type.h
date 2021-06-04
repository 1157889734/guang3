//����������������������������������������������������������������������������������������������������������������
//���ļ�����:type.h
//���ļ�˵��:����������������
//���ļ�����:��  ��
//����������:2007��5��
//����������������������������������������������������������������������������������������������������������������������



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



//KEILC51���������ݶ���
#define xdata
#define data
#define idata
#define code const

//������������
typedef unsigned char  uint8;                   /* �޷���8λ���ͱ���                        */
typedef signed   char  int8;                    /* �з���8λ���ͱ���                        */
typedef unsigned short uint16;                  /* �޷���16λ���ͱ���                       */
typedef signed   short int16;                   /* �з���16λ���ͱ���                       */
typedef unsigned int   uint32;                  /* �޷���32λ���ͱ���                       */
typedef signed   int   int32;                   /* �з���32λ���ͱ���                       */
typedef float          fp32;                    /* �����ȸ�������32λ���ȣ�                 */
typedef double         fp64;                    /* ˫���ȸ�������64λ���ȣ�                 */


//��Ԥ����ʱ�Զ����ɱ������� packet_##fn---�Ѳ�ͬ��fn�滻��packet_����
#define __packetlist(nm, fn) \
static packetlist_t packet_##fn={nm, fn}



#endif

