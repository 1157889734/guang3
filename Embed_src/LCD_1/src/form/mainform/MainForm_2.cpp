#include "MainForm.h"
#include "ui_MainForm.h"
#include "include.h"
#include <QMainWindow>


extern int isFrameChange;
//extern QStringList fileNamesList;
QStringList fileNamesList;



#define PLAY_REALTIME_IDEL			-1
#define PLAY_REALTIME_4D1			0
#define PLAY_REALTIME_HD_CAB		1
#define PLAY_REALTIME_HD_CARRIAGE	2


//static int play_realtime_state = PLAY_REALTIME_IDEL;
//static int play_realtime_state_record = PLAY_REALTIME_IDEL;

MY_RECT_T my_vo_rect[VO_CHANNEL_COUNT];

#define ADDR_MAX_NUM 30
const char *addr[ADDR_MAX_NUM] = { "番禺广场", "市桥", "汉溪长隆", "大石", "厦滘","沥滘","大塘","客村","广州塔\
","珠江新城","体育西路","林和西\
","广州东站\
","燕塘","梅花园\
","京溪南方医院","同和","永泰","白云大道北\
","嘉禾望岗","龙归","人和","高增","机场南\
","机场北\
","石牌桥\
","岗顶","华师","五山","天河客运站\
"};

QString allPageNum;

/**************??????*******************/
int g_addr_num = 0;
bool g_is_press_ATC_or_hander = false;
int g_addr_name_choose_flag = 1;
bool g_flag_press_begin_station_button = false;
bool g_flag_press_next_station_button = false;
bool g_flag_press_end_station_button = false;
//bool g_is_press_driver_talkback_flag = false;
bool g_is_press_client_talkback_flag = false;

/**************???е???****************/
QString ADDR;
QButtonGroup *pushButton_choose_addr_group;
QPushButton *pushButton_choose_addr[ADDR_COUNT];

/**************?????*****************/
QString tempName;
QString indexName;
int g_temp_broadcast_choose_flag = 1;
QButtonGroup *pushButton_choose_temp_broadcast_group;
QPushButton *pushButton_choose_temp_broadcast[TEMP_BROADCAST_COUNT];
QLabel *tempPageIndex;

/**************?????***************/
int g_emergency_num = 0;
int g_emergency_broadcast_choose_flag = 1;
bool g_emergency_broadcast_play_flag = false;
bool g_emergency_broadcast_stop_flag = false;
QButtonGroup *pushButton_choose_emergency_broadcast_group;
QPushButton *pushButton_choose_emergency_broadcast[EMERGENCY_BROADCAST_COUNT];

/*************????????*************/
int g_emergenct_client_num = 0;
bool g_emergency_client_talkback_connect_flag = false;
bool g_emergency_client_talkback_hangup_flag = false;
QButtonGroup *pushButton_choose_emergency_clientTalkBack_group;
QPushButton *pushButton_choose_emergency_clientTalkBack[EMERGENCY_CLIENTTALKBACK_COUNT];

/*************选择路线*************/
int chooseed_line = 99;
const char *choosed_line_route[LINE_COUNT] = {"天河客运站\
-番禺广场","番禺广场-天河客运站\
","机场南\
-体育西路","体育西路-机场南\
","同和-大石","大石-同和","同和-体育西路","体育西路-同和"};

QString choose_line;
QButtonGroup *pushButton_choose_line_group;
QPushButton *pushButton_choose_line[LINE_COUNT];
/**************???????****************/
int g_turnErrorPage = 0;
QString ERROR;
QString errorPageNum;
const char *error_message[ERROR_COUNT] = {"PISC2故障,请检查PISC2设备\
					","PISC1故障,请检查PISC1设备\
					","客室6故障,请检查LCU6设备\
					","客室5故障,请检查LCU5设备\
					","客室4故障,请检查LCU4设备\
					","客室3故障,请检查LCU3设备\
					","客室2故障,请检查LCU2设备\
					","客室1故障,请检查LCU1设备\
					","DCP2故障,请检查DCP2设备\
					","DCP1故障,请检查DCP1设备\
					","APU2通信故障,请检查APU2设备\
					","APU1通信故障,请检查APU1设备\
					","DVA2通信故障,请检查DVA2设备\
					","DVA1通信故障,请检查DVA1设备\
					","IO6设备通信故障,请检查IO6设备\
					","IO5设备通信故障,请检查IO5设备\
					","IO4设备通信故障,请检查IO4设备\
					","IO3设备通信故障,请检查IO3设备\
					","IO2设备通信故障,请检查IO2设备\
					","IO1设备通信故障,请检查IO1设备\
					","ATC2设备通信故障,请检查ATC2设备\
					","ATC1设备通信故障,请检查ATC1设备\
					"," "};

const char *error_messagelist[ERROR_COUNT] = {"     _PISC2故障\
						","     _PISC1故障\
						","     _LCU6故障\
						","     _LCU5故障\
						","     _LCU4故障\
						","     _LCU3故障\
						","     _LCU2故障\
						","     _LCU1故障\
						","     _DCP2故障\
						","     _DCP1故障\
						","     _APU2通信故障\
						","     _APU1通信故障\
						","     _DVA2通信故障\
						","     _DVA1通信故障\
						","     _IO6设备通信故障\
						","     _IO5设备通信故障\
						","     _IO4设备通信故障\
						","     _IO3设备通信故障\
						","     _IO2设备通信故障\
						","     _IO1设备通信故障\
						","     _ATC2设备通信故障\
						","     _ATC1设备通信故障\
						"};

const char *error_message_recover[ERROR_COUNT] = {"     PISC2_故障已修复\
					","     PISC1_故障已修复\
					","     LCU6_故障已修复\
					","     LCU5_故障已修复\
					","     LCU4_故障已修复\
					","     LCU3_故障已修复\
					","     LCU2_故障已修复\
					","     LCU1_故障已修复\
					","     DCP2_故障已修复\
					","     DCP1_故障已修复\
					","     APU2_通信故障已修复\
					","     APU1_通信故障已修复\
					","     DVA2_通信故障已修复\
					","     DVA1_通信故障已修复\
					","     IO6_设备通信故障已修复\
					","     IO5_设备通信故障已修复\
					","     IO4_设备通信故障已修复\
					","     IO3_设备通信故障已修复\
					","     IO2_设备通信故障已修复\
					","     IO1_设备通信故障已修复\
					","     ATC2_设备通信故障已修复\
					","     ATC1_设备通信故障已修复\
					"," "};

const char *turnpageName[7]={"1","2","3","4","5","6","7"};

extern int g_writeRepairedErr[30];


/**************自定义路线*******************/
int g_line9_addr_num = 0;
int g_count = 0;

int g_line9_addr_name_choose_flag = 1;
int g_pCode[25] = {0};
#define CUSTOM_MAX_NUM 	25
char *custom_addr[CUSTOM_MAX_NUM] = {NULL};

QString line9Name;
QButtonGroup *pushButton_line9Name_addr_group;
QPushButton *pushButton_line9Name_addr[LINE9_COUNT];


/**************???****************/
int g_turnLogPage = 0;
int g_log_choose_flag = 1;

const char *log_messagelist[5] = {"     系统启动\
					","     手动报站\
					","     DVA广播报站\
					","     司机对讲\
					","     乘客紧急报警\
					"};
const char *EsuOddLog[9] = {"     奇车A4号门乘客紧急报警\
						","     奇车A5号门乘客紧急报警\
						","     奇车A8号门乘客紧急报警\
						","     奇车B4号门乘客紧急报警\
						","     奇车B5号门乘客紧急报警\
						","     奇车B8号门乘客紧急报警\
						","     奇车C1号门乘客紧急报警\
						","     奇车C4号门乘客紧急报警\
						","     奇车C5号门乘客紧急报警\
						"};

const char *EsuEvenLog[9] = {"     偶车A4号门乘客紧急报警\
						","     偶车A5号门乘客紧急报警\
						","     偶车A8号门乘客紧急报警\
						","     偶车B4号门乘客紧急报警\
						","     偶车B5号门乘客紧急报警\
						","     偶车B8号门乘客紧急报警\
						","     偶车C1号门乘客紧急报警\
						","     偶车C4号门乘客紧急报警\
						","     偶车C5号门乘客紧急报警\
						"};

const char *turnpageNameLog[7]={"1","2","3","4","5","6","7"};

QString logPageNum;
QString logContent;
QButtonGroup *logContent_group;
QPushButton *logContent_num[LOG_COUNT];

/*************错误信息**************/
int g_error_choose_flag = 1;

QString errorContent;
QButtonGroup *errorContent_group;
QPushButton *errorContent_num[ERRORLIST_COUNT];

/*************音量设置************/
#define VOL_MAX_NUM 16
const char *volCueNum[VOL_MAX_NUM]={"0","1","2","3","4","5","6","7","8","9","10",\
									"11","12","13","14","15"};
QString volCur;
int gVolCur = 0;
/*************版本**************/
unsigned char gPiscLevl = 0;
QString PiscLev1;
QString PiscLev2;
QString LCDLev1;
QString LCDLev2;
QProgressBar *progressBar_update_program;



char piscLevl[20] = {0};
char piscLev2[20] = {0};


//收到数据的内容
extern NVRT_TO_VVS_CONTENT_T g_nvrtToVvsContent;

//发送到广播和内容
extern LCD_TO_DCP_CONNECT_T g_LcdtoDcpContent;

//发送到广播合标志位
int g_sendLCDToDcpPackFlag = 0;

//原设备号来判断钥匙
extern unsigned char SrcDeviceNum;//原设备号

//
extern unsigned char recvDate;

/***************************************/
QString mFilename = "/root/usr_exe/ERROR/2001-1.ini";


QPalette pal;

QTimer *show_addr_name;
QTimer *show_client_talkback;
QTimer *show_client_rever_talkback;
QTimer *show_systime_timer;
QTimer *show_error_proc;
QTimer *show_emer_boarding;


bool passenger_alarm_state[PASSENGER_ALARM_COUNT];
bool fire_alarm_state[FIRE_ALARM_COUNT];
bool door_open_alarm_state[DOOR_OPEN_ALARM_COUNT];
typedef struct camera_warnning_state_t
{
	bool passenger_alarm_state;
	bool fire_alarm_state;
	bool door_open_alarm_state;
}CAMERA_WARNNING_STATE_T;
CAMERA_WARNNING_STATE_T camera_warnning_state[CAMERA_COUNT];

typedef struct camera_warnning_show_t
{
	unsigned char warnning_camera_id[CAMERA_COUNT];
	int warnning_camera_count;
}CAMERA_WARNNING_SHOW_T;
CAMERA_WARNNING_SHOW_T my_camera_warnning_show;
CAMERA_WARNNING_SHOW_T my_camera_warnning_show_record;
bool camera_warnning_show_start = false;

typedef struct passenger_alarm_information_t
{
	int count;
	int camera_index[CAMERA_COUNT];
}PASSENGER_ALARM_INFORMATION_T;

PASSENGER_ALARM_INFORMATION_T passenger_alarm_information[PASSENGER_ALARM_COUNT];

typedef struct door_open_alarm_information_t
{
	int count;
	int camera_index[CAMERA_COUNT];
}DOOR_OPEN_ALARM_INFORMATION_T;

DOOR_OPEN_ALARM_INFORMATION_T door_open_alarm_information[DOOR_OPEN_ALARM_COUNT];

typedef struct fire_alarm_information_t
{
	int count;
	int camera_index[CAMERA_COUNT];
}FIRE_ALARM_INFORMATION_T;

FIRE_ALARM_INFORMATION_T fire_alarm_information[FIRE_ALARM_COUNT];

MainForm::MainForm(QWidget *parent) //
		: QWidget(parent),
		ui(new Ui::MainForm)
{
	ui->setupUi(this);//

	bh_malloc_thread_great();

	init_mainform();//?????
}

MainForm::~MainForm()
{
	delete ui;
}

void MainForm::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
#if 0
void Write(QString &Filename)
{
    QFile mFile(Filename);

    if(!mFile.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "could not open file for writing";
        return;
    }

    QTextStream out(&mFile);

    out <<"Hello world";

    mFile.flush();
    mFile.close();
}
void Read(QString &Filename)
{
    QFile mFile(Filename);

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "could not open file for reading";
        return;
    }

    QTextStream in(&mFile);
    QString mText = in.readAll();
    qDebug() << mText;
    mFile.close();
}
#endif
void MainForm::init_mainform()
{
    pal.setColor(QPalette::ButtonText, QColor(255,255,255));   
    ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
    printf("***************************************************\n");
    printf("***************g3 lcd init version:20190314v2.4*********\n");
    printf("***************************************************\n"); 
	
	//初始化按钮
	init_choose_addr_buttons();
	init_choose_line_buttons();
	init_choose_line9_addr_buttons();
	init_logList();
	init_errorList();
	//隐藏按钮
	init_hide_addr_button();
	//init_hide_temp_broadcast_button();
    //for(int i=0 ; i<1000;i++);
	init_hide_log_button();
	init_hide_error_button();
	init_main_button_setEnabled();    
      	init_choose_temp_broadcast_buttons();
       init_hide_temp_broadcast_button();
	init_configuration_file();


	//Start_HWatchDog_thread();
	//ErrorLog_sqlite_init();//数据库初始化
	rtc_init();
	if(!(mkdir("/root/usr_exe/ERROR/",0777))){
		printf("mkdired ERROR\n");
	}
	if(!(mkdir("/root/usr_exe/Log/",0777))){
		printf("mkdired Log\n");
	}
	GpioInit();
	Lcd_Serial_process_Install();/* 收数据 */
	Start_process_tcms_thread();/* 发送数据 */
	
	writeLog(log_messagelist[0]);
	writeError("     SystemOpen");

	sleep(1);
	init_Error_message_check_rep();/* 修复开机好的时间 */
	
	show_addr_name = new QTimer(this);
	connect(show_addr_name, SIGNAL(timeout()),this,SLOT(show_pushButton_addr_name()));
	show_addr_name->start(200);
	
	if( SrcDeviceNum == 0x01 ){
		show_client_talkback = new QTimer(this);
		connect(show_client_talkback, SIGNAL(timeout()),this,SLOT(show_client_talk_flag()));
		show_client_talkback->start(1000);
	}
	else if( SrcDeviceNum == 0x02 ){
		show_client_rever_talkback = new QTimer(this);
		connect(show_client_rever_talkback, SIGNAL(timeout()),this,SLOT(show_client_talk_rever_flag()));
		show_client_rever_talkback->start(1000);
	}

	show_systime_timer = new QTimer(this);
	connect(show_systime_timer, SIGNAL(timeout()), this, SLOT(show_systime_timeoutslot()));
	show_systime_timer->start(1000);

	show_error_proc = new QTimer(this);
	connect(show_error_proc, SIGNAL(timeout()), this, SLOT(show_error_message()));
	show_error_proc->start(3000);
	progressBar_update_program = ui->progressBar_update;
}

void MainForm::init_choose_addr_buttons()
{
    pushButton_choose_addr_group = new QButtonGroup(this);
    pushButton_choose_addr[0] = ui->pushButton_addr_name_station_1;
    pushButton_choose_addr[1] = ui->pushButton_addr_name_station_2;
    pushButton_choose_addr[2] = ui->pushButton_addr_name_station_3;  
    pushButton_choose_addr[3] = ui->pushButton_addr_name_station_4; 
    pushButton_choose_addr[4] = ui->pushButton_addr_name_station_5; 
    pushButton_choose_addr[5] = ui->pushButton_addr_name_station_6;
    pushButton_choose_addr[6] = ui->pushButton_addr_name_station_7;
    pushButton_choose_addr[7] = ui->pushButton_addr_name_station_8;  
    pushButton_choose_addr[8] = ui->pushButton_addr_name_station_9; 
    pushButton_choose_addr[9] = ui->pushButton_addr_name_station_10; 
    pushButton_choose_addr[10] = ui->pushButton_addr_name_station_11;
    pushButton_choose_addr[11] = ui->pushButton_addr_name_station_12;
    pushButton_choose_addr[12] = ui->pushButton_addr_name_station_13;  
    pushButton_choose_addr[13] = ui->pushButton_addr_name_station_14; 
    pushButton_choose_addr[14] = ui->pushButton_addr_name_station_15; 
    pushButton_choose_addr[15] = ui->pushButton_addr_name_station_16;
    pushButton_choose_addr[16] = ui->pushButton_addr_name_station_17;
    pushButton_choose_addr[17] = ui->pushButton_addr_name_station_18;  
    pushButton_choose_addr[18] = ui->pushButton_addr_name_station_19; 
    pushButton_choose_addr[19] = ui->pushButton_addr_name_station_20; 
    pushButton_choose_addr[20] = ui->pushButton_addr_name_station_21;
    pushButton_choose_addr[21] = ui->pushButton_addr_name_station_22;
    pushButton_choose_addr[22] = ui->pushButton_addr_name_station_23;  
    pushButton_choose_addr[23] = ui->pushButton_addr_name_station_24; 
    pushButton_choose_addr[24] = ui->pushButton_addr_name_station_25; 
    pushButton_choose_addr[25] = ui->pushButton_addr_name_station_26;
    pushButton_choose_addr[26] = ui->pushButton_addr_name_station_27;
    pushButton_choose_addr[27] = ui->pushButton_addr_name_station_28;  
    pushButton_choose_addr[28] = ui->pushButton_addr_name_station_29; 
    pushButton_choose_addr[29] = ui->pushButton_addr_name_station_30; 

	for (int i=0;i<ADDR_COUNT;i++)
	{
		pushButton_choose_addr_group->addButton(pushButton_choose_addr[i],i);
	}
	connect(pushButton_choose_addr_group,SIGNAL(buttonClicked(int)),this,SLOT(on_choose_addr_button_group_clicked(int)));
}

#if 1

//????????1000??????????
void MainForm::show_pushButton_addr_name()
{
	if( g_nvrtToVvsContent.code.endStation > 0 && g_nvrtToVvsContent.code.endStation <= 30)
	{
		ui->lineEdit_arrive_next->show();
		ADDR = QString(addr[g_nvrtToVvsContent.code.endStation-1]);
	    ui->lineEdit_arrive_end->setText(ADDR);  
	}
	if( g_nvrtToVvsContent.code.nextStation > 0 && g_nvrtToVvsContent.code.nextStation <= 30)
	{
		ui->lineEdit_arrive_end->show();
	    ADDR = QString(addr[g_nvrtToVvsContent.code.nextStation-1]);
	    ui->lineEdit_arrive_next->setText(ADDR);  
	}   
	//回读音量的状态
	/*
	if( g_nvrtToVvsContent.volume.moni_broadcast_off >= 0 && g_nvrtToVvsContent.volume.moni_broadcast_off<=15 && g_nvrtToVvsContent.volume.volumeFlag == 0)
	{
		volCur = QString(volCueNum[g_nvrtToVvsContent.volume.moni_broadcast_off]);
		printf("!!!!!!!!!!!\n");
		ui->lineEdit_arrive_volume_display->setText(volCur); 
	}*/
	if(gPiscLevl == 1)
	{
		if(g_nvrtToVvsContent.linemode.pis == 0)
		{
			sprintf(piscLevl,"%d.%d",g_nvrtToVvsContent.linemode.version_high,g_nvrtToVvsContent.linemode.version_low);
			printf("message=%s\n",piscLevl);
			PiscLev1= QString(piscLevl);
			LCDLev1 = QString("3.4");
			ui->lineEdit_system_pisc->setText(PiscLev1); 
			ui->lineEdit_system_pisc_3->setText("2.4");
		}
		else if(g_nvrtToVvsContent.linemode.pis == 1)
		{
			sprintf(piscLev2,"%d.%d",g_nvrtToVvsContent.linemode.version_high,g_nvrtToVvsContent.linemode.version_low);
			printf("message=%s\n",piscLev2);
			PiscLev2= QString(piscLev2);
			LCDLev2 = QString("3.4");
			ui->lineEdit_system_pisc_2->setText(PiscLev2); 
			ui->lineEdit_system_pisc_4->setText("2.4");
		}

	}
}

