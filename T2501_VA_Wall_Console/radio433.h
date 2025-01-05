#ifndef __RADIO433_H__
#define __RADIO433_H__

#define RADIO433_MAX_MSG_LEN  60
bool radio433_reset(void);
void radio433_init(void);
uint8_t radio433_read_msg(char *inp_buf, uint8_t max_len);
void radio433_send_msg(char *rf69_msg );
void radio433_print_registers(void);


#endif
