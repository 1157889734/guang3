#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QtGui>

#include <QDir>
#include <QFileInfo>
#include <QTextStream>

namespace Ui
{
	class MainForm;
}

class MainForm : public QWidget
{
	Q_OBJECT /* �궨�壬����������õ���signal����slots����Ҫ��������� */
public:
	MainForm(QWidget *parent = 0);/* ���캯����Qt �ؼ���ı�׼��ʽ��Ĭ�ϵĸ�����ΪNULL��˵��û�и��ؼ���*/
	~MainForm();

protected:
	void changeEvent(QEvent *e);
private:
	Ui::MainForm *ui;
	void init_mainform();
	 //��ť��
	void init_choose_addr_buttons();
	void init_choose_temp_broadcast_buttons();
	void init_choose_line_buttons();
	void init_choose_line9_addr_buttons();
	void init_logList();
	void init_errorList();
	void init_Error_message_check_rep();
	void update_choose_next_station();

	void Write(QString &Filename);
	void Read(QString &Filename);
	int cmd_dir_Log();
	int cmd_dir_Err();
	void Get_Filelist_Log();
	void Get_Filelist_Err();

       int bro_num;
	
private slots: //������slot���� slotsҲ��һ����
	
	//��ʼ��
	void init_main_button_setEnabled();//�������
	void init_hide_addr_button();//���ذ�ť
	void init_hide_temp_broadcast_button();
	void init_hide_log_button();
	void init_hide_error_button();
	void init_configuration_file();
	void TextOpen_Log();
	void TextOpen_Err();
	void loadFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);

	//������
	void on_pushButton_broadcast_play_clicked();
	void on_pushButton_broadcast_stop_clicked();
	void on_pushButton_temp_broadcast_mode_clicked();
	void on_pushButton_select_addr_up_clicked();
	void on_pushButton_select_addr_down_clicked();
	void on_pushButton_choose_line_clicked();
	void on_pushButton_arrive_next_station_2_clicked();
	void on_pushButton_systemset_clicked();
	void on_pushButton_error_clicked();

	//ѡ·�߽���
	void on_pushButton_broadcast_back_5_clicked();
	void on_init_choose_line_buttons_clicked(int choose_line_id);
	void on_pushButton_choose_line_affirm_clicked();
	void on_pushButton_choose_route_1_clicked();
	void on_pushButton_choose_route_2_clicked();
	void on_pushButton_choose_route_3_clicked();
	void on_pushButton_choose_route_4_clicked();
	void on_pushButton_choose_route_5_clicked();
	void on_pushButton_choose_route_6_clicked();
	void on_pushButton_choose_route_7_clicked();

	//����վ������
	void on_pushButton_broadcast_back_4_clicked();
	void on_pushButton_addr_name_choose_down_clicked();
	void on_pushButton_addr_name_choose_up_clicked();
	void page_turn_funcion();
	void on_choose_addr_button_group_clicked(int addr_id);
	void on_pushButton_addr_name_confirm_clicked();

	//(��һվѡ�����)�Զ������
	void on_pushButton_custom_route_back_clicked();	
	void page_turn_funcion9();
	void on_choose_line9_addr_button_group_clicked(int line9_addr_id);
	void on_pushButton_line9_addr_name_down_clicked();
	void on_pushButton_line9_addr_name_up_clicked();
	void on_pushButton_custom_route_confirm_clicked();
	void on_pushButton_turn_line_clicked();
	void on_pushButton_turn_line_2_clicked();

	//���ý���
	void on_pushButton_systemset_back_clicked();
	void on_pushButton_log_clicked();
	int on_pushButton_error_message_clicked();
	void on_pushButton_mute_or_monitor_clicked();
	void on_pushButton_system_message_clicked();
	
	//�����㲥����
	void page_turn_funcion2();
	void on_pushButton_broadcast_back_3_clicked();
	void on_pushButton_temp_broadcast_choose_down_clicked();
	void on_pushButton_temp_broadcast_choose_up_clicked();
	void on_choose_temp_braodcast_button_group_clicked(int temp_braodcast_id);
	void on_pushButton_temp_broadcast_play_clicked();
	void on_pushButton_temp_broadcast_stop_clicked();

	//��־����
	void on_pushButton_broadcast_back_9_clicked();	
	void page_turn_funcion_log();
	void on_pushButton_log_up_clicked();
	void on_pushButton_log_down_clicked();
	int TurnPageLog();
	void on_pushButton_textedit_page_down_clicked();
	void on_pushButton_textedit_page_up_clicked();

	//������Ϣ����
	void on_pushButton_error_message_back_clicked();
	void page_turn_funcion_error();
	void on_pushButton_error_up_clicked();
	void on_pushButton_error_down_clicked();
	void on_pushButton_error_time_roll_clicked();
	void on_pushButton_error_start_time_roll_clicked();
	void on_pushButton_error_end_time_roll_clicked();
	int TurnPage();

	//�������ý���
	void on_pushButton_error_message_back_volume_clicked();
	void on_pushButton_volume_up_clicked();
	void on_pushButton_volume_down_clicked();
	void on_pushButton_set_moin_clicked();

	//ϵͳ�汾����
	void on_pushButton_system_message_back_clicked();
	void init_system_datetime_edit();
	void on_pushButton_set_datetime_clicked();
	void on_pushButton_into_update_file_clicked();
	
	
	//��ʱ����ѯ
	void show_pushButton_addr_name();
	void show_client_talk_flag();
	void show_client_talk_rever_flag();
	void show_systime_timeoutslot();
	void show_error_message();
	void show_emery_boarding_message();
	
};

#endif // MAINFORM_H