void MainForm::show_client_talk_flag()
{
	static int EsuWriLogFlag[18]  = {0};

    static int cab1[4] = {0},cab2[4] = {0},cab3[4] = {0},cab4[4] = {0},
                cab5[4] = {0},cab6[4] = {0};
	if( g_nvrtToVvsContent.cab.cab_1_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_1_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_1_alarm_3 != 0\
	||  g_nvrtToVvsContent.cab.cab_2_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_2_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_2_alarm_3 != 0\
	||  g_nvrtToVvsContent.cab.cab_3_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_3_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_3_alarm_3 != 0\
	||  g_nvrtToVvsContent.cab.cab_4_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_4_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_4_alarm_3 != 0\
	||  g_nvrtToVvsContent.cab.cab_5_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_5_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_5_alarm_3 != 0\
	||  g_nvrtToVvsContent.cab.cab_6_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_6_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_6_alarm_3 != 0)
	{
	//最右第一个车厢
		if( g_nvrtToVvsContent.cab.cab_1_alarm_1 != 0)//?????01 ?? 10
		{
			EsuWriLogFlag[0] = 1+EsuWriLogFlag[0];
			if(EsuWriLogFlag[0] == 1)
			{
				writeLog(EsuOddLog[0]);
			}
			else if(EsuWriLogFlag[0] == 5)
			{
				EsuWriLogFlag[0] = 2;
			}
		  if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_1 )
		  {
			  if( 0 == cab1[1])
			  {
				  ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
				  cab1[1] = 1;
			  }
			  else if( 1 == cab1[1])
			  {
				  ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
				  cab1[1] = 0;
			  }
		  }
		  else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_1 )
		  {
			  ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		  } 	
		}
		else 
		{
		  ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		}

	  //最右第一个个车厢2
	  if( g_nvrtToVvsContent.cab.cab_1_alarm_2 != 0)//?????01 ?? 10
      {
      		EsuWriLogFlag[1] = 1+EsuWriLogFlag[1];
			if(EsuWriLogFlag[1] == 1)
			{
				writeLog(EsuOddLog[1]);
			}
			else if(EsuWriLogFlag[1] == 5)
			{
				EsuWriLogFlag[1] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_2 )
      	{
          	if( 0 == cab1[1])
			{
			    ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab1[1] = 1;
			}
			else if( 1 == cab1[1])
			{
			    ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab1[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_2 )
		{
			ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_1_alarm_3 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[2] = 1+EsuWriLogFlag[2];
			if(EsuWriLogFlag[2] == 1)
			{
				writeLog(EsuOddLog[2]);
			}
			else if(EsuWriLogFlag[2] == 5)
			{
				EsuWriLogFlag[2] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_3 )
      	{
          	if( 0 == cab1[2])
			{
			    ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab1[2] = 1;
			}
			else if( 1 == cab1[2])
			{
			    ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab1[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_3 )
		{
			ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
    if( g_nvrtToVvsContent.cab.cab_2_alarm_1 != 0)//?????0x01??0x10
      {
            EsuWriLogFlag[3] = 1+EsuWriLogFlag[3];
			if(EsuWriLogFlag[3] == 1)
			{
				writeLog(EsuOddLog[3]);
			}
			else if(EsuWriLogFlag[3] == 5)
			{
				EsuWriLogFlag[3] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_1 )
      	{
          	if( 0 == cab2[0])
			{
			    ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab2[0] = 1;
			}
			else if( 1 == cab2[0])
			{
			    ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab2[0] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_1 )
		{
			ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
    if( g_nvrtToVvsContent.cab.cab_2_alarm_2 != 0)//?????0x01??0x10
      {
      	    EsuWriLogFlag[4] = 1+EsuWriLogFlag[4];
			if(EsuWriLogFlag[4] == 1)
			{
				writeLog(EsuOddLog[4]);
			}
			else if(EsuWriLogFlag[4] == 5)
			{
				EsuWriLogFlag[4] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_2 )
      	{
          	if( 0 == cab2[1])
			{
			    ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab2[1] = 1;
			}
			else if( 1 == cab2[1])
			{
			    ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab2[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_2 )
		{
			ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
    if( g_nvrtToVvsContent.cab.cab_2_alarm_3 != 0)//?????0x01??0x10
      {
      	    EsuWriLogFlag[5] = 1+EsuWriLogFlag[5];
			if(EsuWriLogFlag[5] == 1)
			{
				writeLog(EsuOddLog[5]);
			}
			else if(EsuWriLogFlag[5] == 5)
			{
				EsuWriLogFlag[5] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_3 )
      	{
          	if( 0 == cab2[2])
			{
			    ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab2[2] = 1;
			}
			else if( 1 == cab2[2])
			{
			    ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab2[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_3 )
		{
			ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  } 
    if( g_nvrtToVvsContent.cab.cab_3_alarm_1 != 0)//?????0x01??0x10
      {
      	    EsuWriLogFlag[6] = 1+EsuWriLogFlag[6];
			if(EsuWriLogFlag[6] == 1)
			{
				writeLog(EsuOddLog[6]);
			}
			else if(EsuWriLogFlag[6] == 5)
			{
				EsuWriLogFlag[6] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_1 )
      	{
          	if( 0 == cab3[0])
			{
			    ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab3[0] = 1;
			}
			else if( 1 == cab3[0])
			{
			    ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab3[0] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_1 )
		{
			ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
    if( g_nvrtToVvsContent.cab.cab_3_alarm_2 != 0)//?????0x01??0x10
      {
      	    EsuWriLogFlag[7] = 1+EsuWriLogFlag[7];
			if(EsuWriLogFlag[7] == 1)
			{
				writeLog(EsuOddLog[7]);
			}
			else if(EsuWriLogFlag[7] == 5)
			{
				EsuWriLogFlag[7] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_2 )
      	{
          	if( 0 == cab3[1])
			{
			    ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab3[1] = 1;
			}
			else if( 1 == cab3[1])
			{
			    ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab3[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_2 )
		{
			ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
    if( g_nvrtToVvsContent.cab.cab_3_alarm_3 != 0)//?????0x01??0x10
      {
      	    EsuWriLogFlag[8] = 1+EsuWriLogFlag[8];
			if(EsuWriLogFlag[8] == 1)
			{
				writeLog(EsuOddLog[8]);
			}
			else if(EsuWriLogFlag[8] == 5)
			{
				EsuWriLogFlag[8] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_3 )
      	{
          	if( 0 == cab3[2])
			{
			    ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab3[2] = 1;
			}
			else if( 1 == cab3[2])
			{
			    ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab3[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_3 )
		{
			ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_4_alarm_1 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[9] = 1+EsuWriLogFlag[9];
			if(EsuWriLogFlag[9] == 1)
			{
				writeLog(EsuEvenLog[6]);
			}
			else if(EsuWriLogFlag[9] == 5)
			{
				EsuWriLogFlag[9] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_1 )
      	{
          	if( 0 == cab4[0])
			{
			    ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab4[0] = 1;
			}
			else if( 1 == cab4[0])
			{
			    ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab4[0] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_1 )
		{
			ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_4_alarm_2 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[10] = 1+EsuWriLogFlag[10];
			if(EsuWriLogFlag[10] == 1)
			{
				writeLog(EsuEvenLog[7]);
			}
			else if(EsuWriLogFlag[10] == 5)
			{
				EsuWriLogFlag[10] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_2 )
      	{
          	if( 0 == cab4[1])
			{
			    ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab4[1] = 1;
			}
			else if( 1 == cab4[1])
			{
			    ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab4[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_2 )
		{
			ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_4_alarm_3 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[11] = 1+EsuWriLogFlag[11];
			if(EsuWriLogFlag[11] == 1)
			{
				writeLog(EsuEvenLog[8]);
			}
			else if(EsuWriLogFlag[11] == 5)
			{
				EsuWriLogFlag[11] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_3 )
      	{
          	if( 0 == cab4[2])
			{
			    ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab4[2] = 1;
			}
			else if( 1 == cab4[2])
			{
			    ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab4[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_3 )
		{
			ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_5_alarm_1 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[12] = 1+EsuWriLogFlag[12];
			if(EsuWriLogFlag[12] == 1)
			{
				writeLog(EsuEvenLog[3]);
			}
			else if(EsuWriLogFlag[12] == 5)
			{
				EsuWriLogFlag[12] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_1 )
      	{
          	if( 0 == cab5[0])
			{
			    ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab5[0] = 1;
			}
			else if( 1 == cab5[0])
			{
			    ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab5[0] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_1 )
		{
			ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_5_alarm_2 != 0)//?????01 ?? 10
      {
            EsuWriLogFlag[13] = 1+EsuWriLogFlag[13];
			if(EsuWriLogFlag[13] == 1)
			{
				writeLog(EsuEvenLog[4]);
			}
			else if(EsuWriLogFlag[13] == 5)
			{
				EsuWriLogFlag[13] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_2 )
      	{
          	if( 0 == cab5[1])
			{
			    ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab5[1] = 1;
			}
			else if( 1 == cab5[1])
			{
			    ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab5[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_2 )
		{
			ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else
	  {
		ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_5_alarm_3 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[14] = 1+EsuWriLogFlag[14];
			if(EsuWriLogFlag[14] == 1)
			{
				writeLog(EsuEvenLog[5]);
			}
			else if(EsuWriLogFlag[14] == 5)
			{
				EsuWriLogFlag[14] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_3 )
      	{
          	if( 0 == cab5[2])
			{
			    ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab5[2] = 1;
			}
			else if( 1 == cab5[2])
			{
			    ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab5[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_3 )
		{
			ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_6_alarm_1 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[15] = 1+EsuWriLogFlag[15];
			if(EsuWriLogFlag[15] == 1)
			{
				writeLog(EsuEvenLog[0]);
			}
			else if(EsuWriLogFlag[15] == 5)
			{
				EsuWriLogFlag[15] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_1 )
      	{
          	if( 0 == cab6[0])
			{
			    ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab6[0] = 1;
			}
			else if( 1 == cab6[0])
			{
			    ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab6[0] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_1 )
		{
			ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else
	  {
		ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_6_alarm_2 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[16] = 1+EsuWriLogFlag[16];
			if(EsuWriLogFlag[16] == 1)
			{
				writeLog(EsuEvenLog[1]);
			}
			else if(EsuWriLogFlag[16] == 5)
			{
				EsuWriLogFlag[16] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_2 )
      	{
          	if( 0 == cab6[1])
			{
			    ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab6[1] = 1;
			}
			else if( 1 == cab6[1])
			{
			    ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab6[1] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_2 )
		{
			ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	  if( g_nvrtToVvsContent.cab.cab_6_alarm_3 != 0)//?????01 ?? 10
      {
      	    EsuWriLogFlag[17] = 1+EsuWriLogFlag[17];
			if(EsuWriLogFlag[17] == 1)
			{
				writeLog(EsuEvenLog[2]);
			}
			else if(EsuWriLogFlag[17] == 5)
			{
				EsuWriLogFlag[17] = 2;
			}
      	if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_3 )
      	{
          	if( 0 == cab6[2])
			{
			    ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			    cab6[2] = 1;
			}
			else if( 1 == cab6[2])
			{
			    ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			    cab6[2] = 0;
			}
      	}
		else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_3 )
		{
			ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
		}	  
	  }
	  else 
	  {
		ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
	  }
	}
	else
	{
		for(int j=0;j<18;j++)
		{
			EsuWriLogFlag[j] = 0;
		}
		ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			
	}

 }

void MainForm::show_client_talk_rever_flag()
	{
		static int EsuWriLogFlag[18]= {0};
	
		static int cab1[4] = {0},cab2[4] = {0},cab3[4] = {0},cab4[4] = {0},
					cab5[4] = {0},cab6[4] = {0};
		if( (g_nvrtToVvsContent.cab.cab_1_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_1_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_1_alarm_3 != 0\
		||	g_nvrtToVvsContent.cab.cab_2_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_2_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_2_alarm_3 != 0\
		||	g_nvrtToVvsContent.cab.cab_3_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_3_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_3_alarm_3 != 0\
		||	g_nvrtToVvsContent.cab.cab_4_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_4_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_4_alarm_3 != 0\
		||	g_nvrtToVvsContent.cab.cab_5_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_5_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_5_alarm_3 != 0\
		||	g_nvrtToVvsContent.cab.cab_6_alarm_1 != 0 || g_nvrtToVvsContent.cab.cab_6_alarm_2 != 0 || g_nvrtToVvsContent.cab.cab_6_alarm_3 != 0))
		{
		//最右第一个车厢
			if( g_nvrtToVvsContent.cab.cab_1_alarm_1 != 0)//?????01 ?? 10
			{
				EsuWriLogFlag[0] = 1+EsuWriLogFlag[0];
				if(EsuWriLogFlag[0] == 1)
				{
					writeLog(EsuOddLog[0]);
				}
				else if(EsuWriLogFlag[0] == 5)
				{
					EsuWriLogFlag[0] = 2;
				}
			  if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_1 )
			  {
				  if( 0 == cab1[1])
				  {
					  ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					  cab1[1] = 1;
				  }
				  else if( 1 == cab1[1])
				  {
					  ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					  cab1[1] = 0;
				  }
			  }
			  else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_1 )
			  {
				  ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			  } 	
			}
			else 
			{
			  ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			}
		  //最右第一个个车厢2
		  if( g_nvrtToVvsContent.cab.cab_1_alarm_2 != 0)//?????01 ?? 10
		  {
		  		EsuWriLogFlag[1] = 1+EsuWriLogFlag[1];
				if(EsuWriLogFlag[1] == 1)
				{
					writeLog(EsuOddLog[1]);
				}
				else if(EsuWriLogFlag[1] == 5)
				{
					EsuWriLogFlag[1] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_2 )
			{
				if( 0 == cab1[1])
				{
					ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab1[1] = 1;
				}
				else if( 1 == cab1[1])
				{
					ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab1[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_2 )
			{
				ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  if( g_nvrtToVvsContent.cab.cab_1_alarm_3 != 0)//?????01 ?? 10
		  {
		  		EsuWriLogFlag[2] = 1+EsuWriLogFlag[2];
				if(EsuWriLogFlag[2] == 1)
				{
					writeLog(EsuOddLog[2]);
				}
				else if(EsuWriLogFlag[2] == 5)
				{
					EsuWriLogFlag[2] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_1_alarm_3 )
			{
				if( 0 == cab1[2])
				{
					ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab1[2] = 1;
				}
				else if( 1 == cab1[2])
				{
					ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab1[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_1_alarm_3 )
			{
				ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		//??2?????1??????
		if( g_nvrtToVvsContent.cab.cab_2_alarm_1 != 0)//?????0x01??0x10
		  {
		  		EsuWriLogFlag[3] = 1+EsuWriLogFlag[3];
				if(EsuWriLogFlag[3] == 1)
				{
					writeLog(EsuOddLog[3]);
				}
				else if(EsuWriLogFlag[3] == 5)
				{
					EsuWriLogFlag[3] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_1 )
			{
				if( 0 == cab2[0])
				{
					ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab2[0] = 1;
				}
				else if( 1 == cab2[0])
				{
					ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab2[0] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_1 )
			{
				ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		//??2?????2??????
		if( g_nvrtToVvsContent.cab.cab_2_alarm_2 != 0)//?????0x01??0x10
		  {
		  		EsuWriLogFlag[4] = 1+EsuWriLogFlag[4];
				if(EsuWriLogFlag[4] == 1)
				{
					writeLog(EsuOddLog[4]);
				}
				else if(EsuWriLogFlag[4] == 5)
				{
					EsuWriLogFlag[4] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_2 )
			{
				if( 0 == cab2[1])
				{
					ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab2[1] = 1;
				}
				else if( 1 == cab2[1])
				{
					ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab2[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_2 )
			{
				ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		//??2?????3??????
		if( g_nvrtToVvsContent.cab.cab_2_alarm_3 != 0)//?????0x01??0x10
		  {
		 		EsuWriLogFlag[5] = 1+EsuWriLogFlag[5];
				if(EsuWriLogFlag[5] == 1)
				{
					writeLog(EsuOddLog[5]);
				}
				else if(EsuWriLogFlag[5] == 5)
				{
					EsuWriLogFlag[5] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_2_alarm_3 )
			{
				if( 0 == cab2[2])
				{
					ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab2[2] = 1;
				}
				else if( 1 == cab2[2])
				{
					ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab2[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_2_alarm_3 )
			{
				ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  } 
		//??3?????1??????
		if( g_nvrtToVvsContent.cab.cab_3_alarm_1 != 0)//?????0x01??0x10
		  {
		  		EsuWriLogFlag[6] = 1+EsuWriLogFlag[6];
				if(EsuWriLogFlag[6] == 1)
				{
					writeLog(EsuOddLog[6]);
				}
				else if(EsuWriLogFlag[6] == 5)
				{
					EsuWriLogFlag[6] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_1 )
			{
				if( 0 == cab3[0])
				{
					ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab3[0] = 1;
				}
				else if( 1 == cab3[0])
				{
					ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab3[0] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_1 )
			{
				ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		//??3?????2??????
		if( g_nvrtToVvsContent.cab.cab_3_alarm_2 != 0)//?????0x01??0x10
		  {
		  		EsuWriLogFlag[7] = 1+EsuWriLogFlag[7];
				if(EsuWriLogFlag[7] == 1)
				{
					writeLog(EsuOddLog[7]);
				}
				else if(EsuWriLogFlag[7] == 5)
				{
					EsuWriLogFlag[7] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_2 )
			{
				if( 0 == cab3[1])
				{
					ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab3[1] = 1;
				}
				else if( 1 == cab3[1])
				{
					ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab3[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_2 )
			{
				ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		//??3?????3??????
		if( g_nvrtToVvsContent.cab.cab_3_alarm_3 != 0)//?????0x01??0x10
		  {
		  		EsuWriLogFlag[8] = 1+EsuWriLogFlag[8];
				if(EsuWriLogFlag[8] == 1)
				{
					writeLog(EsuOddLog[8]);
				}
				else if(EsuWriLogFlag[8] == 5)
				{
					EsuWriLogFlag[8] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_3_alarm_3 )
			{
				if( 0 == cab3[2])
				{
					ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab3[2] = 1;
				}
				else if( 1 == cab3[2])
				{
					ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab3[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_3_alarm_3 )
			{
				ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		 //??4?????1??????
		  if( g_nvrtToVvsContent.cab.cab_4_alarm_1 != 0)//?????01 ?? 10
		  {
		 		EsuWriLogFlag[9] = 1+EsuWriLogFlag[9];
				if(EsuWriLogFlag[9] == 1)
				{
					writeLog(EsuEvenLog[6]);
				}
				else if(EsuWriLogFlag[9] == 5)
				{
					EsuWriLogFlag[9] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_1 )
			{
				if( 0 == cab4[0])
				{
					ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab4[0] = 1;
				}
				else if( 1 == cab4[0])
				{
					ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab4[0] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_1 )
			{
				ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??4?????2??????
		  if( g_nvrtToVvsContent.cab.cab_4_alarm_2 != 0)//?????01 ?? 10
		  {
		  		EsuWriLogFlag[10] = 1+EsuWriLogFlag[10];
				if(EsuWriLogFlag[10] == 1)
				{
					writeLog(EsuEvenLog[7]);
				}
				else if(EsuWriLogFlag[10] == 5)
				{
					EsuWriLogFlag[10] = 2;
				}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_2 )
			{
				if( 0 == cab4[1])
				{
					ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab4[1] = 1;
				}
				else if( 1 == cab4[1])
				{
					ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab4[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_2 )
			{
				ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		 //??4?????3??????
		  if( g_nvrtToVvsContent.cab.cab_4_alarm_3 != 0)//?????01 ?? 10
		  {
		    EsuWriLogFlag[11] = 1+EsuWriLogFlag[11];
			if(EsuWriLogFlag[11] == 1)
			{
				writeLog(EsuEvenLog[8]);
			}
			else if(EsuWriLogFlag[11] == 5)
			{
				EsuWriLogFlag[11] = 2;
			}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_4_alarm_3 )
			{
				if( 0 == cab4[2])
				{
					ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab4[2] = 1;
				}
				else if( 1 == cab4[2])
				{
					ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab4[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_4_alarm_3 )
			{
				ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??5?????1??????
		  if( g_nvrtToVvsContent.cab.cab_5_alarm_1 != 0)//?????01 ?? 10
		  {
		  	EsuWriLogFlag[12] = 1+EsuWriLogFlag[12];
			if(EsuWriLogFlag[12] == 1)
			{
				writeLog(EsuEvenLog[3]);
			}
			else if(EsuWriLogFlag[12] == 5)
			{
				EsuWriLogFlag[12] = 2;
			}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_1 )
			{
				if( 0 == cab5[0])
				{
					ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab5[0] = 1;
				}
				else if( 1 == cab5[0])
				{
					ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab5[0] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_1 )
			{
				ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??5?????2??????
		  if( g_nvrtToVvsContent.cab.cab_5_alarm_2 != 0)//?????01 ?? 10
		  {
	        EsuWriLogFlag[13] = 1+EsuWriLogFlag[13];
			if(EsuWriLogFlag[13] == 1)
			{
				writeLog(EsuEvenLog[4]);
			}
			else if(EsuWriLogFlag[13] == 5)
			{
				EsuWriLogFlag[13] = 2;
			}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_2 )
			{
				if( 0 == cab5[1])
				{
					ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab5[1] = 1;
				}
				else if( 1 == cab5[1])
				{
					ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab5[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_2 )
			{
				ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??5?????3??????
		  if( g_nvrtToVvsContent.cab.cab_5_alarm_3 != 0)//?????01 ?? 10
		  {
			  EsuWriLogFlag[14] = 1+EsuWriLogFlag[14];
			  if(EsuWriLogFlag[14] == 1)
			  {
				  writeLog(EsuEvenLog[5]);
			  }
			  else if(EsuWriLogFlag[14] == 5)
			  {
				  EsuWriLogFlag[14] = 2;
			  }

			if( 0x01 == g_nvrtToVvsContent.cab.cab_5_alarm_3 )
			{
				if( 0 == cab5[2])
				{
					ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab5[2] = 1;
				}
				else if( 1 == cab5[2])
				{
					ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab5[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_5_alarm_3 )
			{
				ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??6?????1??????
		  if( g_nvrtToVvsContent.cab.cab_6_alarm_1 != 0)//?????01 ?? 10
		  {
			  EsuWriLogFlag[15] = 1+EsuWriLogFlag[15];
			  if(EsuWriLogFlag[15] == 1)
			  {
				  writeLog(EsuEvenLog[0]);
			  }
			  else if(EsuWriLogFlag[15] == 5)
			  {
				  EsuWriLogFlag[15] = 2;
			  }

			if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_1 )
			{
				if( 0 == cab6[0])
				{
					ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab6[0] = 1;
				}
				else if( 1 == cab6[0])
				{
					ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab6[0] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_1 )
			{
				ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??6?????2??????
		  if( g_nvrtToVvsContent.cab.cab_6_alarm_2 != 0)//?????01 ?? 10
		  {
		    EsuWriLogFlag[16] = 1+EsuWriLogFlag[16];
			if(EsuWriLogFlag[16] == 1)
			{
				writeLog(EsuEvenLog[1]);
			}
			else if(EsuWriLogFlag[16] == 5)
			{
				EsuWriLogFlag[16] = 2;
			}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_2 )
			{
				if( 0 == cab6[1])
				{
					ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab6[1] = 1;
				}
				else if( 1 == cab6[1])
				{
					ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab6[1] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_2 )
			{
				ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		  //??6?????3??????
		  if( g_nvrtToVvsContent.cab.cab_6_alarm_3 != 0)//?????01 ?? 10
		  {
		    EsuWriLogFlag[17] = 1+EsuWriLogFlag[17];
			if(EsuWriLogFlag[17] == 1)
			{
				writeLog(EsuEvenLog[2]);
			}
			else if(EsuWriLogFlag[17] == 5)
			{
				EsuWriLogFlag[17] = 2;
			}
			if( 0x01 == g_nvrtToVvsContent.cab.cab_6_alarm_3 )
			{
				if( 0 == cab6[2])
				{
					ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
					cab6[2] = 1;
				}
				else if( 1 == cab6[2])
				{
					ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
					cab6[2] = 0;
				}
			}
			else if( 0x02 == g_nvrtToVvsContent.cab.cab_6_alarm_3 )
			{
				ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/红铃铛.png);"));
			}	  
		  }
		  else 
		  {
			ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		  }
		}
		else
		{
			for(int j=0;j<18;j++)
			{
				EsuWriLogFlag[j] = 0;
			}

		ui->pushButton_carrage1_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage1_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage1_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage2_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage3_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage4_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage5_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_1->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_2->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
		ui->pushButton_carrage6_phone_3->setStyleSheet(QString("border-image: url(:/蓝铃铛.png);"));
			
		}
	
	 }


//extern tDcpHeader gDcpData;
int tempCheckKey = 1;
void MainForm::show_systime_timeoutslot()
{
	static unsigned char urgenBro_Flag = 0;
	QDateTime time_now = QDateTime::currentDateTime();
	ui->label_systime->setText(time_now.toString("yyyy-MM-dd HH:mm:ss"));
	ui->label_systime_2->setText(time_now.toString("yyyy-MM-dd HH:mm:ss"));
	
	printf("-------------%02x----------------\n",recvDate);
#if 1

	if(recvDate == 0)//收不到数据
	{
		ui->tabWidget->setCurrentIndex(TAB_RECV_DATE);
		tempCheckKey = 1;
	}
	else
	{
	       #if 0
                if(1){
                        ui->pushButton_key->show();
                        if( tempCheckKey == 1 )
                        {
                            ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
                            printf("-----TAB_BROADCAST-------\n");
                            tempCheckKey = 0;//其他状态
                        }
                }
              #else
              
	        //钥匙信号	
		if( g_nvrtToVvsContent.info.activate == 1)
		{
			ui->pushButton_key->show();
			if( tempCheckKey == 1 )
			{
				ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
                printf("-----TAB_BROADCAST-------\n");
				tempCheckKey = 0;//其他状态
			}
		}
		else
		{
			ui->pushButton_key->hide();
			//if( tempCheckKey == 0)
			//{
				ui->tabWidget->setCurrentIndex(TAB_BALCK);
                printf("-----TAB_BALCK0000000-------\n");
				tempCheckKey = 1;//其他状态
			//}
		}
              #endif

        
	}

#endif
	
	//显示模式
	if( g_nvrtToVvsContent.communfault.AtcorHandMode == 1)
	{
		ui->label_hand->hide();
		ui->label_atc->show();
	}
	else if(g_nvrtToVvsContent.communfault.AtcorHandMode == 0)
	{
		ui->label_hand->show();
		ui->label_atc->hide();
	}
	//紧急广播触发位
	if(g_nvrtToVvsContent.trigger.urgentBro == 1)
	{
		if(urgenBro_Flag == 0)
		{
			urgenBro_Flag = 1;
			printf("okokokokokokokokok\n");
			if(g_nvrtToVvsContent.code.urgentBroCode >= 1)
            pushButton_choose_temp_broadcast[g_nvrtToVvsContent.code.urgentBroCode-1]->setStyleSheet(QString("border-image: url(:/yellow_square.png);text-align: center;font:30px;")); 
		}
		else
		{
			urgenBro_Flag = 0;
			if(g_nvrtToVvsContent.code.urgentBroCode >= 1)
            pushButton_choose_temp_broadcast[g_nvrtToVvsContent.code.urgentBroCode-1]->setStyleSheet(QString("border-image: url(0);text-align: center;font:30px;"));
			printf("nonononononononono\n");
		}
	}
	else if(urgenBro_Flag == 1)
	{
		urgenBro_Flag = 0;
		printf("nononosssssssssononono\n");
		if(g_nvrtToVvsContent.code.urgentBroCode >= 1)
		pushButton_choose_temp_broadcast[g_nvrtToVvsContent.code.urgentBroCode-1]->setStyleSheet(QString("border-image: url(0);text-align: center;font:30px;"));
	}
}


void MainForm::show_error_message()
{
	static int ErrorNum[ERROR_COUNT] = {0};
	static int writeErrorFlag[ERROR_COUNT] = {0};
	if( g_nvrtToVvsContent.errorProc.crck2 == 1 && ErrorNum[0] == 0)
    {  

		ErrorNum[0] = 1;
		writeErrorFlag[0] = 1+writeErrorFlag[0];
 
		if(writeErrorFlag[0] == 1)
		{
				    ERROR= QString(error_message[0]);
        	ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[0]);
		}
		else if(writeErrorFlag[0] == 5)
		{
			writeErrorFlag[0] = 2;
		}

    }
	else if( g_nvrtToVvsContent.errorProc.crck1 == 1 && ErrorNum[1] == 0)
    {  

		ErrorNum[1] = 1;
		writeErrorFlag[1] = 1+writeErrorFlag[1];
  
		if(writeErrorFlag[1] == 1)
		{
					ERROR= QString(error_message[1]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[1]);
		}
		else if(writeErrorFlag[1] == 5)
		{
			writeErrorFlag[1] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.srck6 == 1 && ErrorNum[2] == 0)
    {  
 
		ErrorNum[2] = 1;
		writeErrorFlag[2] = 1+writeErrorFlag[2];

		if(writeErrorFlag[2] == 1)
		{
					ERROR= QString(error_message[2]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[2]);
		}
		else if(writeErrorFlag[2] == 5)
		{
			writeErrorFlag[2] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.srck5 == 1 && ErrorNum[3] == 0)
    {  
  
		ErrorNum[3] = 1;
		writeErrorFlag[3] = 1+writeErrorFlag[3];

		if(writeErrorFlag[3] == 1)
		{
					ERROR= QString(error_message[3]);
			ui->pushButton_error->setText(ERROR);


			writeError(error_messagelist[3]);
		}
		else if(writeErrorFlag[3] == 5)
		{
			writeErrorFlag[3] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.srck4 == 1 && ErrorNum[4] == 0)
    {  
 
		ErrorNum[4] = 1;
		writeErrorFlag[4] = 1+writeErrorFlag[4];

		if(writeErrorFlag[4] == 1)
		{

					ERROR= QString(error_message[4]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[4]);
		}
		else if(writeErrorFlag[4] == 5)
		{
			writeErrorFlag[4] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.srck3 == 1 && ErrorNum[5] == 0)
    {  
 
		ErrorNum[5] = 1;
		writeErrorFlag[5] = 1+writeErrorFlag[5];

		if(writeErrorFlag[5] == 1)
		{
					ERROR= QString(error_message[5]);
			ui->pushButton_error->setText(ERROR); 


			writeError(error_messagelist[5]);
		}
		else if(writeErrorFlag[5] == 5)
		{
			writeErrorFlag[5] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.srck2 == 1 && ErrorNum[6] == 0)
    {  
  
		ErrorNum[6] = 1;
		writeErrorFlag[6] = 1+writeErrorFlag[6];
	
		if(writeErrorFlag[6] == 1)
		{

				ERROR= QString(error_message[6]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[6]);
		}
		else if(writeErrorFlag[6] == 5)
		{
			writeErrorFlag[6] = 2;
		}

	
    }
	else if( g_nvrtToVvsContent.errorProc.srck1 == 1 && ErrorNum[7] == 0)
    {   
		ErrorNum[7] = 1;
		writeErrorFlag[7] = 1+writeErrorFlag[7];
	
		if(writeErrorFlag[7] == 1)
		{

				ERROR= QString(error_message[7]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[7]);
		}
		else if(writeErrorFlag[7] == 5)
		{
			writeErrorFlag[7] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.dcp2 == 1 && ErrorNum[8] == 0)
    {  
 
		ErrorNum[8] = 1;
		writeErrorFlag[8] = 1+writeErrorFlag[8];
	
		if(writeErrorFlag[8] == 1)
		{

				ERROR= QString(error_message[8]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[8]);
		}
		else if(writeErrorFlag[8] == 5)
		{
			writeErrorFlag[8] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.dcp1 == 1 && ErrorNum[9] == 0)
    {  

		ErrorNum[9] = 1;
		writeErrorFlag[9] = 1+writeErrorFlag[9];
		
		if(writeErrorFlag[9] == 1)
		{

			ERROR= QString(error_message[9]);
			ui->pushButton_error->setText(ERROR);  

			writeError(error_messagelist[9]);
		}
		else if(writeErrorFlag[9] == 5)
		{
			writeErrorFlag[9] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.apu2 == 1 && ErrorNum[10] == 0)
    {  

		ErrorNum[10] = 1;
		writeErrorFlag[10] = 1+writeErrorFlag[10];
		  
		if(writeErrorFlag[10] == 1)
		{

			ERROR= QString(error_message[10]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[10]);
		}
		else if(writeErrorFlag[10] == 5)
		{
			writeErrorFlag[10] = 2;
		}


    }
	else if( g_nvrtToVvsContent.errorProc.apu1 == 1 && ErrorNum[11] == 0)
    {  

		ErrorNum[11] = 1;
		writeErrorFlag[11] = 1+writeErrorFlag[11];
	 
		if(writeErrorFlag[11] == 1)
		{

				ERROR= QString(error_message[11]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[11]);
		}
		else if(writeErrorFlag[11] == 5)
		{
			writeErrorFlag[11] = 2;

		}


    }
	else if( g_nvrtToVvsContent.errorProc.dva2 == 1 && ErrorNum[12] == 0)
    {  

		ErrorNum[12] = 1;
		writeErrorFlag[12] = 1+writeErrorFlag[12];

		if(writeErrorFlag[12] == 1)
		{

					ERROR= QString(error_message[12]);
			ui->pushButton_error->setText(ERROR);//  临时屏蔽dva故障

			writeError(error_messagelist[12]);
		}
		else if(writeErrorFlag[12] == 5)
		{
			writeErrorFlag[12] = 2;
		}

    }
	else if( g_nvrtToVvsContent.errorProc.dva1 == 1 && ErrorNum[13] == 0)
    {  
  
		ErrorNum[13] = 1;
		writeErrorFlag[13] = 1+writeErrorFlag[13];
		
		if(writeErrorFlag[13] == 1)
		{

			ERROR= QString(error_message[13]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[13]);
		}
		else if(writeErrorFlag[13] == 5)
		{
			writeErrorFlag[13] = 2;

		}
    }
	else if( g_nvrtToVvsContent.errorProc.io6 == 1 && ErrorNum[14] == 0)
    {  
 
		ErrorNum[14] = 1;
		writeErrorFlag[14] = 1+writeErrorFlag[14];
		
		if(writeErrorFlag[14] == 1)
		{
			ERROR= QString(error_message[14]);
			ui->pushButton_error->setText(ERROR); 


			writeError(error_messagelist[14]);
		}
		else if(writeErrorFlag[14] == 5)
		{
			writeErrorFlag[14] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.io5 == 1 && ErrorNum[15] == 0)
    {  

		ErrorNum[15] = 1;
		writeErrorFlag[15] = 1+writeErrorFlag[15];
	
		if(writeErrorFlag[15] == 1)
		{
  				ERROR= QString(error_message[15]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[15]);
		}
		else if(writeErrorFlag[15] == 5)
		{
			writeErrorFlag[15] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.io4 == 1 && ErrorNum[16] == 0)
    {  

		ErrorNum[16] = 1;
		writeErrorFlag[16] = 1+writeErrorFlag[16];
	 
		if(writeErrorFlag[16] == 1)
		{
 				ERROR= QString(error_message[16]);
			ui->pushButton_error->setText(ERROR);

			writeError(error_messagelist[16]);
		}
		else if(writeErrorFlag[16] == 5)
		{
			writeErrorFlag[16] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.io3 == 1 && ErrorNum[17] == 0)
    {  

		ErrorNum[17] = 1;
		writeErrorFlag[17] = 1+writeErrorFlag[17];
	 
		if(writeErrorFlag[17] == 1)
		{

				ERROR= QString(error_message[17]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[17]);
		}
		else if(writeErrorFlag[17] == 5)
		{
			writeErrorFlag[17] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.io2 == 1 && ErrorNum[18] == 0)
    {  
 
		ErrorNum[18] = 1;
		writeErrorFlag[18] = 1+writeErrorFlag[18];
	
		if(writeErrorFlag[18] == 1)
		{
				ERROR= QString(error_message[18]);
			ui->pushButton_error->setText(ERROR); 


			writeError(error_messagelist[18]);
		}
		else if(writeErrorFlag[18] == 5)
		{
			writeErrorFlag[18] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.io1 == 1 && ErrorNum[19] == 0)
    {  

		ErrorNum[19] = 1;
		writeErrorFlag[19] = 1+writeErrorFlag[19];
		 
		if(writeErrorFlag[19] == 1)
		{

			ERROR= QString(error_message[19]);
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[19]);
		}
		else if(writeErrorFlag[19] == 5)
		{
			writeErrorFlag[19] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.atc2 == 1 && ErrorNum[20] == 0)
    {  

		ErrorNum[20] = 1;
		writeErrorFlag[20] = 1+writeErrorFlag[20];
	
		if(writeErrorFlag[20] == 1)
		{
				ERROR= QString(error_message[20]);
			ui->pushButton_error->setText(ERROR);  

			printf("atc2-------------------------------------\n");

			writeError(error_messagelist[20]);
		}
		else if(writeErrorFlag[20] == 5)
		{
			writeErrorFlag[20] = 2;
		}
    }
	else if( g_nvrtToVvsContent.errorProc.atc1 == 1 && ErrorNum[21] == 0)
    {  
 
		ErrorNum[21] = 1;
		writeErrorFlag[21] = 1+writeErrorFlag[21];

		if(writeErrorFlag[21] == 1)
		{

					ERROR= QString(error_message[21]);//闪一下
			ui->pushButton_error->setText(ERROR); 

			writeError(error_messagelist[21]);
		}
		else if(writeErrorFlag[21] == 5)
		{
			writeErrorFlag[21] = 2;
		}
    }
	else
	{
		for(int i=0;i<ERROR_COUNT;i++)
		{
			ErrorNum[i] = 0;
		}		
		ERROR= QString(error_message[22]);// 清空不显示
		ui->pushButton_error->setText(ERROR); 

		printf("clear error again\n");
	}
	
	if( g_nvrtToVvsContent.errorProc.crck2 == 0 && (writeErrorFlag[0] != 0 ))
	{
		writeErrorFlag[0] = 0;
		g_writeRepairedErr[3] = 0;
		writeError(error_message_recover[0]);
	}
	else if( g_nvrtToVvsContent.errorProc.crck1 == 0 && (writeErrorFlag[1] != 0 ))
	{
		writeErrorFlag[1] = 0;
		g_writeRepairedErr[2] = 0;
		writeError(error_message_recover[1]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck6 == 0 && (writeErrorFlag[2] != 0 ))
	{
		writeErrorFlag[2] = 0;
		g_writeRepairedErr[4] = 0;
		writeError(error_message_recover[2]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck5 == 0 && (writeErrorFlag[3] != 0 ))
	{
		writeErrorFlag[3] = 0;
		g_writeRepairedErr[5] = 0;
		writeError(error_message_recover[3]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck4 == 0 && (writeErrorFlag[4] != 0 ))
	{
		writeErrorFlag[4] = 0;
		g_writeRepairedErr[6] = 0;
		writeError(error_message_recover[4]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck3 == 0 && (writeErrorFlag[5] != 0 ))
	{
		writeErrorFlag[5] = 0;
		g_writeRepairedErr[7] = 0;
		writeError(error_message_recover[5]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck2 == 0 && (writeErrorFlag[6] != 0 ))
	{
		writeErrorFlag[6] = 0;
		g_writeRepairedErr[8] = 0;
		writeError(error_message_recover[6]);
	}
	else if( g_nvrtToVvsContent.errorProc.srck1 == 0 && (writeErrorFlag[7] != 0 ))
	{
		writeErrorFlag[7] = 0;
		g_writeRepairedErr[9] = 0;
		writeError(error_message_recover[7]);
	}
	else if( g_nvrtToVvsContent.errorProc.dcp2 == 0 && (writeErrorFlag[8] != 0 ))
	{
		writeErrorFlag[8] = 0;
		g_writeRepairedErr[10] = 0;
		writeError(error_message_recover[8]);
	}
	else if( g_nvrtToVvsContent.errorProc.dcp1 == 0 && (writeErrorFlag[9] != 0 ))
	{
		writeErrorFlag[9] = 0;
		g_writeRepairedErr[11] = 0;
		writeError(error_message_recover[9]);
	}
	else if( g_nvrtToVvsContent.errorProc.apu2 == 0 && (writeErrorFlag[10] != 0 ))
	{
		writeErrorFlag[10] = 0;
		g_writeRepairedErr[12] = 0;
		writeError(error_message_recover[10]);
	}
	else if( g_nvrtToVvsContent.errorProc.apu1 == 0 && (writeErrorFlag[11] != 0 ))
	{
		writeErrorFlag[11] = 0;
		g_writeRepairedErr[13] = 0;
		writeError(error_message_recover[11]);
	}
	else if( g_nvrtToVvsContent.errorProc.dva2 == 0 && (writeErrorFlag[12] != 0 ))
	{
		writeErrorFlag[12] = 0;
		g_writeRepairedErr[1] = 0;
		writeError(error_message_recover[12]);
	}
	else if( g_nvrtToVvsContent.errorProc.dva1 == 0 && (writeErrorFlag[13] != 0 ))
	{
		writeErrorFlag[13] = 0;
		g_writeRepairedErr[0] = 0;//开机检查之前的错误是否修复好
		writeError(error_message_recover[13]);
	}
	else if( g_nvrtToVvsContent.errorProc.io6 == 0 && (writeErrorFlag[14] != 0 ))
	{
		writeErrorFlag[14] = 0;
		g_writeRepairedErr[14] = 0;
		writeError(error_message_recover[14]);
	}
	else if( g_nvrtToVvsContent.errorProc.io5 == 0 && (writeErrorFlag[15] != 0 ))
	{
		writeErrorFlag[15] = 0;
		g_writeRepairedErr[15] = 0;
		writeError(error_message_recover[15]);
	}
	else if( g_nvrtToVvsContent.errorProc.io4 == 0 && (writeErrorFlag[16] != 0 ))
	{
		writeErrorFlag[16] = 0;
		g_writeRepairedErr[16] = 0;
		writeError(error_message_recover[16]);
	}
	else if( g_nvrtToVvsContent.errorProc.io3 == 0 && (writeErrorFlag[17] != 0 ))
	{
		writeErrorFlag[17] = 0;
		g_writeRepairedErr[17] = 0;
		writeError(error_message_recover[17]);
	}
	else if( g_nvrtToVvsContent.errorProc.io2 == 0 && (writeErrorFlag[18] != 0 ))
	{
		writeErrorFlag[18] = 0;
		g_writeRepairedErr[18] = 0;
		writeError(error_message_recover[18]);
	}
	else if( g_nvrtToVvsContent.errorProc.io1 == 0 && (writeErrorFlag[19] != 0 ))
	{
		writeErrorFlag[19] = 0;
		g_writeRepairedErr[19] = 0;
		writeError(error_message_recover[19]);
	}
	else if( g_nvrtToVvsContent.errorProc.atc2 == 0 && (writeErrorFlag[20] != 0 ))
	{
		writeErrorFlag[20] = 0;
		g_writeRepairedErr[20] = 0;
		writeError(error_message_recover[20]);
	}
	else if( g_nvrtToVvsContent.errorProc.atc1 == 0 && (writeErrorFlag[21] != 0 ))
	{
		writeErrorFlag[21] = 0;
		g_writeRepairedErr[21] = 0;
		writeError(error_message_recover[21]);
	}
	else
	{
		for(int i=0;i<30;i++)
		{
			if(g_writeRepairedErr[i]!=0)
			{
				g_writeRepairedErr[i] = 0;
				printf("g_writeRepairedErr[%d]=%d\n",i,g_writeRepairedErr[i]);
			}
		}
	}
	//判断是否全部没错误了
	if( g_nvrtToVvsContent.errorProc.atc1 == 0 && g_nvrtToVvsContent.errorProc.atc2 == 0
	&&  g_nvrtToVvsContent.errorProc.io1 == 0 && g_nvrtToVvsContent.errorProc.io2 == 0
	&&  g_nvrtToVvsContent.errorProc.io3 == 0 && g_nvrtToVvsContent.errorProc.io4 == 0
	&&  g_nvrtToVvsContent.errorProc.io5 == 0 && g_nvrtToVvsContent.errorProc.io6 == 0
	&&  g_nvrtToVvsContent.errorProc.dva1 == 0&& g_nvrtToVvsContent.errorProc.dva2 == 0
	&&  g_nvrtToVvsContent.errorProc.apu1 == 0&& g_nvrtToVvsContent.errorProc.apu2 == 0
	&&  g_nvrtToVvsContent.errorProc.dcp1 == 0&& g_nvrtToVvsContent.errorProc.dcp2 == 0
	&&  g_nvrtToVvsContent.errorProc.srck1 == 0&&g_nvrtToVvsContent.errorProc.srck2 == 0
	&&  g_nvrtToVvsContent.errorProc.srck3 == 0&&g_nvrtToVvsContent.errorProc.srck4 == 0
	&&  g_nvrtToVvsContent.errorProc.srck5 == 0&&g_nvrtToVvsContent.errorProc.srck6 == 0
	&&  g_nvrtToVvsContent.errorProc.crck1 == 0&&g_nvrtToVvsContent.errorProc.crck2 == 0)
	{
		ERROR= QString(error_message[22]);
        ui->pushButton_error->setText(ERROR); 
	}
	printf("g_nvrtToVvsContent.errorProc.atc2=%02x\n",g_nvrtToVvsContent.errorProc.atc2);
}


void MainForm::init_main_button_setEnabled()
{    
	//最下面一排按钮
	ui->pushButton_broadcast_play->show();	
	ui->pushButton_broadcast_stop->show();
	ui->pushButton_temp_broadcast_mode->show();	
	ui->pushButton_select_addr_down->show();	
	ui->pushButton_select_addr_up->show();	
	ui->pushButton_choose_line->show();	
	ui->pushButton_arrive_next_station_2->show(); 
	//最下面一排按钮标签
	ui->label_play->show();
	ui->label_stop->show();
	ui->label_emergency->show();
	ui->label_up->show();
	ui->label_down->show();	
	ui->label_choose_route->show();
	ui->label_next_station_choose->show();
	//下一站
	ui->lineEdit_arrive_next->show(); 
	//终点站
	ui->lineEdit_arrive_end->show(); 
	//atc模式文字
	ui->label_hand->hide();
	ui->label_atc->hide();

	//没选路线不给按确认
	//ui->pushButton_choose_line_affirm->setEnabled(false);
	//主界面错误信息封印
	ui->pushButton_error->setEnabled(false);

	//路线初始化自定义路线 起始站:番禺广场 终点站:天河客运站 
	chooseed_line = 8;
	
	//隐藏鼠标
	ui->tabWidget->setCursor(Qt::BlankCursor);
	//隐藏音量回读状态
	ui->pushButton_volume_up->hide();
	ui->pushButton_volume_down->hide();
	ui->lineEdit_arrive_volume_display->hide();
	ui->volume->hide();
	ui->volume_3->hide();//静音
	//隐藏排序故障
	ui->pushButton_error_start_time_roll->hide();
	ui->pushButton_error_end_time_roll->hide();
	ui->pushButton_error_time_roll->hide();
	ui->pushButton_error_content_2->hide();
	//初始化日志为只读
	ui->textEdit->setReadOnly(true);
#if 0
	ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
#endif
}
void MainForm::Write(QString &Filename)
{
	QFile mFile(Filename);

	if(!mFile.open(QFile::WriteOnly | QFile::Text)) {
		printf("could not open file for write\n");
		return;
	}

	QTextStream out(&mFile);
	out << "Hello world";

	mFile.flush();
	mFile.close();
}

void MainForm::Read(QString &Filename)
{
	QFile mFile(Filename);

	if(!mFile.open(QFile::ReadOnly | QFile::Text)) {
		printf("could not open file for read\n");
		return;
	}

	QTextStream in(&mFile);
	QString mText = in.readAll();

	qDebug() << mText;

	mFile.close();
}


void MainForm::init_Error_message_check_rep()
{

	char bufError[MAX_FILE_SIZE] = {0};
	t_errorLog *errorLog[30];
	char *resultError[2048] = {0};
	int count = 0;
	for(int i=0;i<30;i++)
	{
		errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
		memset(errorLog[i],0,sizeof(t_errorLog));
	}
	
	if(__readError(bufError,resultError,&count,errorLog)!=0)
	{
		printf("readLog error\n");
	}		

	for(int k=0;k<30;k++)
	{
		free(errorLog[k]);
	}
}

/**************************************************隐藏按钮************************************************/
void MainForm::init_hide_addr_button()
{
	for(int i=9;i<30;i++)
	{
		pushButton_choose_addr[i]->hide();
	}
}
void MainForm::init_hide_temp_broadcast_button()
{
	for(int i=0;i<9;i++)
	{
		pushButton_choose_temp_broadcast[i]->setStyleSheet("QPushButton{text-align : center;font:30px;}");
	}
	for(int i=9;i<TEMP_BROADCAST_COUNT;i++)
	{
		pushButton_choose_temp_broadcast[i]->hide();
		pushButton_choose_temp_broadcast[i]->setStyleSheet("QPushButton{text-align : center;font:30px;}");
	}
        tempPageIndex->setText("1");
}

void MainForm::init_hide_log_button()
{	
	for(int i=0;i<15;i++)
	{
		logContent_num[i]->setStyleSheet("QPushButton{text-align : left;}");
	}
	for(int i=15;i<105;i++)
	{
		logContent_num[i]->hide();
		logContent_num[i]->setStyleSheet("QPushButton{text-align : left;}");
	}	
}

void MainForm::init_hide_error_button()
{	
	for(int i=0;i<15;i++)
	{
		errorContent_num[i]->setStyleSheet("QPushButton{text-align : left;}");
	}
	for(int i=15;i<105;i++)
	{
		errorContent_num[i]->hide();
		errorContent_num[i]->setStyleSheet("QPushButton{text-align : left;}");
	}	
}



#endif

void MainForm::on_choose_addr_button_group_clicked(int addr_id)
{
    int i;
    for( i = 0; i < ADDR_COUNT; i++)
    {
        if( addr_id == i )
        {            
            pushButton_choose_addr[addr_id]->setStyleSheet(QString("border-image: url(:/yellow_square.png);"));        
        }
        else
        {
            pushButton_choose_addr[i]->setStyleSheet(QString("border-image: url(0);"));
        }
        g_addr_num = addr_id;
    }
}
unsigned char playback_nvr_style;


void MainForm::on_pushButton_temp_broadcast_mode_clicked()
{
	ui->tabWidget->setCurrentIndex(TAB_TEMP_BROADCAST);
}
void MainForm::on_pushButton_error_clicked()
{
	char bufError[MAX_FILE_SIZE] = {0};
	char *resultError[2048] = {0};
	t_errorLog *errorLog[30]={0};
	for(int i=0;i<30;i++)
	{
		errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
	}
	int count = 0;
	int i = 0,j= 0,temp = 0;
	if(readError(bufError,resultError,&count,errorLog)!=0)
	{
		printf("readLog error\n");
	}	
	printf("count=%d\n",count);
	if(count>105)
	{
		temp = count-105;//116-99=13
	}
	else
	{
		temp = 0;
	}
	for(i=temp ;i<count;i++)
	{
		errorContent = QString(resultError[i]);
		if(count>105)
		{
			j = i-(count-105);//j=13-112-99=0  112   
		}
		else
		{
			j = i;
		}
		errorContent_num[j]->setText(errorContent);
	}
	g_turnErrorPage = count;
	TurnPage();
	ui->tabWidget->setCurrentIndex(TAB_ERROR);
}




void MainForm::on_pushButton_arrive_next_station_2_clicked()
{
	//起始站记录
	if( g_LcdtoDcpContent.code.beginStation == 0) {
		g_LcdtoDcpContent.code.beginStation = g_nvrtToVvsContent.code.beginStation;//把清零的下一站复原
	}
	//终点站记录
	if( g_LcdtoDcpContent.code.endStation == 0) {
		g_LcdtoDcpContent.code.endStation = g_nvrtToVvsContent.code.endStation;//把清零的下一站复原
	}
	//printf("chooseed_line=%d\n",chooseed_line);
	//printf("begin=%d\n",g_LcdtoDcpContent.code.beginStation);
	//printf("end=%d\n",g_nvrtToVvsContent.code.endStation);
	if(chooseed_line == 8) {
		for(int i=0;i<25;i++) {
			g_pCode[i] = 0;
		}
		if(g_count != 0) {
			for(int i=0;i<g_count;i++) {
				if(custom_addr[i] != NULL) {
					free(custom_addr[i]);
					custom_addr[i] = NULL;
					printf("custom_addr[%d] is free\n",i);
				}
			}
		}
		if(g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 25) {
			g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1;
			if( g_count > 0) {
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
			}else {
				g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1;
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();

					g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
				}	
			}
			
		}
		else if((g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation < 12)|| (g_LcdtoDcpContent.code.beginStation < 12 && g_LcdtoDcpContent.code.endStation > 25))
		{
			g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1-14;
			if(g_count < 0)
			{
				g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1-14;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-25;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-25;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-14-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i-14;
				}

			}
			else
			{
					int j = 0;
					for(int i=0;i<12-g_LcdtoDcpContent.code.beginStation;i++)
					{
						ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
						pushButton_line9Name_addr[i]->setText(ADDR); 
						pushButton_line9Name_addr[i]->show();

						g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
					}
					for(int i=12-g_LcdtoDcpContent.code.beginStation;i<g_count;i++)
					{
						ADDR = QString(addr[26-1+j]);
						pushButton_line9Name_addr[i]->setText(ADDR); 
						pushButton_line9Name_addr[i]->show();

						g_pCode[i]= 26+j;
						j++;
					}
			}	
		}
		else if((g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 11)|| (g_LcdtoDcpContent.code.beginStation > 11 && g_LcdtoDcpContent.code.endStation > 25))
		{
			//???-??????
			int j = 0;
			if(g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 11)
			{
				g_count = (g_LcdtoDcpContent.code.beginStation-25)+(g_LcdtoDcpContent.code.endStation-11)+1;
				//g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1-14;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-25;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-25;i<g_count;i++)
				{
					ADDR = QString(addr[11-1+j]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= 11+j;
					j++;
				}

			}
		//??????-???
			else if((g_LcdtoDcpContent.code.beginStation > 11 && g_LcdtoDcpContent.code.endStation > 25))
			{
				int j = 0;
				g_count = (g_LcdtoDcpContent.code.endStation-25)+(g_LcdtoDcpContent.code.beginStation-11)+1;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-10;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-10;i<g_count;i++)
				{
					ADDR = QString(addr[26-1+j]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= 26+j;
					j++;
				}
			}
		}
		//25·????·??
		else
		{
			g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1;
			if(g_count < 0)//??????-???
			{
				g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1;
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();

					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}
			}
			else//???-??????
			{
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
				}
			}	
		}				
		printf("******************************************\n");
		for(int i=0;i<g_count;i++)//?????????
		{
			printf("%d ",g_pCode[i]);
		}
		printf("\n");
		for(int i=0;i<g_count;i++)//??????????
		{
			custom_addr[i] = (char*)calloc(sizeof(addr[g_pCode[i]-1]),10);
			strcpy(custom_addr[i],addr[g_pCode[i]-1]);

			printf("custom_addr[%d]=%s\n ",i,custom_addr[i]);
		}		
		for(int i=g_count;i<CUSTOM_MAX_NUM;i++)
		{
			custom_addr[i] = (char*)calloc(sizeof(addr[g_pCode[i]-1]),10);
			printf("custom_addr[%d]=%s\n ",i,custom_addr[i]);
		}
		printf("******************************************\n");

		if(g_count < 9 )//·????????9?????
		{
			for(int i=g_count;i<25;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
		}
		//???????·?????
		ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)]);
		ui->label_begin_station->setText(ADDR);//?????·??????
		ADDR = QString(addr[(g_LcdtoDcpContent.code.endStation-1)]);
		ui->label_end_station->setText(ADDR);//?????·??????


		
		chooseed_line = 0;
		ui->tabWidget->setCurrentIndex(TAB_LINE9_ADDR);//?????·??
		
   }
   else
   {
		ui->tabWidget->setCurrentIndex(TAB_LINE9_ADDR);//?????·??
   }
	//清零
	g_LcdtoDcpContent.code.beginStation = 0;
	g_LcdtoDcpContent.code.endStation = 0;
}


/*void MainForm::on_pushButton_broadcast_back_2_clicked()
{
	ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
}
*/
//紧急广播界面返回
void MainForm::on_pushButton_broadcast_back_3_clicked()
{
	ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
}
//自定义选择下一站界面返回
void MainForm::on_pushButton_broadcast_back_4_clicked()
{
    ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
}


//up??
void MainForm::on_pushButton_select_addr_up_clicked()
{		
	if( g_nvrtToVvsContent.code.endStation < g_nvrtToVvsContent.code.beginStation)
	{
		g_addr_num = g_nvrtToVvsContent.code.nextStation;
		//体育西路-天可
		if( g_addr_num == 11 &&(g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 26;
		}
		 //体育西-机场北
		else
		{
			g_addr_num++;
		}
		//体育西路-天可清零
		if( g_addr_num > 30 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 1;
		}
		//体育西路-机场北清零
		else if( g_addr_num > 25 && (g_nvrtToVvsContent.code.beginStation <= 25))
		{
			g_addr_num = 1;
		}
		ADDR = QString(addr[g_addr_num-1]);
		g_LcdtoDcpContent.code.nextStation = g_addr_num;
		printf("nextStation=%d\n",g_LcdtoDcpContent.code.nextStation);
		g_sendLCDToDcpPackFlag = 1;
	}
	else if( g_nvrtToVvsContent.code.endStation > g_nvrtToVvsContent.code.beginStation)
	{
		g_addr_num = g_nvrtToVvsContent.code.nextStation;
			
		//天可-体育西
		if( g_addr_num == 26 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 11;
		}
		 //机场北-体育西
		else
		{
			g_addr_num--;
		}
		//天可-体育西清零
		if( g_addr_num <= 0 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = ADDR_MAX_NUM;
		}
		//体育西路-机场北清零
		else if( g_addr_num <= 0 && (g_nvrtToVvsContent.code.endStation <= 25 ))
		{
			g_addr_num = 25;
		}
		ADDR = QString(addr[g_addr_num-1]);
		g_LcdtoDcpContent.code.nextStation = g_addr_num;
		printf("nextStation=%d\n",g_LcdtoDcpContent.code.nextStation);
		g_sendLCDToDcpPackFlag = 1;	
		}
}

//down??
void MainForm::on_pushButton_select_addr_down_clicked()
{ 
	if( g_nvrtToVvsContent.code.endStation > g_nvrtToVvsContent.code.beginStation)
	{
		g_addr_num = g_nvrtToVvsContent.code.nextStation;
		//体育西路-天可
		if( g_addr_num == 11 &&(g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 26;
		}
		 //体育西-机场北
		else
		{
			g_addr_num++;
		}
		//体育西路-天可清零
		if( g_addr_num > 30 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 1;
		}
		//体育西路-机场北清零
		else if( g_addr_num > 25 && (g_nvrtToVvsContent.code.endStation <= 25 ))
		{
			g_addr_num = 1;
		}
		ADDR = QString(addr[g_addr_num-1]);
		g_LcdtoDcpContent.code.nextStation = g_addr_num;
		printf("nextStation=%d\n",g_LcdtoDcpContent.code.nextStation);
		g_sendLCDToDcpPackFlag = 1;
	}
	else if( g_nvrtToVvsContent.code.endStation < g_nvrtToVvsContent.code.beginStation)
	{
		g_addr_num = g_nvrtToVvsContent.code.nextStation;
			
		//天可-体育西
		if( g_addr_num == 26 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = 11;
		}
		 //机场北-体育西
		else
		{
			g_addr_num--;
		}
		//天可-体育西清零
		if( g_addr_num <= 0 && (g_nvrtToVvsContent.code.endStation > 25 || g_nvrtToVvsContent.code.beginStation > 25))
		{
			g_addr_num = ADDR_MAX_NUM;
		}
		//体育西路-机场北清零
		else if( g_addr_num <= 0 && ( g_nvrtToVvsContent.code.beginStation <= 25))
		{
			g_addr_num = 25;
		}
		ADDR = QString(addr[g_addr_num-1]);
		g_LcdtoDcpContent.code.nextStation = g_addr_num;
		printf("nextStation=%d\n",g_LcdtoDcpContent.code.nextStation);
		g_sendLCDToDcpPackFlag = 1;	
	}
	

}




//?????沥???????
void MainForm::on_pushButton_broadcast_play_clicked()
{
    g_LcdtoDcpContent.trigger.leaveBroadcast = 1;//??????????
    g_LcdtoDcpContent.code.nextStation = g_nvrtToVvsContent.code.nextStation;
    g_sendLCDToDcpPackFlag = 1;
//	g_ClearLCDToDcpPackFlag = 1;
	writeLog(log_messagelist[1]);//???:?????????
}

//??????????????
void MainForm::on_pushButton_broadcast_stop_clicked()
{
    g_LcdtoDcpContent.trigger.stopBroadcast= 1;////????????
    g_sendLCDToDcpPackFlag = 1;
}

/*
void MainForm::on_pushButton_arrived_broadcast_clicked()
{
    g_LcdtoDcpContent.trigger.arivedBroadcast = 1;////?????????
    //g_sendLCDToDcpPackFlag = 1;
}
*/
//?????????????


//主界面设置按钮
void MainForm::on_pushButton_systemset_clicked()
{
    ui->tabWidget->setCurrentIndex(TAB_SYSSET);
}

//?????????????/???
void MainForm::on_pushButton_mute_or_monitor_clicked()
{
   /* static int i = 0;
    if( i == 0)
    {
    	
		ui->pushButton_mute_or_monitor->setStyleSheet(QString("border-image: url(:/设置界面_0000_监听静音.png);"));
		ui->label_set_listen->hide();
		ui->label_set_mute->show();
        g_LcdtoDcpContent.trigger.muteBroadcast = 1;//???????      
        //g_sendLCDToDcpPackFlag = 1;
        i = 1;      
    }
    else
    {
		ui->pushButton_mute_or_monitor->setStyleSheet(QString("border-image: url(:/修改/监听按钮.png);"));
		ui->label_set_listen->show();
		ui->label_set_mute->hide();
        g_LcdtoDcpContent.trigger.monitorBroadcast = 1;//????????     
        //g_sendLCDToDcpPackFlag = 1;
       i = 0;//??????                 
    }  */
    ui->tabWidget->setCurrentIndex(TAB_VOLUME);
    
}
void MainForm::on_pushButton_system_message_clicked()
{
	//回读系统信息版本的状态
	gPiscLevl = 1;
		//printf("pis=%d\n",g_nvrtToVvsContent.linemode.pis);
		//printf("version_high=%d\n",g_nvrtToVvsContent.linemode.version_high);
		//printf("version_low=%d\n",g_nvrtToVvsContent.linemode.version_low);
	//sprintf(piscLevl,"pisc %d %d.%d",g_nvrtToVvsContent.linemode.pis,g_nvrtToVvsContent.linemode.version_high,g_nvrtToVvsContent.linemode.version_low);

	init_system_datetime_edit();
    ui->tabWidget->setCurrentIndex(TAB_SYSTEM_MESSAGE);
}

//?????????·????
void MainForm::on_pushButton_choose_line_clicked()
{
		ui->pushButton_choose_route_1->show();
		pushButton_choose_line[0]->hide();
		pushButton_choose_line[1]->hide();
		pushButton_choose_line[2]->hide();
		pushButton_choose_line[3]->hide();
		pushButton_choose_line[4]->hide();
		pushButton_choose_line[5]->hide();
		pushButton_choose_line[6]->hide();
		pushButton_choose_line[7]->hide();
		pushButton_choose_line[8]->show();//隐藏自定义
		pushButton_choose_line[9]->hide();
		pushButton_choose_line[10]->hide();
    ui->tabWidget->setCurrentIndex(TAB_CHOOSE_LINE);
} 

void MainForm::setCurrentFile(const QString &fileName)
//! [46] //! [47]
{
    QString curFile = fileName;
    ui->textEdit->document()->setModified(false);
    //setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}


void MainForm::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#if 1
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->textEdit->setPlainText(in.readAll());
#if 1
    QApplication::restoreOverrideCursor();
#endif
}

void MainForm::TextOpen_Log()
{
	if (ui->tableWidget_select_file_list->currentRow() < 0) return;
    QString fileName("/root/usr_exe/Log/"+ui->tableWidget_select_file_list->currentItem()->text());
    if (!fileName.isEmpty())
        loadFile(fileName);
    
}
void MainForm::TextOpen_Err()
{
	if (ui->tableWidget_select_file_list->currentRow() < 0) return;
    QString fileName("/root/usr_exe/ERROR/"+ui->tableWidget_select_file_list_error->currentItem()->text());
    if (!fileName.isEmpty())
        loadFile(fileName);
    
}


void MainForm::on_pushButton_log_clicked()
{
	Get_Filelist_Log();
	Get_Filelist_Err();
/*
	char buf[MAX_FILE_SIZE] = {0};
	char *result[2048] = {0};
	int count = 0;
	int ret = mkdir("/root/usr_exe/ERROR/",0777);
	if(ret < 0)
	{
		printf("mkdired\n");
	}	

	int i = 0,j= 0,temp=0;
	if(readLog(buf,result,&count)!=0)
	{
		printf("readLog error\n");
	}	
	printf("count=%d\n",count);
	if(count>105)
	{
		temp = count-105;//116-99=13
	}
	else
	{
		temp = 0;
	}
	for(i=temp ;i<count;i++)
	{
		logContent = QString(result[i]);
		if(count>105)
		{
			j = i-(count-105);//j=13-112-99=0  112   
		}
		else
		{
			j = i;
		}
		logContent_num[j]->setText(logContent);
	}
	g_turnLogPage = count;
	TurnPageLog();
*/
	//textEdit = new QPlainTextEdit;
	//setCentralWidget(textEdit);
	
	char DateNameLog[100]={0};
	combineNameLog(DateNameLog);
	QString fileName = QString(DateNameLog);
	loadFile(fileName);
	connect(ui->pushButton_open, SIGNAL(clicked()), this, SLOT(TextOpen_Log()));
	connect(ui->pushButton_open_Error, SIGNAL(clicked()), this, SLOT(TextOpen_Err()));
	
	
	ui->tabWidget->setCurrentIndex(TAB_LOG);
}


//?????滻?????????
void MainForm::page_turn_funcion()
{
     if(  1 == g_addr_name_choose_flag )
     {      
        ui->pushButton_addr_name_station_1->show();
        ui->pushButton_addr_name_station_2->show();
        ui->pushButton_addr_name_station_3->show();
        ui->pushButton_addr_name_station_4->show();
        ui->pushButton_addr_name_station_5->show();
        ui->pushButton_addr_name_station_6->show();
        ui->pushButton_addr_name_station_7->show();
        ui->pushButton_addr_name_station_8->show();
        ui->pushButton_addr_name_station_9->show();

        ui->pushButton_addr_name_station_10->hide();
        ui->pushButton_addr_name_station_11->hide();
        ui->pushButton_addr_name_station_12->hide();
        ui->pushButton_addr_name_station_13->hide();
        ui->pushButton_addr_name_station_14->hide();
        ui->pushButton_addr_name_station_15->hide();
        ui->pushButton_addr_name_station_16->hide();
        ui->pushButton_addr_name_station_17->hide();
        ui->pushButton_addr_name_station_18->hide(); 

        ui->pushButton_addr_name_station_19->hide();
        ui->pushButton_addr_name_station_20->hide();
        ui->pushButton_addr_name_station_21->hide();
        ui->pushButton_addr_name_station_22->hide();
        ui->pushButton_addr_name_station_23->hide();
        ui->pushButton_addr_name_station_24->hide();
        ui->pushButton_addr_name_station_25->hide();
        ui->pushButton_addr_name_station_26->hide();
        ui->pushButton_addr_name_station_27->hide(); 

        ui->pushButton_addr_name_station_28->hide();
        ui->pushButton_addr_name_station_29->hide(); 
        ui->pushButton_addr_name_station_30->hide();
		
     }
     else if( 2 == g_addr_name_choose_flag )
     {
        ui->pushButton_addr_name_station_1->hide();
        ui->pushButton_addr_name_station_2->hide();
        ui->pushButton_addr_name_station_3->hide();
        ui->pushButton_addr_name_station_4->hide();
        ui->pushButton_addr_name_station_5->hide();
        ui->pushButton_addr_name_station_6->hide();
        ui->pushButton_addr_name_station_7->hide();
        ui->pushButton_addr_name_station_8->hide();
        ui->pushButton_addr_name_station_9->hide();

        ui->pushButton_addr_name_station_10->show();
        ui->pushButton_addr_name_station_11->show();
        ui->pushButton_addr_name_station_12->show();
        ui->pushButton_addr_name_station_13->show();
        ui->pushButton_addr_name_station_14->show();
        ui->pushButton_addr_name_station_15->show();
        ui->pushButton_addr_name_station_16->show();
        ui->pushButton_addr_name_station_17->show();
        ui->pushButton_addr_name_station_18->show();

        ui->pushButton_addr_name_station_19->hide();
        ui->pushButton_addr_name_station_20->hide();
        ui->pushButton_addr_name_station_21->hide();
        ui->pushButton_addr_name_station_22->hide();
        ui->pushButton_addr_name_station_23->hide();
        ui->pushButton_addr_name_station_24->hide();
        ui->pushButton_addr_name_station_25->hide();
        ui->pushButton_addr_name_station_26->hide();
        ui->pushButton_addr_name_station_27->hide(); 

        ui->pushButton_addr_name_station_28->hide();
        ui->pushButton_addr_name_station_29->hide(); 
        ui->pushButton_addr_name_station_30->hide();

     }
     else if( 3 == g_addr_name_choose_flag )
     {
        ui->pushButton_addr_name_station_1->hide();
        ui->pushButton_addr_name_station_2->hide();
        ui->pushButton_addr_name_station_3->hide();
        ui->pushButton_addr_name_station_4->hide();
        ui->pushButton_addr_name_station_5->hide();
        ui->pushButton_addr_name_station_6->hide();
        ui->pushButton_addr_name_station_7->hide();
        ui->pushButton_addr_name_station_8->hide();
        ui->pushButton_addr_name_station_9->hide();

        ui->pushButton_addr_name_station_10->hide();
        ui->pushButton_addr_name_station_11->hide();
        ui->pushButton_addr_name_station_12->hide();
        ui->pushButton_addr_name_station_13->hide();
        ui->pushButton_addr_name_station_14->hide();
        ui->pushButton_addr_name_station_15->hide();
        ui->pushButton_addr_name_station_16->hide();
        ui->pushButton_addr_name_station_17->hide();
        ui->pushButton_addr_name_station_18->hide();

        ui->pushButton_addr_name_station_19->show();
        ui->pushButton_addr_name_station_20->show();
        ui->pushButton_addr_name_station_21->show();
        ui->pushButton_addr_name_station_22->show();
        ui->pushButton_addr_name_station_23->show();
        ui->pushButton_addr_name_station_24->show();
        ui->pushButton_addr_name_station_25->show();
        ui->pushButton_addr_name_station_26->show();
        ui->pushButton_addr_name_station_27->show(); 

        ui->pushButton_addr_name_station_28->hide();
        ui->pushButton_addr_name_station_29->hide(); 
        ui->pushButton_addr_name_station_30->hide();

     }
     else if( 4 == g_addr_name_choose_flag )
     {

        ui->pushButton_addr_name_station_1->hide();
        ui->pushButton_addr_name_station_2->hide();
        ui->pushButton_addr_name_station_3->hide();
        ui->pushButton_addr_name_station_4->hide();
        ui->pushButton_addr_name_station_5->hide();
        ui->pushButton_addr_name_station_6->hide();
        ui->pushButton_addr_name_station_7->hide();
        ui->pushButton_addr_name_station_8->hide();
        ui->pushButton_addr_name_station_9->hide();

        ui->pushButton_addr_name_station_10->hide();
        ui->pushButton_addr_name_station_11->hide();
        ui->pushButton_addr_name_station_12->hide();
        ui->pushButton_addr_name_station_13->hide();
        ui->pushButton_addr_name_station_14->hide();
        ui->pushButton_addr_name_station_15->hide();
        ui->pushButton_addr_name_station_16->hide();
        ui->pushButton_addr_name_station_17->hide();
        ui->pushButton_addr_name_station_18->hide();

        ui->pushButton_addr_name_station_19->hide();
        ui->pushButton_addr_name_station_20->hide();
        ui->pushButton_addr_name_station_21->hide();
        ui->pushButton_addr_name_station_22->hide();
        ui->pushButton_addr_name_station_23->hide();
        ui->pushButton_addr_name_station_24->hide();
        ui->pushButton_addr_name_station_25->hide();
        ui->pushButton_addr_name_station_26->hide();
        ui->pushButton_addr_name_station_27->hide(); 

        ui->pushButton_addr_name_station_28->show();
        ui->pushButton_addr_name_station_29->show();        
        ui->pushButton_addr_name_station_30->show();

     }
}

//????????
void MainForm::on_pushButton_addr_name_choose_down_clicked()
{
    g_addr_name_choose_flag++;
    if(  5 == g_addr_name_choose_flag )
    {
        g_addr_name_choose_flag = 1;
    }
	//页码
	allPageNum = QString(turnpageName[g_addr_name_choose_flag-1]);
	ui->label_page_num_now_3->setText(allPageNum); 
	
    page_turn_funcion(); 
    
}

//????????
void MainForm::on_pushButton_addr_name_choose_up_clicked()
{
    g_addr_name_choose_flag--;
    if(  0 == g_addr_name_choose_flag )
    {
        g_addr_name_choose_flag = 4;
    }    
	//页码
	allPageNum = QString(turnpageName[g_addr_name_choose_flag-1]);
	ui->label_page_num_now_3->setText(allPageNum); 
	
    page_turn_funcion(); 
}

//?????????
void MainForm::on_pushButton_addr_name_confirm_clicked()
{
    ADDR = QString(addr[g_addr_num]);
    if( true == g_flag_press_begin_station_button )
    {
		ui->label_begin_name->show();
		ui->label_begin_name->setText(ADDR);//自定义路线选择界面起始站的选择
        g_flag_press_begin_station_button = false;
        g_LcdtoDcpContent.code.beginStation = g_addr_num+1;//起始站代码
        g_LcdtoDcpContent.code.nextStation = g_addr_num+1;//下一站和起始站一样
        
		g_flag_press_end_station_button = true;
		ui->tabWidget->setCurrentIndex(TAB_ADDR_NAME);
        
    }
    else if( true == g_flag_press_next_station_button)
    {
        //ui->lineEdit_arrive_next->setText(ADDR);
//		ui->pushButton_arrive_next_station->setText(ADDR);//??????
        g_flag_press_next_station_button = false;
        //g_LcdtoDcpContent.code.nextStation= g_addr_num+1;//????????
        
		g_sendLCDToDcpPackFlag = 1;
    }
    else if(true == g_flag_press_end_station_button)
    {
        //ui->lineEdit_arrive_end->setText(ADDR);
        ui->label_end_name->show();
		ui->label_end_name->setText(ADDR);//自定义路线选择界面终点站站的选择
        g_flag_press_end_station_button = false;
        g_LcdtoDcpContent.code.endStation= g_addr_num+1;//????????
        g_sendLCDToDcpPackFlag = 1;
        ui->tabWidget->setCurrentIndex(TAB_BROADCAST); 
    } 
       
}

/************************************?????*********************************************/
void MainForm::page_turn_funcion2()
{
    int i = 0;

    for(i=0;i<TEMP_BROADCAST_COUNT;i++)
    {
        if((i/9)==(g_temp_broadcast_choose_flag-1))
            pushButton_choose_temp_broadcast[i]->show();
        else
            pushButton_choose_temp_broadcast[i]->hide();
    }

   indexName = QString("%1").arg(g_temp_broadcast_choose_flag);
   //tempPageIndex->setStyleSheet(QString("text-align: center;font:38px;"));
   tempPageIndex->setText(indexName);
}

void MainForm::on_pushButton_temp_broadcast_choose_down_clicked()
{
    g_temp_broadcast_choose_flag++;
    if((TEMP_BROADCAST_COUNT%9)!=0)
    {
        if( (TEMP_BROADCAST_COUNT/9)+1 < g_temp_broadcast_choose_flag )
            g_temp_broadcast_choose_flag  = 1;
    }
    else
    {
        if((TEMP_BROADCAST_COUNT/9) < g_temp_broadcast_choose_flag )
            g_temp_broadcast_choose_flag = 1;           
    }
    page_turn_funcion2();     
}

void MainForm::on_pushButton_temp_broadcast_choose_up_clicked()
{
    g_temp_broadcast_choose_flag--;
    if(0 == g_temp_broadcast_choose_flag){
        #if 1
        if((TEMP_BROADCAST_COUNT%9)!=0)
            g_temp_broadcast_choose_flag = TEMP_BROADCAST_COUNT/9+1;
        else
            g_temp_broadcast_choose_flag = TEMP_BROADCAST_COUNT/9;
        #endif
        //g_temp_broadcast_choose_flag = 1;
    }    
    page_turn_funcion2(); 
}

void MainForm::on_choose_temp_braodcast_button_group_clicked(int temp_braodcast_id)
{
    int i;
    for( i = 0; i < TEMP_BROADCAST_COUNT; i++)
    {
        if( temp_braodcast_id == i ){            
            pushButton_choose_temp_broadcast[temp_braodcast_id]->setStyleSheet(QString("border-image: url(:/yellow_square.png);text-align: center;font:30px;")); 
        }else{
            pushButton_choose_temp_broadcast[i]->setStyleSheet(QString("border-image: url(0);text-align: center;font:30px;"));
        }
        g_LcdtoDcpContent.code.urgentBroCode = temp_braodcast_id+1;
        printf("temp_braodcast_id=%d\n",temp_braodcast_id+1);
    }
}


void MainForm::on_pushButton_temp_broadcast_play_clicked()
{
    g_LcdtoDcpContent.trigger.stopUrgentBroadcast = 1;//?????????
    g_sendLCDToDcpPackFlag = 1;
	writeLog("     临时广播报站");
}

//??????????????
void MainForm::on_pushButton_temp_broadcast_stop_clicked()
{
    g_LcdtoDcpContent.trigger.stopBroadcast = 1;//?????????
    g_sendLCDToDcpPackFlag = 1;
}

void MainForm::init_choose_temp_broadcast_buttons()
{
    pushButton_choose_temp_broadcast_group = new QButtonGroup(this);
    #if 0
    pushButton_choose_temp_broadcast[0] = ui->pushButton_temp_broadcast_1;
    pushButton_choose_temp_broadcast[1] = ui->pushButton_temp_broadcast_2;
    pushButton_choose_temp_broadcast[2] = ui->pushButton_temp_broadcast_3;  
    pushButton_choose_temp_broadcast[3] = ui->pushButton_temp_broadcast_4; 
    pushButton_choose_temp_broadcast[4] = ui->pushButton_temp_broadcast_5; 
    pushButton_choose_temp_broadcast[5] = ui->pushButton_temp_broadcast_6;
    pushButton_choose_temp_broadcast[6] = ui->pushButton_temp_broadcast_7;
    pushButton_choose_temp_broadcast[7] = ui->pushButton_temp_broadcast_8;  
    pushButton_choose_temp_broadcast[8] = ui->pushButton_temp_broadcast_9; 
    pushButton_choose_temp_broadcast[9] = ui->pushButton_temp_broadcast_10; 
    pushButton_choose_temp_broadcast[10] = ui->pushButton_temp_broadcast_11;
    pushButton_choose_temp_broadcast[11] = ui->pushButton_temp_broadcast_12;
    pushButton_choose_temp_broadcast[12] = ui->pushButton_temp_broadcast_13;  
    pushButton_choose_temp_broadcast[13] = ui->pushButton_temp_broadcast_14; 
    pushButton_choose_temp_broadcast[14] = ui->pushButton_temp_broadcast_15; 
    pushButton_choose_temp_broadcast[15] = ui->pushButton_temp_broadcast_16;
    pushButton_choose_temp_broadcast[16] = ui->pushButton_temp_broadcast_17;
    pushButton_choose_temp_broadcast[17] = ui->pushButton_temp_broadcast_18;  
    pushButton_choose_temp_broadcast[18] = ui->pushButton_temp_broadcast_19; 
    pushButton_choose_temp_broadcast[19] = ui->pushButton_temp_broadcast_20; 
    pushButton_choose_temp_broadcast[20] = ui->pushButton_temp_broadcast_21;
    pushButton_choose_temp_broadcast[21] = ui->pushButton_temp_broadcast_22;
    pushButton_choose_temp_broadcast[22] = ui->pushButton_temp_broadcast_23;  
    pushButton_choose_temp_broadcast[23] = ui->pushButton_temp_broadcast_24; 
    pushButton_choose_temp_broadcast[24] = ui->pushButton_temp_broadcast_25; 
    pushButton_choose_temp_broadcast[25] = ui->pushButton_temp_broadcast_26;
    pushButton_choose_temp_broadcast[26] = ui->pushButton_temp_broadcast_27;
    pushButton_choose_temp_broadcast[27] = ui->pushButton_temp_broadcast_28;  
    pushButton_choose_temp_broadcast[28] = ui->pushButton_temp_broadcast_29; 
    pushButton_choose_temp_broadcast[29] = ui->pushButton_temp_broadcast_30;

    pushButton_choose_temp_broadcast[30] = ui->pushButton_temp_broadcast_31; 
    pushButton_choose_temp_broadcast[31] = ui->pushButton_temp_broadcast_32; 
    pushButton_choose_temp_broadcast[32] = ui->pushButton_temp_broadcast_33; 
    pushButton_choose_temp_broadcast[33] = ui->pushButton_temp_broadcast_34; 
    pushButton_choose_temp_broadcast[34] = ui->pushButton_temp_broadcast_35; 
    pushButton_choose_temp_broadcast[35] = ui->pushButton_temp_broadcast_36; 
    pushButton_choose_temp_broadcast[36] = ui->pushButton_temp_broadcast_37; 
    pushButton_choose_temp_broadcast[37] = ui->pushButton_temp_broadcast_38; 
    pushButton_choose_temp_broadcast[38] = ui->pushButton_temp_broadcast_39; 
    pushButton_choose_temp_broadcast[39] = ui->pushButton_temp_broadcast_40; 
    pushButton_choose_temp_broadcast[40] = ui->pushButton_temp_broadcast_41; 
    pushButton_choose_temp_broadcast[41] = ui->pushButton_temp_broadcast__42; 
    pushButton_choose_temp_broadcast[42] = ui->pushButton_temp_broadcast__43; 
    pushButton_choose_temp_broadcast[43] = ui->pushButton_temp_broadcast__44; 
    pushButton_choose_temp_broadcast[44] = ui->pushButton_temp_broadcast__45; 
    #endif

    tempPageIndex =  new QLabel();
    tempPageIndex->setParent(ui->tab_temp_broadcast);
    tempPageIndex->setGeometry(850,336,71,60);
    QFont font;
    font.setPointSize(35);
    tempPageIndex->setFont(font);
    tempPageIndex->setAlignment(Qt::AlignHCenter);
    
	for (int i=0;i<TEMP_BROADCAST_COUNT;i++)
	{
               switch(i%9)
               {
                    case 0:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_1->x()+150,ui->pushButton_temp_broadcast_1->y()+110,ui->pushButton_temp_broadcast_1->width(),ui->pushButton_temp_broadcast_1->height());
                        break;
                    case 1:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_2->x()+150,ui->pushButton_temp_broadcast_2->y()+110,ui->pushButton_temp_broadcast_2->width(),ui->pushButton_temp_broadcast_2->height());
                        break;

                    case 2:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_3->x()+150,ui->pushButton_temp_broadcast_3->y()+110,ui->pushButton_temp_broadcast_3->width(),ui->pushButton_temp_broadcast_3->height());
                        break;

                    case 3:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_4->x()+150,ui->pushButton_temp_broadcast_4->y()+110,ui->pushButton_temp_broadcast_4->width(),ui->pushButton_temp_broadcast_4->height());
                        break;

                    case 4:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_5->x()+150,ui->pushButton_temp_broadcast_5->y()+110,ui->pushButton_temp_broadcast_5->width(),ui->pushButton_temp_broadcast_5->height());
                        break;

                    case 5:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_6->x()+150,ui->pushButton_temp_broadcast_6->y()+110,ui->pushButton_temp_broadcast_6->width(),ui->pushButton_temp_broadcast_6->height());
                        break;

                    case 6:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_7->x()+150,ui->pushButton_temp_broadcast_7->y()+110,ui->pushButton_temp_broadcast_7->width(),ui->pushButton_temp_broadcast_7->height());
                        break;

                    case 7:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_8->x()+150,ui->pushButton_temp_broadcast_8->y()+110,ui->pushButton_temp_broadcast_8->width(),ui->pushButton_temp_broadcast_8->height());
                        break;

                    case 8:
                        pushButton_choose_temp_broadcast[i] = new QPushButton();
                        pushButton_choose_temp_broadcast[i]->setParent(ui->tab_temp_broadcast);
                        pushButton_choose_temp_broadcast[i]->setGeometry(ui->pushButton_temp_broadcast_9->x()+150,ui->pushButton_temp_broadcast_9->y()+110,ui->pushButton_temp_broadcast_9->width(),ui->pushButton_temp_broadcast_9->height());
                        break;                   

                    default:
                        break;
               }

		pushButton_choose_temp_broadcast_group->addButton(pushButton_choose_temp_broadcast[i],i);
	}
	connect(pushButton_choose_temp_broadcast_group,SIGNAL(buttonClicked(int)),this,SLOT(on_choose_temp_braodcast_button_group_clicked(int)));
}

void MainForm::show_emery_boarding_message()
{
	static unsigned char urgentBro[30] = {0};
	if(g_nvrtToVvsContent.trigger.urgentBro == 1)
	{
		//第一个紧急广播
		if(g_nvrtToVvsContent.code.urgentBroCode == 0x01)
		{
			if(urgentBro[0] == 0)
			{
				urgentBro[0] = 1;
				ui->pushButton_temp_broadcast_1->setStyleSheet(QString("border-image: url(:/yellow_square.png);text-align: left;"));
			}
			else if(urgentBro[0] == 1)
			{
				ui->pushButton_temp_broadcast_1->setStyleSheet(QString("border-image: url(0);text-align: left;"));
				urgentBro[0] = 0;
			}
		}		
	}
	else if(urgentBro[0] == 1)
	{
		ui->pushButton_temp_broadcast_1->setStyleSheet(QString("border-image: url(0);text-align: left;"));
		urgentBro[0] = 0;
	}
	printf("------%2x-------\n",g_nvrtToVvsContent.code.urgentBroCode);
}

void MainForm::init_configuration_file()
{	
	char bufBroName[MAX_FILE_SIZE] = {0};
	char *tempBroNameList[TEMP_BROADCAST_COUNT];
	int count = 0;
	char *cutFlag = "\n";
	int num = 0;

	FILE *fp = fopen("/root/setTempBroName.ini","r+");
	//把文件读进buf
	if(fp == NULL) {
             printf("fopen setTempBroName.ini error!\n");
		return;
	}
	printf("init_configuration_file!!!!!!!!\n");
	bufBroName[count] = fgetc(fp);
	while(feof(fp)==0)
	{	
		count++;
		bufBroName[count] = fgetc(fp);
	}
	bufBroName[count]='\0';
	//把buf切割
	tempBroNameList[num] = strtok(bufBroName,cutFlag);	
	while(tempBroNameList[num]!=NULL)
	{
		num++;
		if(num>TEMP_BROADCAST_COUNT) {
			break;
		}
		tempBroNameList[num] = strtok(NULL,cutFlag);
	}
	printf("num=%d\n",num); 
	
	for(int i=0;i<TEMP_BROADCAST_COUNT;i++){
              if(i<num){
		    tempName= QString(tempBroNameList[i]);
		    pushButton_choose_temp_broadcast[i]->setText(tempName);
              }
              else
              {
                    tempName = QString("%1").arg(i+1);
                    pushButton_choose_temp_broadcast[i]->setText(tempName);
              }
              pushButton_choose_temp_broadcast[i]->setStyleSheet(QString("font:30px"));
	}
	fclose(fp);

        ui->pushButton_temp_broadcast_1->hide();
        ui->pushButton_temp_broadcast_2->hide();
        ui->pushButton_temp_broadcast_3->hide();
        ui->pushButton_temp_broadcast_4->hide();
        ui->pushButton_temp_broadcast_5->hide();
        ui->pushButton_temp_broadcast_6->hide();
        ui->pushButton_temp_broadcast_7->hide();
        ui->pushButton_temp_broadcast_8->hide();
        ui->pushButton_temp_broadcast_9->hide();
}

int MainForm::cmd_dir_Log()
{
	fileNamesList.clear();
	
	QString path="/root/usr_exe/Log";
	QDir *dir=new QDir(path);
	QStringList filter;
	filter<<"*.ini";
	dir->setNameFilters(filter);
	QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
	qDebug()<<fileInfo->count();
	for(int i=0;i<fileInfo->count();i++) {
		qDebug()<<fileInfo->at(i).fileName();
		fileNamesList.append(fileInfo->at(i).fileName());
	}
	return 1;
}

int MainForm::cmd_dir_Err()
{
	fileNamesList.clear();
	
	QString path="/root/usr_exe/ERROR";
	QDir *dir=new QDir(path);
	QStringList filter;
	filter<<"*.ini";
	dir->setNameFilters(filter);
	QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
	qDebug()<<fileInfo->count();
	for(int i=0;i<fileInfo->count();i++) {
		qDebug()<<fileInfo->at(i).fileName();
		fileNamesList.append(fileInfo->at(i).fileName());
	}
	return 1;
}

void MainForm::Get_Filelist_Log()
{
	ui->tableWidget_select_file_list->clear();
	ui->tableWidget_select_file_list->setRowCount(0);

	cmd_dir_Log();

	ui->tableWidget_select_file_list->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget_select_file_list->setSelectionMode(QAbstractItemView::SingleSelection);

	ui->tableWidget_select_file_list->setRowCount(fileNamesList.count());
	if (fileNamesList.count()>0)
	{
		for (int i=0; i<fileNamesList.count(); i++)
		{
			ui->tableWidget_select_file_list->setItem(i, 0, new QTableWidgetItem(fileNamesList[i],0));
		}
		ui->tableWidget_select_file_list->sortByColumn(0);
		ui->tableWidget_select_file_list->setCurrentCell(0, 0);
	}
}
void MainForm::Get_Filelist_Err()
{
	ui->tableWidget_select_file_list_error->clear();
	ui->tableWidget_select_file_list_error->setRowCount(0);

	cmd_dir_Err();

	ui->tableWidget_select_file_list_error->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget_select_file_list_error->setSelectionMode(QAbstractItemView::SingleSelection);

	ui->tableWidget_select_file_list_error->setRowCount(fileNamesList.count());
	if (fileNamesList.count()>0)
	{
		for (int i=0; i<fileNamesList.count(); i++)
		{
			ui->tableWidget_select_file_list_error->setItem(i, 0, new QTableWidgetItem(fileNamesList[i],0));
		}
		ui->tableWidget_select_file_list_error->sortByColumn(0);
		ui->tableWidget_select_file_list_error->setCurrentCell(0, 0);
	}
}



/*************************************选择路线界面**********************************************/
void MainForm::init_choose_line_buttons()
{
    pushButton_choose_line_group = new QButtonGroup(this);
    pushButton_choose_line[0] = ui->pushButton_choose_line_1;
    pushButton_choose_line[1] = ui->pushButton_choose_line_2;
    pushButton_choose_line[2] = ui->pushButton_choose_line_3;  
    pushButton_choose_line[3] = ui->pushButton_choose_line_4; 
    pushButton_choose_line[4] = ui->pushButton_choose_line_5; 
    pushButton_choose_line[5] = ui->pushButton_choose_line_6;
    pushButton_choose_line[6] = ui->pushButton_choose_line_7;
    pushButton_choose_line[7] = ui->pushButton_choose_line_8;
    pushButton_choose_line[8] = ui->pushButton_choose_line_9;
	pushButton_choose_line[9] = ui->pushButton_choose_line_10;
	pushButton_choose_line[10] = ui->pushButton_choose_line_11;
    
	for (int i=0;i<LINE_COUNT;i++)
	{
		pushButton_choose_line_group->addButton(pushButton_choose_line[i],i);
	}
	connect(pushButton_choose_line_group,SIGNAL(buttonClicked(int)),this,SLOT(on_init_choose_line_buttons_clicked(int)));
}

//???·??????水?
void MainForm::on_pushButton_broadcast_back_5_clicked()
{
    ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
} 

//??а????
void MainForm::on_init_choose_line_buttons_clicked( int choose_line_id)
{        
    int i;
    for( i = 0; i < LINE_COUNT; i++)
    {
        if( choose_line_id == i )
        {            
            pushButton_choose_line[choose_line_id]->setStyleSheet(QString("border-image: url(:/yellow_square.png);"));        
        }
        else
        {
            pushButton_choose_line[i]->setStyleSheet(QString("border-image: url(0);"));
        }
    }
   
    chooseed_line = choose_line_id;
      
}

void MainForm::update_choose_next_station()
{
	if( chooseed_line == 0 || chooseed_line == 1 || chooseed_line == 2 || chooseed_line == 3 || chooseed_line == 4
	   || chooseed_line == 5 || chooseed_line == 6 || chooseed_line == 7 || chooseed_line == 8|| chooseed_line == 9|| chooseed_line == 10)//?????·??
	{
   	for(int i=0;i<25;i++)
   	{
   		g_pCode[i] = 0;
   	}
	if(g_count != 0)
	{
		for(int i=0;i<g_count;i++)
		{
			if(custom_addr[i] != NULL)//?????????????
			{
				free(custom_addr[i]);
				custom_addr[i] = NULL;
				printf("custom_addr[%d] is free\n",i);
			}
		}
	}
   	if(g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 25)
	{
		g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1;
		// ???-?????
		if( g_count > 0)
		{
			for(int i=0;i<g_count;i++)
			{
				ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
				pushButton_line9Name_addr[i]->setText(ADDR); 
				pushButton_line9Name_addr[i]->show();
				
				g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;//?????????
			}				
		}
		//???-???
		else
		{
			g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1;
			for(int i=0;i<g_count;i++)
			{
				ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
				pushButton_line9Name_addr[i]->setText(ADDR); 
				pushButton_line9Name_addr[i]->show();

				g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;//?????code?????????
			}	
		}
		
		}
   		else if((g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation < 12)|| (g_LcdtoDcpContent.code.beginStation < 12 && g_LcdtoDcpContent.code.endStation > 25))
   		{
   			g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1-14;
   			if(g_count < 0)//???-???
   			{
   				g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1-14;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-25;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-25;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-14-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i-14;
				}
   			}
			else
			{
				if(g_count > 11)//???-???
				{
					int j = 0;
					for(int i=0;i<12-g_LcdtoDcpContent.code.beginStation;i++)
					{
						ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
						pushButton_line9Name_addr[i]->setText(ADDR); 
						pushButton_line9Name_addr[i]->show();

						g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
					}
					for(int i=12-g_LcdtoDcpContent.code.beginStation;i<g_count;i++)
					{
						ADDR = QString(addr[26-1+j]);
						pushButton_line9Name_addr[i]->setText(ADDR); 
						pushButton_line9Name_addr[i]->show();

						g_pCode[i]= 26+j;
						j++;
					}
				}
				else//???-??????
				{
					for(int i=0;i<g_count;i++)
					{
						ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
						pushButton_line9Name_addr[i]->setText(ADDR); 
						pushButton_line9Name_addr[i]->show();

						g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
					}
				}
			}	
   		}
		else if((g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 11)|| (g_LcdtoDcpContent.code.beginStation > 11 && g_LcdtoDcpContent.code.endStation > 25))
   		{
   			//???-??????
   			int j = 0;
   			if(g_LcdtoDcpContent.code.beginStation > 25 && g_LcdtoDcpContent.code.endStation > 11)
   			{
				g_count = (g_LcdtoDcpContent.code.beginStation-25)+(g_LcdtoDcpContent.code.endStation-11)+1;
   				//g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1-14;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-25;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-25;i<g_count;i++)
				{
					ADDR = QString(addr[11-1+j]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= 11+j;
					j++;
				}
   			}
		//??????-???
			else if((g_LcdtoDcpContent.code.beginStation > 11 && g_LcdtoDcpContent.code.endStation > 25))
			{
				int j = 0;
				g_count = (g_LcdtoDcpContent.code.endStation-25)+(g_LcdtoDcpContent.code.beginStation-11)+1;
				for(int i=0;i<g_LcdtoDcpContent.code.beginStation-10;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}				
				for(int i=g_LcdtoDcpContent.code.beginStation-10;i<g_count;i++)
				{
					ADDR = QString(addr[26-1+j]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= 26+j;
					j++;
				}
			}
		}
		//25·????·??
		else
		{
			g_count = (g_LcdtoDcpContent.code.endStation-1)-(g_LcdtoDcpContent.code.beginStation-1)+1;
			if(g_count < 0)//??????-???
			{
				g_count = (g_LcdtoDcpContent.code.beginStation-1)-(g_LcdtoDcpContent.code.endStation-1)+1;
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)-i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();

					g_pCode[i]= g_LcdtoDcpContent.code.beginStation-i;
				}
			}
			else//???-??????
			{
				for(int i=0;i<g_count;i++)
				{
					ADDR = QString(addr[(g_LcdtoDcpContent.code.beginStation-1)+i]);
					pushButton_line9Name_addr[i]->setText(ADDR); 
					pushButton_line9Name_addr[i]->show();
					
					g_pCode[i]= g_LcdtoDcpContent.code.beginStation+i;
				}
			}	
		}

	printf("******************************************\n");
	for(int i=0;i<g_count;i++)//?????????
	{
		printf("%d ",g_pCode[i]);
	}
	printf("\n");
	for(int i=0;i<g_count;i++)//??????????
	{
		custom_addr[i] = (char*)calloc(sizeof(addr[g_pCode[i]-1]),10);
		strcpy(custom_addr[i],addr[g_pCode[i]-1]);

		printf("custom_addr[%d]=%s\n ",i,custom_addr[i]);
	}		
	for(int i=g_count;i<CUSTOM_MAX_NUM;i++)
	{
		custom_addr[i] = (char*)calloc(sizeof(addr[g_pCode[i]-1]),10);
		printf("custom_addr[%d]=%s\n ",i,custom_addr[i]);
	}
	printf("******************************************\n");
	g_line9_addr_num = 0;
	g_LcdtoDcpContent.code.nextStation = g_pCode[g_line9_addr_num];
	//printf("nextStation=%d\n",g_LcdtoDcpContent.code.nextStation);
	if(g_count < 9 )
	{
		for(int i=g_count;i<25;i++)
		{
			pushButton_line9Name_addr[i]->hide();
		}
	}
}
}


void MainForm::on_pushButton_choose_line_affirm_clicked()
{
	 if(chooseed_line == 0)//???????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x1e ;//???
        g_LcdtoDcpContent.code.endStation = 0x01;//???
    }
    else if(chooseed_line == 1)//???????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x01 ;//???
        g_LcdtoDcpContent.code.endStation = 0x1e;//???
    }
    else if(chooseed_line == 2)//????????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x18 ;//??????
        g_LcdtoDcpContent.code.endStation = 0x0b;//??????
    }
    else if(chooseed_line == 3)//????????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x0b ;//??????
        g_LcdtoDcpContent.code.endStation = 0x18;//??????
    }
    else if(chooseed_line == 4)//????????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x11 ;//???
        g_LcdtoDcpContent.code.endStation = 0x04;//???
    }
    else if(chooseed_line == 5)//??5????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x04 ;//???
        g_LcdtoDcpContent.code.endStation = 0x11;//???
    }
    else if(chooseed_line == 6)//??6????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x11 ;//???
        g_LcdtoDcpContent.code.endStation = 0x0b;//??????
    }
    else if(chooseed_line == 7)//??7????·
    {
        g_LcdtoDcpContent.code.beginStation = 0x0b ;//??????
        g_LcdtoDcpContent.code.endStation = 0x11;//???
    }
	else if(chooseed_line == 8)//自定义
    {
        g_LcdtoDcpContent.code.beginStation = 0x01 ;//
        g_LcdtoDcpContent.code.endStation = 0x1e;//
    }
	else if(chooseed_line == 9)//番禺广场-机场北
    {
        g_LcdtoDcpContent.code.beginStation = 0x01 ;//
        g_LcdtoDcpContent.code.endStation = 0x19;//
    }
	else if(chooseed_line == 10)//机场北-番禺广场
    {
        g_LcdtoDcpContent.code.beginStation = 0x19;//机场北
        g_LcdtoDcpContent.code.endStation = 0x01;//番禺广场
    }
    
    choose_line = QString(addr[g_LcdtoDcpContent.code.beginStation-1]);
	ui->label_begin_station->setText(choose_line);

    choose_line = QString(addr[g_LcdtoDcpContent.code.beginStation-1]);	

    choose_line = QString(addr[g_LcdtoDcpContent.code.endStation-1]);
	ui->label_end_station->setText(choose_line);

	update_choose_next_station();

	//选路线颜色变为正常
	//pushButton_choose_line[chooseed_line]->setStyleSheet(QString("border-image: url(0);")); 
	
	if(chooseed_line == 8)
	{
		ui->label_begin_name->hide();
		ui->label_end_name->hide();
		g_flag_press_begin_station_button = true;
		
		ui->tabWidget->setCurrentIndex(TAB_ADDR_NAME);	  
	}
	else
	{
		g_sendLCDToDcpPackFlag = 1;		
		ui->tabWidget->setCurrentIndex(TAB_BROADCAST);	  
	}

	//ui->pushButton_turn_line->hide();//隐藏下一站选择左上角按钮
	
}

//·???????? ·??1
void MainForm::on_pushButton_choose_route_1_clicked()
{
		pushButton_choose_line[0]->show();
		pushButton_choose_line[1]->show();
		pushButton_choose_line[2]->hide();
		pushButton_choose_line[3]->hide();
		pushButton_choose_line[4]->hide();
		pushButton_choose_line[5]->hide();
		pushButton_choose_line[6]->hide();
		pushButton_choose_line[7]->hide();
		pushButton_choose_line[9]->hide();//番禺-机场北
		pushButton_choose_line[10]->hide();//机场北-番禺
		pushButton_choose_line[8]->show();
}

//·???????? ·??2
void MainForm::on_pushButton_choose_route_2_clicked()
{
		pushButton_choose_line[0]->hide();
		pushButton_choose_line[1]->hide();
		pushButton_choose_line[2]->show();
		pushButton_choose_line[3]->show();
		pushButton_choose_line[4]->hide();
		pushButton_choose_line[5]->hide();
		pushButton_choose_line[6]->hide();
		pushButton_choose_line[7]->hide();
		pushButton_choose_line[9]->hide();//番禺-机场北
		pushButton_choose_line[10]->hide();//机场北-番禺
		pushButton_choose_line[8]->show();//?????
}

//·???????? ·??3
void MainForm::on_pushButton_choose_route_3_clicked()
{
		pushButton_choose_line[0]->hide();
		pushButton_choose_line[1]->hide();
		pushButton_choose_line[2]->hide();
		pushButton_choose_line[3]->hide();
		pushButton_choose_line[4]->show();
		pushButton_choose_line[5]->show();
		pushButton_choose_line[6]->hide();
		pushButton_choose_line[7]->hide();
		pushButton_choose_line[9]->hide();//番禺-机场北
		pushButton_choose_line[10]->hide();//机场北-番禺
		pushButton_choose_line[8]->show();//?????
}

//·???????? ·??4
void MainForm::on_pushButton_choose_route_4_clicked()
{
		pushButton_choose_line[0]->hide();
		pushButton_choose_line[1]->hide();
		pushButton_choose_line[2]->hide();
		pushButton_choose_line[3]->hide();
		pushButton_choose_line[4]->hide();
		pushButton_choose_line[5]->hide();
		pushButton_choose_line[6]->show();
		pushButton_choose_line[7]->show();
		pushButton_choose_line[9]->hide();//番禺-机场北
		pushButton_choose_line[10]->hide();//机场北-番禺
		pushButton_choose_line[8]->show();//?????
}

void MainForm::on_pushButton_choose_route_5_clicked()
{
		pushButton_choose_line[0]->hide();
		pushButton_choose_line[1]->hide();
		pushButton_choose_line[2]->hide();
		pushButton_choose_line[3]->hide();
		pushButton_choose_line[4]->hide();
		pushButton_choose_line[5]->hide();
		pushButton_choose_line[6]->hide();
		pushButton_choose_line[7]->hide();
		pushButton_choose_line[9]->show();//番禺-机场北
		pushButton_choose_line[10]->show();//机场北-番禺
		pushButton_choose_line[8]->show();//?????
}


/************************************·??1 ???-?????******************************************/
//·??1????·?????











/************************************·??2 ???-????******************************************/
//·??2????·?????



//·??2??????


//·??2???????????

//????????


//????????


//?????????

/************************************??????-??????*****************************************/

//????????


//·??3???????????
/*void MainForm::page_turn_funcion_line3()
{
     if(  1 == g_line3_addr_name_choose_flag )
     {      
        ui->pushButton_line3_addr_name_1->show();
        ui->pushButton_line3_addr_name_2->show();
        ui->pushButton_line3_addr_name_3->show();
        ui->pushButton_line3_addr_name_4->show();
        ui->pushButton_line3_addr_name_5->show();
        ui->pushButton_line3_addr_name_6->show();
        ui->pushButton_line3_addr_name_7->show();
        ui->pushButton_line3_addr_name_8->show();
        ui->pushButton_line3_addr_name_9->show();

        ui->pushButton_line2_addr_name_10->hide();
        ui->pushButton_line2_addr_name_11->hide();
        ui->pushButton_line2_addr_name_12->hide();
        ui->pushButton_line2_addr_name_13->hide();
        ui->pushButton_line2_addr_name_14->hide();
        ui->pushButton_line2_addr_name_15->hide();
        ui->pushButton_line2_addr_name_16->hide();

     }
     else if( 2 == g_line2_addr_name_choose_flag )
     {
         ui->pushButton_line2_addr_name_1->hide();
         ui->pushButton_line2_addr_name_2->hide();
         ui->pushButton_line2_addr_name_3->hide();
         ui->pushButton_line2_addr_name_4->hide();
         ui->pushButton_line2_addr_name_5->hide();
         ui->pushButton_line2_addr_name_6->hide();
         ui->pushButton_line2_addr_name_7->hide();
         ui->pushButton_line2_addr_name_8->hide();
         ui->pushButton_line2_addr_name_9->hide();
         
         ui->pushButton_line2_addr_name_10->show();
         ui->pushButton_line2_addr_name_11->show();
         ui->pushButton_line2_addr_name_12->show();
         ui->pushButton_line2_addr_name_13->show();
         ui->pushButton_line2_addr_name_14->show();
         ui->pushButton_line2_addr_name_15->show();
         ui->pushButton_line2_addr_name_16->show();
     }
}


*/

/*******************************日志界面******************************************/
void MainForm::on_pushButton_broadcast_back_9_clicked()
{
        ui->tabWidget->setCurrentIndex(TAB_SYSSET);
}

void MainForm::init_logList()
{
    logContent_group = new QButtonGroup(this);
    logContent_num[0] = ui->pushButton_log_1;
    logContent_num[1] = ui->pushButton_log_2;
    logContent_num[2] = ui->pushButton_log_3;  
    logContent_num[3] = ui->pushButton_log_4; 
    logContent_num[4] = ui->pushButton_log_5; 
    logContent_num[5] = ui->pushButton_log_6;
    logContent_num[6] = ui->pushButton_log_7;
    logContent_num[7] = ui->pushButton_log_8;  
    logContent_num[8] = ui->pushButton_log_9; 
    logContent_num[9] = ui->pushButton_log_10; 
    logContent_num[10] = ui->pushButton_log_11;
    logContent_num[11] = ui->pushButton_log_12;
    logContent_num[12] = ui->pushButton_log_13;  
    logContent_num[13] = ui->pushButton_log_14; 
    logContent_num[14] = ui->pushButton_log_15; 

	logContent_num[15] = ui->pushButton_log_16;
    logContent_num[16] = ui->pushButton_log_17;
    logContent_num[17] = ui->pushButton_log_18;  
    logContent_num[18] = ui->pushButton_log_19; 
    logContent_num[19] = ui->pushButton_log_20; 
    logContent_num[20] = ui->pushButton_log_21;
    logContent_num[21] = ui->pushButton_log_22;
    logContent_num[22] = ui->pushButton_log_23;  
    logContent_num[23] = ui->pushButton_log_24; 
    logContent_num[24] = ui->pushButton_log_25; 
    logContent_num[25] = ui->pushButton_log_26;
    logContent_num[26] = ui->pushButton_log_27;
    logContent_num[27] = ui->pushButton_log_28;  
    logContent_num[28] = ui->pushButton_log_29; 
    logContent_num[29] = ui->pushButton_log_30; 

	logContent_num[30] = ui->pushButton_log_31;
    logContent_num[31] = ui->pushButton_log_32;
    logContent_num[32] = ui->pushButton_log_33;  
    logContent_num[33] = ui->pushButton_log_34; 
    logContent_num[34] = ui->pushButton_log_35; 
    logContent_num[35] = ui->pushButton_log_36;
    logContent_num[36] = ui->pushButton_log_37;
    logContent_num[37] = ui->pushButton_log_38;  
    logContent_num[38] = ui->pushButton_log_39; 
    logContent_num[39] = ui->pushButton_log_40; 
    logContent_num[40] = ui->pushButton_log_41;
    logContent_num[41] = ui->pushButton_log_42;
    logContent_num[42] = ui->pushButton_log_43;  
    logContent_num[43] = ui->pushButton_log_44; 
    logContent_num[44] = ui->pushButton_log_45; 

	logContent_num[45] = ui->pushButton_log_46;
    logContent_num[46] = ui->pushButton_log_47;
    logContent_num[47] = ui->pushButton_log_48;  
    logContent_num[48] = ui->pushButton_log_49; 
    logContent_num[49] = ui->pushButton_log_50; 
    logContent_num[50] = ui->pushButton_log_51;
    logContent_num[51] = ui->pushButton_log_52;
    logContent_num[52] = ui->pushButton_log_53;  
    logContent_num[53] = ui->pushButton_log_54; 
    logContent_num[54] = ui->pushButton_log_55; 
    logContent_num[55] = ui->pushButton_log_56;
    logContent_num[56] = ui->pushButton_log_57;
    logContent_num[57] = ui->pushButton_log_58;  
    logContent_num[58] = ui->pushButton_log_59; 
    logContent_num[59] = ui->pushButton_log_60; 

    logContent_num[60] = ui->pushButton_log_61;
    logContent_num[61] = ui->pushButton_log_62;
    logContent_num[62] = ui->pushButton_log_63;  
    logContent_num[63] = ui->pushButton_log_64; 
    logContent_num[64] = ui->pushButton_log_65; 
    logContent_num[65] = ui->pushButton_log_66;
    logContent_num[66] = ui->pushButton_log_67;
    logContent_num[67] = ui->pushButton_log_68;  
    logContent_num[68] = ui->pushButton_log_69; 
    logContent_num[69] = ui->pushButton_log_70; 
    logContent_num[70] = ui->pushButton_log_71;
    logContent_num[71] = ui->pushButton_log_72;
    logContent_num[72] = ui->pushButton_log_73;  
    logContent_num[73] = ui->pushButton_log_74; 
    logContent_num[74] = ui->pushButton_log_75; 

	logContent_num[75] = ui->pushButton_log_76;
    logContent_num[76] = ui->pushButton_log_77;
    logContent_num[77] = ui->pushButton_log_78;  
    logContent_num[78] = ui->pushButton_log_79; 
    logContent_num[79] = ui->pushButton_log_80; 
    logContent_num[80] = ui->pushButton_log_81;
    logContent_num[81] = ui->pushButton_log_82;
    logContent_num[82] = ui->pushButton_log_83;  
    logContent_num[83] = ui->pushButton_log_84; 
    logContent_num[84] = ui->pushButton_log_85; 
    logContent_num[85] = ui->pushButton_log_86;
    logContent_num[86] = ui->pushButton_log_87;
    logContent_num[87] = ui->pushButton_log_88;  
    logContent_num[88] = ui->pushButton_log_89; 
    logContent_num[89] = ui->pushButton_log_90; 

	logContent_num[90] = ui->pushButton_log_91;
    logContent_num[91] = ui->pushButton_log_92;
    logContent_num[92] = ui->pushButton_log_93;  
    logContent_num[93] = ui->pushButton_log_94; 
    logContent_num[94] = ui->pushButton_log_95; 
    logContent_num[95] = ui->pushButton_log_96;
    logContent_num[96] = ui->pushButton_log_97;
    logContent_num[97] = ui->pushButton_log_98;  
    logContent_num[98] = ui->pushButton_log_99; 
	logContent_num[99] = ui->pushButton_log_100; 
	logContent_num[100] = ui->pushButton_log_101;
	logContent_num[101] = ui->pushButton_log_102;
	logContent_num[102] = ui->pushButton_log_103;  
	logContent_num[103] = ui->pushButton_log_104; 	
	logContent_num[104] = ui->pushButton_log_105; 

 

	for (int i=0;i<LOG_COUNT;i++)
	{
		logContent_group->addButton(logContent_num[i],i);
	}
}

void MainForm::page_turn_funcion_log()
{
     if(  1 == g_log_choose_flag )
     {     
     	for(int i=0;i<15;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=15;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
     else if( 2 == g_log_choose_flag )
     {
     	for(int i=0;i<15;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=15;i<30;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=30;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
	 else if( 3 == g_log_choose_flag )
     {
        for(int i=0;i<30;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=30;i<45;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=45;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
	 else if( 4 == g_log_choose_flag )
     {
        for(int i=0;i<45;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=45;i<60;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=60;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
	 else if( 5 == g_log_choose_flag )
     {
        for(int i=0;i<60;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=60;i<75;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=75;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
	 else if( 6 == g_log_choose_flag )
     {
        for(int i=0;i<75;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=75;i<90;i++)
     	{
     		logContent_num[i]->show();
     	}
		for(int i=90;i<105;i++)
		{
			logContent_num[i]->hide();
		}
     }
	 else if( 7 == g_log_choose_flag )
     {
        for(int i=0;i<90;i++)
		{
			logContent_num[i]->hide();
		}
     	for(int i=90;i<105;i++)
     	{
     		logContent_num[i]->show();
     	}
     }
}
void MainForm::on_pushButton_textedit_page_down_clicked()
{
	int count = ui->textEdit->verticalScrollBar()->value();

ui->textEdit->verticalScrollBar()->setValue(count+100);
//ui.textEdit.verticalScrollBar().setValue(ui.textEdit.verticalScrollBar().maximumHeight());

	
}

void MainForm::on_pushButton_textedit_page_up_clicked()
{
	int count = ui->textEdit->verticalScrollBar()->value();

ui->textEdit->verticalScrollBar()->setValue(count-100);
//ui.textEdit.verticalScrollBar().setValue(ui.textEdit.verticalScrollBar().maximumHeight());

	
}


int MainForm::TurnPageLog()
{
	int turnpage = 7;
	if( g_turnLogPage < 15 )
	{
		turnpage = 1;
	}
	else if( g_turnLogPage > 15 && g_turnLogPage <= 30 )
	{
		turnpage = 2;
	}
	else if( g_turnLogPage > 30 && g_turnLogPage <= 45 )
	{
		turnpage = 3;
	}
	else if( g_turnLogPage > 45 && g_turnLogPage <= 60 )
	{
		turnpage = 4;
	}
	else if( g_turnLogPage > 60 && g_turnLogPage <= 75 )
	{
		turnpage = 5;
	}
	else if( g_turnLogPage > 75 && g_turnLogPage <= 90 )
	{
		turnpage = 6;
	}
	else if( g_turnLogPage > 90 && g_turnLogPage <= 105 )
	{
		turnpage = 7;
	}
	logPageNum = QString(turnpageNameLog[turnpage-1]);
	ui->label_page_num_end_2->setText(logPageNum); 
	return turnpage;
}


//????????
void MainForm::on_pushButton_log_up_clicked()
{
	int page = TurnPageLog();
    g_log_choose_flag--;
    if(  0 == g_log_choose_flag )
    {
        g_log_choose_flag = page;
    }    
	logPageNum = QString(turnpageNameLog[g_log_choose_flag-1]);
	ui->label_page_num_now_2->setText(logPageNum); 
    page_turn_funcion_log(); 
}

//????????
void MainForm::on_pushButton_log_down_clicked()
{
	int page = TurnPageLog();
    g_log_choose_flag++;
    if(  page+1 == g_log_choose_flag )
    {
        g_log_choose_flag = 1;
    }
	logPageNum = QString(turnpageNameLog[g_log_choose_flag-1]);
	ui->label_page_num_now_2->setText(logPageNum); 
    page_turn_funcion_log(); 
}

/*********************************故障信息界面****************************************/
//?????????
void MainForm::on_pushButton_error_message_back_clicked()
{
		init_Error_message_check_rep();//修复开机好的时间
        ui->tabWidget->setCurrentIndex(TAB_SYSSET);
}

void MainForm::init_errorList()
{
    errorContent_group = new QButtonGroup(this);
    errorContent_num[0] = ui->pushButton_error_1;
    errorContent_num[1] = ui->pushButton_error_2;
    errorContent_num[2] = ui->pushButton_error_3;  
    errorContent_num[3] = ui->pushButton_error_4; 
    errorContent_num[4] = ui->pushButton_error_5; 
    errorContent_num[5] = ui->pushButton_error_6;
    errorContent_num[6] = ui->pushButton_error_7;
    errorContent_num[7] = ui->pushButton_error_8;  
    errorContent_num[8] = ui->pushButton_error_9; 
    errorContent_num[9] = ui->pushButton_error_10; 
    errorContent_num[10] = ui->pushButton_error_11;
    errorContent_num[11] = ui->pushButton_error_12;
    errorContent_num[12] = ui->pushButton_error_13;  
    errorContent_num[13] = ui->pushButton_error_14; 
    errorContent_num[14] = ui->pushButton_error_15; 

	errorContent_num[15] = ui->pushButton_error_16;
    errorContent_num[16] = ui->pushButton_error_17;
    errorContent_num[17] = ui->pushButton_error_18;  
    errorContent_num[18] = ui->pushButton_error_19; 
    errorContent_num[19] = ui->pushButton_error_20; 
    errorContent_num[20] = ui->pushButton_error_21;
    errorContent_num[21] = ui->pushButton_error_22;
    errorContent_num[22] = ui->pushButton_error_23;  
    errorContent_num[23] = ui->pushButton_error_24; 
    errorContent_num[24] = ui->pushButton_error_25; 
    errorContent_num[25] = ui->pushButton_error_26;
    errorContent_num[26] = ui->pushButton_error_27;
    errorContent_num[27] = ui->pushButton_error_28;  
    errorContent_num[28] = ui->pushButton_error_29; 
    errorContent_num[29] = ui->pushButton_error_30; 

	errorContent_num[30] = ui->pushButton_error_31;
    errorContent_num[31] = ui->pushButton_error_32;
    errorContent_num[32] = ui->pushButton_error_33;  
    errorContent_num[33] = ui->pushButton_error_34; 
    errorContent_num[34] = ui->pushButton_error_35; 
    errorContent_num[35] = ui->pushButton_error_36;
    errorContent_num[36] = ui->pushButton_error_37;
    errorContent_num[37] = ui->pushButton_error_38;  
    errorContent_num[38] = ui->pushButton_error_39; 
    errorContent_num[39] = ui->pushButton_error_40; 
    errorContent_num[40] = ui->pushButton_error_41;
    errorContent_num[41] = ui->pushButton_error_42;
    errorContent_num[42] = ui->pushButton_error_43;  
    errorContent_num[43] = ui->pushButton_error_44; 
    errorContent_num[44] = ui->pushButton_error_45; 

	errorContent_num[45] = ui->pushButton_error_46;
    errorContent_num[46] = ui->pushButton_error_47;
    errorContent_num[47] = ui->pushButton_error_48;  
    errorContent_num[48] = ui->pushButton_error_49; 
    errorContent_num[49] = ui->pushButton_error_50; 
    errorContent_num[50] = ui->pushButton_error_51;
    errorContent_num[51] = ui->pushButton_error_52;
    errorContent_num[52] = ui->pushButton_error_53;  
    errorContent_num[53] = ui->pushButton_error_54; 
    errorContent_num[54] = ui->pushButton_error_55; 
    errorContent_num[55] = ui->pushButton_error_56;
    errorContent_num[56] = ui->pushButton_error_57;
    errorContent_num[57] = ui->pushButton_error_58;  
    errorContent_num[58] = ui->pushButton_error_59; 
    errorContent_num[59] = ui->pushButton_error_60; 

    errorContent_num[60] = ui->pushButton_error_61;
    errorContent_num[61] = ui->pushButton_error_62;
    errorContent_num[62] = ui->pushButton_error_63;  
    errorContent_num[63] = ui->pushButton_error_64; 
    errorContent_num[64] = ui->pushButton_error_65; 
    errorContent_num[65] = ui->pushButton_error_66;
    errorContent_num[66] = ui->pushButton_error_67;
    errorContent_num[67] = ui->pushButton_error_68;  
    errorContent_num[68] = ui->pushButton_error_69; 
    errorContent_num[69] = ui->pushButton_error_70; 
    errorContent_num[70] = ui->pushButton_error_71;
    errorContent_num[71] = ui->pushButton_error_72;
    errorContent_num[72] = ui->pushButton_error_73;  
    errorContent_num[73] = ui->pushButton_error_74; 
    errorContent_num[74] = ui->pushButton_error_75; 

	errorContent_num[75] = ui->pushButton_error_76;
    errorContent_num[76] = ui->pushButton_error_77;
    errorContent_num[77] = ui->pushButton_error_78;  
    errorContent_num[78] = ui->pushButton_error_79; 
    errorContent_num[79] = ui->pushButton_error_80; 
    errorContent_num[80] = ui->pushButton_error_81;
    errorContent_num[81] = ui->pushButton_error_82;
    errorContent_num[82] = ui->pushButton_error_83;  
    errorContent_num[83] = ui->pushButton_error_84; 
    errorContent_num[84] = ui->pushButton_error_85; 
    errorContent_num[85] = ui->pushButton_error_86;
    errorContent_num[86] = ui->pushButton_error_87;
    errorContent_num[87] = ui->pushButton_error_88;  
    errorContent_num[88] = ui->pushButton_error_89; 
    errorContent_num[89] = ui->pushButton_error_90; 

	errorContent_num[90] = ui->pushButton_error_91;
    errorContent_num[91] = ui->pushButton_error_92;
    errorContent_num[92] = ui->pushButton_error_93;  
    errorContent_num[93] = ui->pushButton_error_94; 
    errorContent_num[94] = ui->pushButton_error_95; 
    errorContent_num[95] = ui->pushButton_error_96;
    errorContent_num[96] = ui->pushButton_error_97;
    errorContent_num[97] = ui->pushButton_error_98;  
    errorContent_num[98] = ui->pushButton_error_99; 
	errorContent_num[99] = ui->pushButton_error_100; 
	errorContent_num[100] = ui->pushButton_error_101;
	errorContent_num[101] = ui->pushButton_error_102;
	errorContent_num[102] = ui->pushButton_error_103;  
	errorContent_num[103] = ui->pushButton_error_104; 	
	errorContent_num[104] = ui->pushButton_error_105; 
 

	for (int i=0;i<ERRORLIST_COUNT;i++)
	{
		errorContent_group->addButton(errorContent_num[i],i);
	}
}

void MainForm::page_turn_funcion_error()
{
     if(  1 == g_error_choose_flag )
     {     
     	for(int i=0;i<15;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=15;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
     else if( 2 == g_error_choose_flag )
     {
     	for(int i=0;i<15;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=15;i<30;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=30;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
	 else if( 3 == g_error_choose_flag )
     {
        for(int i=0;i<30;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=30;i<45;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=45;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
	 else if( 4 == g_error_choose_flag )
     {
        for(int i=0;i<45;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=45;i<60;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=60;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
	 else if( 5 == g_error_choose_flag )
     {
        for(int i=0;i<60;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=60;i<75;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=75;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
	 else if( 6 == g_error_choose_flag )
     {
        for(int i=0;i<75;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=75;i<90;i++)
     	{
     		errorContent_num[i]->show();
     	}
		for(int i=90;i<105;i++)
		{
			errorContent_num[i]->hide();
		}
     }
	 else if( 7 == g_error_choose_flag )
     {
        for(int i=0;i<90;i++)
		{
			errorContent_num[i]->hide();
		}
     	for(int i=90;i<105;i++)
     	{
     		errorContent_num[i]->show();
     	}
     }
}

int MainForm::TurnPage()
{
	int turnpage = 7;
	if( g_turnErrorPage < 15 ) {
		turnpage = 1;
	}else if( g_turnErrorPage > 15 && g_turnErrorPage <= 30 ) {
		turnpage = 2;
	}else if( g_turnErrorPage > 30 && g_turnErrorPage <= 45 ) {
		turnpage = 3;
	}else if( g_turnErrorPage > 45 && g_turnErrorPage <= 60 ) {
		turnpage = 4;
	}else if( g_turnErrorPage > 60 && g_turnErrorPage <= 75 ) {
		turnpage = 5;
	}else if( g_turnErrorPage > 75 && g_turnErrorPage <= 90 ) {
		turnpage = 6;
	}else if( g_turnErrorPage > 90 && g_turnErrorPage <= 105 ) {
		turnpage = 7;
	}
	errorPageNum = QString(turnpageName[turnpage-1]);
	ui->label_page_num_end->setText(errorPageNum); 
	return turnpage;
}

void MainForm::on_pushButton_error_up_clicked()
{
	int page = TurnPage();
    g_error_choose_flag--;
    if(  0 == g_error_choose_flag )
    {
        g_error_choose_flag = page;
    }    
	errorPageNum = QString(turnpageName[g_error_choose_flag-1]);
	ui->label_page_num_now->setText(errorPageNum); 
    page_turn_funcion_error(); 
}


void MainForm::on_pushButton_error_down_clicked()
{
	int page = TurnPage();
    g_error_choose_flag++;
    if(  page+1 == g_error_choose_flag )
    {
        g_error_choose_flag = 1;
    }
	errorPageNum = QString(turnpageName[g_error_choose_flag-1]);
	ui->label_page_num_now->setText(errorPageNum); 
    page_turn_funcion_error(); 
}
void MainForm::on_pushButton_error_time_roll_clicked()
{
	static unsigned char errRoll = 0;
	if(errRoll == 0)
	{
		errRoll = 1;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		//char temptimes[20];
		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				//if(memcmp(errorLog[errNumIN]->ErrTime,errorLog[errNumIN+1]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))>0)
				if(errorLog[errNum]->Times < errorLog[errNumIN]->Times)
				{
					int temptimes = errorLog[errNum]->Times;
					errorLog[errNum]->Times = errorLog[errNumIN]->Times;
					errorLog[errNumIN]->Times = temptimes;
					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	else if(errRoll == 1)
	{
		errRoll = 0;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		//char temptimes[20];

		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				//if(memcmp(errorLog[errNumIN]->ErrTime,errorLog[errNumIN+1]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))>0)
				if(errorLog[errNum]->Times > errorLog[errNumIN]->Times)
				{
					int temptimes = errorLog[errNum]->Times;
					errorLog[errNum]->Times = errorLog[errNumIN]->Times;
					errorLog[errNumIN]->Times = temptimes;
					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	
}

void MainForm::on_pushButton_error_start_time_roll_clicked()
{
	static unsigned char errRoll = 0;
	if(errRoll == 0)
	{
		errRoll = 1;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		char temptimes[20];

		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				if(memcmp(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))>0)
				//if(errorLog[errNum]->Times < errorLog[errNumIN]->Times)
				{
					memcpy(temptimes,errorLog[errNum]->ErrTime,sizeof(errorLog[errNum]->ErrTime));
					memcpy(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime));
					memcpy(errorLog[errNumIN]->ErrTime,temptimes,sizeof(temptimes));
					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	else if(errRoll == 1)
	{
		errRoll = 0;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		char temptimes[20];
		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				if(memcmp(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))<0)
				//if(memcmp(errorLog[errNumIN]->ErrTime,errorLog[errNumIN+1]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))>0)
				//if(errorLog[errNum]->ErrTime > errorLog[errNumIN]->ErrTime)
				{
					memcpy(temptimes,errorLog[errNum]->ErrTime,sizeof(errorLog[errNum]->ErrTime));
					memcpy(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime));
					memcpy(errorLog[errNumIN]->ErrTime,temptimes,sizeof(temptimes));
					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	
}

void MainForm::on_pushButton_error_end_time_roll_clicked()
{
	static unsigned char errRoll = 0;
	if(errRoll == 0)
	{
		errRoll = 1;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		char temptimes[20];
		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				if(memcmp(errorLog[errNum]->RepTime,errorLog[errNumIN]->RepTime,sizeof(errorLog[errNumIN]->RepTime))>0)
				//if(errorLog[errNum]->Times < errorLog[errNumIN]->Times)
				{
					memcpy(temptimes,errorLog[errNum]->RepTime,sizeof(errorLog[errNum]->RepTime));
					memcpy(errorLog[errNum]->RepTime,errorLog[errNumIN]->RepTime,sizeof(errorLog[errNumIN]->RepTime));
					memcpy(errorLog[errNumIN]->RepTime,temptimes,sizeof(temptimes));

					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	else if(errRoll == 1)
	{
		errRoll = 0;
		char bufError[MAX_FILE_SIZE] = {0};
		char *resultError[4096] = {0};
		t_errorLog *errorLog[30];
		for(int i=0;i<30;i++)
		{
			errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
			memset(errorLog[i],0,sizeof(t_errorLog));
		}
		
		int count = 0;
		if(readError(bufError,resultError,&count,errorLog)!=0)
		{
			printf("readLog error\n");
		}	
		//顺序排序
		char temp[100] = {0};
		char temptimes[20];
		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				if(memcmp(errorLog[errNum]->RepTime,errorLog[errNumIN]->RepTime,sizeof(errorLog[errNumIN]->RepTime))<0)
				//if(memcmp(errorLog[errNumIN]->ErrTime,errorLog[errNumIN+1]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))>0)
				//if(errorLog[errNum]->ErrTime > errorLog[errNumIN]->ErrTime)
				{
					memcpy(temptimes,errorLog[errNum]->RepTime,sizeof(errorLog[errNum]->RepTime));
					memcpy(errorLog[errNum]->RepTime,errorLog[errNumIN]->RepTime,sizeof(errorLog[errNumIN]->RepTime));
					memcpy(errorLog[errNumIN]->RepTime,temptimes,sizeof(temptimes));

					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
	}
	
}



/************************************自定义路线界面***********************************************************/
void MainForm::init_choose_line9_addr_buttons()
{
    pushButton_line9Name_addr_group = new QButtonGroup(this);
    pushButton_line9Name_addr[0] = ui->pushButton_custom_station_1;
    pushButton_line9Name_addr[1] = ui->pushButton_custom_station_2;
    pushButton_line9Name_addr[2] = ui->pushButton_custom_station_3;  
    pushButton_line9Name_addr[3] = ui->pushButton_custom_station_4; 
    pushButton_line9Name_addr[4] = ui->pushButton_custom_station_5; 
    pushButton_line9Name_addr[5] = ui->pushButton_custom_station_6;
    pushButton_line9Name_addr[6] = ui->pushButton_custom_station_7;
    pushButton_line9Name_addr[7] = ui->pushButton_custom_station_8;  
    pushButton_line9Name_addr[8] = ui->pushButton_custom_station_9; 
    pushButton_line9Name_addr[9] = ui->pushButton_custom_station_10; 
    pushButton_line9Name_addr[10] = ui->pushButton_custom_station_11;
    pushButton_line9Name_addr[11] = ui->pushButton_custom_station_12;
    pushButton_line9Name_addr[12] = ui->pushButton_custom_station_13;  
    pushButton_line9Name_addr[13] = ui->pushButton_custom_station_14; 
    pushButton_line9Name_addr[14] = ui->pushButton_custom_station_15; 
    pushButton_line9Name_addr[15] = ui->pushButton_custom_station_16;
    pushButton_line9Name_addr[16] = ui->pushButton_custom_station_17;
    pushButton_line9Name_addr[17] = ui->pushButton_custom_station_18;
    pushButton_line9Name_addr[18] = ui->pushButton_custom_station_19;  
    pushButton_line9Name_addr[19] = ui->pushButton_custom_station_20; 
    pushButton_line9Name_addr[20] = ui->pushButton_custom_station_21; 
    pushButton_line9Name_addr[21] = ui->pushButton_custom_station_22;
    pushButton_line9Name_addr[22] = ui->pushButton_custom_station_23;
    pushButton_line9Name_addr[23] = ui->pushButton_custom_station_24;  
    pushButton_line9Name_addr[24] = ui->pushButton_custom_station_25; 
    
	for (int i=0;i<LINE9_COUNT;i++)
	{
		pushButton_line9Name_addr_group->addButton(pushButton_line9Name_addr[i],i);
	}
	connect(pushButton_line9Name_addr_group,SIGNAL(buttonClicked(int)),this,SLOT(on_choose_line9_addr_button_group_clicked(int)));
}

//?????·?????
void MainForm::on_choose_line9_addr_button_group_clicked(int line9_addr_id)
{
    int i;
    for( i = 0; i < LINE9_COUNT; i++)
    {
        if( line9_addr_id == i )
        {            
            pushButton_line9Name_addr[line9_addr_id]->setStyleSheet(QString("border-image: url(:/yellow_square.png);"));        
        }
        else
        {
            pushButton_line9Name_addr[i]->setStyleSheet(QString("border-image: url(0);"));
        }
        g_line9_addr_num = line9_addr_id;
       
    }
}

//?????·???????
void MainForm::on_pushButton_custom_route_back_clicked()
{
	//清显示的黄色
	pushButton_line9Name_addr[g_line9_addr_num]->setStyleSheet(QString("border-image: url(0);"));
    ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
}

void MainForm::page_turn_funcion9()
{
	 if(  1 == g_line9_addr_name_choose_flag )
     {     
     	if(g_count<=9)
     	{
			for(int i=0;i<g_count;i++)
			{
				pushButton_line9Name_addr[i]->show();
			}
			
			for(int i=g_count;i<25;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
     	}
		else
		{
			for(int i=0;i<9;i++)
			{
				pushButton_line9Name_addr[i]->show();
			}
			for(int i=9;i<25;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
		}
     }
     else if( 2 == g_line9_addr_name_choose_flag )
     {
     	for(int i=0;i<9;i++)
		{
			pushButton_line9Name_addr[i]->hide();
		}
		if(g_count <= 18)
		{
			for(int i=9;i<g_count;i++)
	     	{
	     		pushButton_line9Name_addr[i]->show();
	     	}
			for(int i=g_count;i<25;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
		}
		else
		{
			for(int i=9;i<18;i++)
	     	{
	     		pushButton_line9Name_addr[i]->show();
	     	}
			for(int i=18;i<25;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
		}     	
     }
	 else if( 3 == g_line9_addr_name_choose_flag && g_count > 18)
     {
     	if(g_count <= 25)
     	{
	     	for(int i=0;i<18;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
	     	for(int i=18;i<g_count;i++)
	     	{
	     		pushButton_line9Name_addr[i]->show();
	     	}	
     	}
		else
		{
			for(int i=0;i<18;i++)
			{
				pushButton_line9Name_addr[i]->hide();
			}
	     	for(int i=18;i<25;i++)
	     	{
	     		pushButton_line9Name_addr[i]->show();
	     	}
		}    	
     }
}  

void MainForm::on_pushButton_line9_addr_name_down_clicked()
{
	if(g_count > 9)//?????????????9???????????
	{
		g_line9_addr_name_choose_flag++;
	    if( 4 == g_line9_addr_name_choose_flag || (3 == g_line9_addr_name_choose_flag&&g_count<=18))
	    {
	        g_line9_addr_name_choose_flag = 1;
	    }
	}
     page_turn_funcion9(); 
}

//????????
void MainForm::on_pushButton_line9_addr_name_up_clicked()
{
	if(g_count > 9)
	{
		g_line9_addr_name_choose_flag--;
	    if(  0 == g_line9_addr_name_choose_flag )
	    {
	    	if(g_count<=18)
	    	{
	    		g_line9_addr_name_choose_flag = 2;
	    	}
			else
			{	
				g_line9_addr_name_choose_flag = 3;
			}
	    }    
	}
    page_turn_funcion9(); 
}

//?????????
void MainForm::on_pushButton_custom_route_confirm_clicked()
{
    line9Name = QString(custom_addr[g_line9_addr_num]);
    //ui->lineEdit_arrive_next->setText(line9Name);//???????
    //ui->pushButton_arrive_next_station->setText(line9Name);//???????

    g_LcdtoDcpContent.code.nextStation= g_pCode[g_line9_addr_num];//????????
	printf("g_LcdtoDcpContent.code.nextStation = %d\n",g_LcdtoDcpContent.code.nextStation);
	
	g_sendLCDToDcpPackFlag = 1;

	//清显示的黄色
	pushButton_line9Name_addr[g_line9_addr_num]->setStyleSheet(QString("border-image: url(0);"));
    ui->tabWidget->setCurrentIndex(TAB_BROADCAST);        
}
void MainForm::on_pushButton_turn_line_clicked()
{
	//static unsigned char changLine_Flag = 1;
	//if(changLine_Flag)
	//{

	//}
	//else
	//{
		chooseed_line = 1;
        g_LcdtoDcpContent.code.beginStation = 0x01 ;//???
        g_LcdtoDcpContent.code.endStation = 0x1e;//???
		//changLine_Flag = 1;

	//}
	choose_line = QString(addr[g_LcdtoDcpContent.code.beginStation-1]);
	ui->label_begin_station->setText(choose_line);
	choose_line = QString(addr[g_LcdtoDcpContent.code.endStation-1]);
	ui->label_end_station->setText(choose_line);
	update_choose_next_station();

}
void MainForm::on_pushButton_turn_line_2_clicked()
{
	//static unsigned char changLine_Flag = 1;
	//if(changLine_Flag)
	//{
		chooseed_line = 9;
		g_LcdtoDcpContent.code.beginStation = 0x01 ;//
		g_LcdtoDcpContent.code.endStation = 0x19;//  
		//changLine_Flag = 0;
	//}
	//else
	//

	//}
	choose_line = QString(addr[g_LcdtoDcpContent.code.beginStation-1]);
	ui->label_begin_station->setText(choose_line);
	choose_line = QString(addr[g_LcdtoDcpContent.code.endStation-1]);
	ui->label_end_station->setText(choose_line);
	update_choose_next_station();

}


/*****************************???y???**********************************************/
void MainForm::on_pushButton_systemset_back_clicked()
{
	ui->tabWidget->setCurrentIndex(TAB_BROADCAST);
}

int MainForm::on_pushButton_error_message_clicked()
{
	char bufError[MAX_FILE_SIZE] = {0};
	char *resultError[4096] = {0};
	t_errorLog *errorLog[30];
	int count = 0;
	int ret = mkdir("/root/usr_exe/ERROR/",0777);
	if(ret < 0)printf("mkdired\n");
	for(int i=0;i<30;i++)
	{
		errorLog[i]=(t_errorLog*)malloc(sizeof(t_errorLog));
		memset(errorLog[i],0,sizeof(t_errorLog));
	}
	
	if(readError(bufError,resultError,&count,errorLog)!=0)
	{
		for(int k=0;k<30;k++)
		{
			free(errorLog[k]);
		}
		return 0;
	}	
	else
	{
		//顺序排序
		char temp[100] = {0};
		char temptimes[20];
	
		for(int errNum=0;errNum<22;errNum++)
		{
			for(int errNumIN=errNum+1;errNumIN<22;errNumIN++)
			{	
				if(memcmp(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime))<0)
				//if(errorLog[errNum]->Times < errorLog[errNumIN]->Times)
				{
					memcpy(temptimes,errorLog[errNum]->ErrTime,sizeof(errorLog[errNum]->ErrTime));
					memcpy(errorLog[errNum]->ErrTime,errorLog[errNumIN]->ErrTime,sizeof(errorLog[errNumIN]->ErrTime));
					memcpy(errorLog[errNumIN]->ErrTime,temptimes,sizeof(temptimes));
					
					memcpy(temp,errorLog[errNum]->Buffer,sizeof(errorLog[errNum]->Buffer));
					memcpy(errorLog[errNum]->Buffer,errorLog[errNumIN]->Buffer,sizeof(errorLog[errNumIN]->Buffer));
					memcpy(errorLog[errNumIN]->Buffer,temp,sizeof(temp));
				}
			}
		}	
	
		//打印组合好的错误
		for(int f=0;f<30;f++)
		{
			errorContent = QString(errorLog[f]->Buffer);
			errorContent_num[f]->setText(errorContent);
		}
		g_turnErrorPage = 23;
		TurnPage();
	}
	
	for(int k=0;k<30;k++)
	{
		free(errorLog[k]);
	}
	ui->tabWidget->setCurrentIndex(TAB_ERROR);
}

/*******************音量设置*******************************************/
void MainForm::on_pushButton_error_message_back_volume_clicked()
{
        ui->tabWidget->setCurrentIndex(TAB_SYSSET);
}

void MainForm::on_pushButton_volume_up_clicked()
{
      gVolCur = g_nvrtToVvsContent.city.moniBrodcast;//获取当前的音量值
      
      if(gVolCur >= 15)
      {
		gVolCur = 0;
      }
	  else
	  {
		gVolCur++;
	  }
	  g_LcdtoDcpContent.lineMode.moniBroadcast = gVolCur;
	  printf("gVolCur=%d\n",g_LcdtoDcpContent.lineMode.moniBroadcast);
      g_sendLCDToDcpPackFlag = 1;
      
}

void MainForm::on_pushButton_volume_down_clicked()
{
      gVolCur = g_nvrtToVvsContent.city.moniBrodcast;//获取当前的音量值
      if(gVolCur <= 0)
      {
		gVolCur = 15;
      }
	  else
	  {
		gVolCur--;
	  }
	  g_LcdtoDcpContent.lineMode.moniBroadcast = gVolCur;
	  printf("gVolCur=%d\n",g_LcdtoDcpContent.lineMode.moniBroadcast);
      g_sendLCDToDcpPackFlag = 1;	
}
void MainForm::on_pushButton_set_moin_clicked()
{
	static int i = 0;
	 if( i == 0)
	 {
		 //ui->pushButton_mute_or_monitor->setStyleSheet(QString("border-image: url(:/设置界面_0000_监听静音.png);"));
		 ui->volume_2->hide();
		 ui->volume_3->show();
		 g_LcdtoDcpContent.lineMode.moniBroadcast= 1;//???????		
		 
		 i = 1; 	 
	 }
	 else
	 {
		 //ui->pushButton_mute_or_monitor->setStyleSheet(QString("border-image: url(:/修改/监听按钮.png);"));
		 ui->volume_2->show();
		 ui->volume_3->hide();
		 g_LcdtoDcpContent.lineMode.moniBroadcast= 0;//????????	   
		 //g_sendLCDToDcpPackFlag = 1;
		i = 0;//??????				   
	 }	
	 g_sendLCDToDcpPackFlag = 1;

}



/*************************************版本信息****************************************************/
void MainForm::on_pushButton_system_message_back_clicked()
{
	gPiscLevl = 0;
    ui->tabWidget->setCurrentIndex(TAB_SYSSET);
}

void MainForm::init_system_datetime_edit()
{
	QDateTime time_now = QDateTime::currentDateTime();
	ui->dateTimeEdit->setDateTime( time_now );
}
void MainForm::on_pushButton_set_datetime_clicked()
{
	char *dt;
	QByteArray ba;
	QString  str;	
	QDateTime time_get = ui->dateTimeEdit->dateTime();
	str = time_get.toString("yyyy-MM-dd HH:mm:ss");
	ba = str.toLatin1();  	
	dt = ba.data();

	SetSystemTime( dt );
	SetDs1307RtcTimeHuman( dt );
}

void MainForm::on_pushButton_into_update_file_clicked()
{
	int ret = 0;
	//QMessageBox::information(this,"Title here","Hello World");
	int index = ui->comboBox_information->currentIndex();
	ui->pushButton_into_update_file->setText(QString("升级"));
	if(index == 0){
		ui->pushButton_into_update_file->setText(QString("正在升级......."));
		ret = updata_from_tftp_file("192.168.1.60", (char*)UPDATE_FILE_NAME,(char*)DOWNLOAD_FILE_PATH);
	}else if(index == 1){
		ui->pushButton_into_update_file->setText(QString("正在升级......."));
		ret = updata_from_tftp_file("192.168.1.60", (char*)UPDATE_FILE_NAME_PRO,(char*)DOWNLOAD_FILE_PRO);
	}
	if (ret == -1){
		if(!index){
			ui->pushButton_into_update_file->setText(QString("配置更新失败!"));
		}else{
			ui->pushButton_into_update_file->setText(QString("程序更新失败!"));
		}
	}else{
		sleep(5);
		if(!index){
			ui->pushButton_into_update_file->setText(QString("配置更新成功"));
		}else{
			ui->pushButton_into_update_file->setText(QString("程序更新成功"));
			ui->pushButton_into_update_file->setEnabled(false);
		}
	}
}






