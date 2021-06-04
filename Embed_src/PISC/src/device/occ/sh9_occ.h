
//输入输出端口定义--根据硬件接口改变
#define OCC_BAUDRATE		9600			//波特率
#define OCC_PORTINIT		uart0_init		//通信端口初始化
#define OCC_RECEIVE_CHAR	uart0_read_char		//通信端口接收
#define OCC_SEND_BUF		uart0_write_buf		//通信端口发送
#define OCC_SEND_CHAR		uart0_write_char	//通信端口发送
#define OCC_FIFO_BACKUP	uart0_fifo_out_backup	//底层指针保存
#define OCC_FIFO_RESUME	uart0_fifo_out_resume	//底层指针回读

#define OCC_LED_NO 			4		//occ singal indicate led
#define OCC_BUF_SIZE		255

//虚拟设备类
typedef struct
{
	void (*init)(void);
	void (*proc)(void);
	void (*send_data)(uint8 *,uint8);
	uint8 (*get_playing)(void);
	
	uint16 comm_error;//串口通信错误
	
}occ_dev_t;
extern occ_dev_t sh9_occ_dev;
