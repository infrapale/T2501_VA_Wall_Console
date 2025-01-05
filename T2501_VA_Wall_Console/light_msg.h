#ifndef __LIGHT_MSG_H__
#define __LIGHT_MSG_H__
#include <Arduino.h>
#include <Wire.h>
//#include <rfm69_support.h>

#define CODE_LEN      6
#define ZONE_LEN      4
#define FUNC_LEN      4

void init_light_msg(void);
void light_msg_action(char btn);
void add_code(const char *new_zone, const char *new_code, const char *new_func);
void radiate_msg( const char *zone, const char *relay_addr, char *func );
void radio_tx_handler(void);

#endif
