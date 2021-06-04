#ifndef __UPDATE_PROCESS_H__
#define __UPDATE_PROCESS_H__

int update_from_local(char *file_path);
int update_from_tftp(char *tftp_server_ip, char *file_name);
//int updata_from_tftp_configuration(char *tftp_server_ip, char *file_name,char *download_path);
int updata_from_tftp_file(char *tftp_server_ip, char *file_name,char *download_path);


#endif
