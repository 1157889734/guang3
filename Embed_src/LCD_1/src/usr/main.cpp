#include <QtGui/QApplication>
#include "QTextCodec"
#include "MainForm.h"
#include "include.h"
#include <QWSServer>

//int isFrameChange = 0x01; // ��?��???DD��?D?��???
int main(int argc, char *argv[])
{	
    /*
	char localIP[20], setSysIP[100], setSysMac[100];
	read_profile_string("localIP", "1", localIP, 20, "192.168.1.30", CONFIG_PATH);
	sprintf(setSysIP,"ifconfig eth0 %s up", localIP);
	int ip1, ip2, ip3, ip4;
	sscanf(localIP,"%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);
	sprintf(setSysMac,"ifconfig eth0 hw ether 00:0C:%02x:%02x:%02x:%02x",ip1,ip2,ip3,ip4);
	system("ifconfig eth0 down");
	system(setSysMac);
	sleep(1);
	system(setSysIP);
	system("route add 255.255.255.255 eth0");
	isFrameChange = read_profile_int("isFrameChange", "1", 0, CONFIG_PATH);
*/
	memopen();
	sample_mpp_init();
	sample_vo_start(SAMPLE_VO_DEV_DHD0);
	sample_hifb_start();
#if 1
	start_tslib();
#endif
	QApplication a(argc, argv);/* ����һ��QApplication ���󣬹���Ӧ�ó������Դ */
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
	MainForm w;
	w.showFullScreen();/* ��ʾ���в�� */
	QWSServer::setCursorVisible(false);
	return a.exec();/* �ó��������Ϣѭ�����ȴ����ܵĲ˵��������������ȵ����룬������Ӧ��*/
}
