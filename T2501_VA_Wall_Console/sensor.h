#ifndef __SENSOR_H__
#define __SENSOR_H__

#define NBR_COLLECTED_SENSORS 4


typedef struct{
    String name;
    String zone;
    String sensor;
    float value;  
    String unit;
    uint8_t nbr_decimals;
} sensor_entry_st;

void sensor_initialize(void);
void test_sens_db(void);
void parse_msg(char *rad_msg);


#endif
