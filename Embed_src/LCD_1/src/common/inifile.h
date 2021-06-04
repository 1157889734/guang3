/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 * @author Deng Yangjun
 * @date 2007-1-9
 * @version 0.2
 */
 
#ifndef INI_FILE_H_
#define INI_FILE_H_

#ifdef __cplusplus
extern "C"
{

#endif

#define MAX_FILE_SIZE 1024*60//60k

int load_ini_file(const char *file, char *buf,int *file_size);
int read_profile_string( const char *section, const char *key,char *value, int size,const char *default_value, const char *file);
int read_profile_int( const char *section, const char *key,int default_value, const char *file);
int read_profile_int_16( const char *section, const char *key,int default_value, 	const char *file);
void read_profile_char_from_buf( const char *section, const char *key,int default_value, char *read_info, char *buf);
int read_profile_int_from_buf( const char *section, const char *key,int default_value, char *buf);
int read_profile_int_16_from_buf( const char *section, const char *key,int default_value, char *buf);
int write_profile_string( const char *section, const char *key,const char *value, const char *file);

#ifdef __cplusplus
}; //end of extern "C" {
#endif

#endif //end of INI_FILE_H_

