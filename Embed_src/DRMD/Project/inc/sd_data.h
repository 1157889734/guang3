#ifndef SD_DATA_H
#define SD_DATA_H

typedef struct 
{
    uint32_t start_addr;
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t reverse;
}index_data_t;

extern void fill_sd_data_fifo(uint8_t *buf, uint16_t length, uint8_t in_out, uint8_t indx);
extern void sd_data_init(void);
extern void set_rtc_date(uint8_t year, uint8_t month, uint8_t date);
extern void set_rtc_time(uint8_t hour, uint8_t min, uint8_t sec);
extern void get_sd_data(index_data_t *from, index_data_t *to);
#endif
