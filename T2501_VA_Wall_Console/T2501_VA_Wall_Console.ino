/******************************************************************************
  T2501 VA_Wall_Concole
*******************************************************************************
* Villa Astrid Wall Terminal 
* RFM69 434MHz Radio module
* Tom Höglund 2020, 2025
 *******************************************************************************
https://github.com/infrapale/T2501_VA_Wall_Console
******************************************************************************/


#include <stdio.h>
#include <string.h>
//#include <avr/dtostrf.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif18pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include "secrets.h"
//#include "sens_db.h"
//#include <RH_RF69.h>
#include "radio433.h"


#include "light_msg.h"
#include "sensor.h"
#include "main.h"
#include "tft_disp.h"
#include "akbd.h"
#include "atask.h"
//#include "TaHa.h" 
#include "io.h"


  
#define KBD_NBR_KEYS       12
#define BTN_NBR_BTNS       3
#define LDR_PIN            A2

#define TEXT_SIZE 1
#define TEXT_VISIBLE_ROWS 25
#define TEXT_VISIBLE_CHAR 44

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

char text_buffer[TEXT_VISIBLE_ROWS][TEXT_VISIBLE_CHAR];
uint8_t show_from=0;
uint8_t insert_at=0;
unsigned long show_menu_millis;
boolean menu_is_active;
Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(PIN_STMPE_CS);
int16_t packetnum = 0;  // packet counter, we increment per xmission

// Function prototypes
void debug_print_task(void);
void scan_kbd (void);
void radio_rx_handler(void);
void read_local_sensors(void);
void reset_radio_task(void);




struct sensors_struct {
    uint8_t light_val;
};

sensors_struct sensor;

int16_t dec_div[] = {1,10,100,1000,10000};

char buf[16];
akbd kbd3x4( PIN_ANALOG_KBD);
uint16_t kbd_values[KBD_NBR_KEYS] = {
  52,92,132,198,312,413,550,699,788,871,936,967
};
uint16_t btn_values[BTN_NBR_BTNS] = {707,917,420};
float sensor_value[3] = { 24.1, -5.5, 4.5 };  //indoor -outdoor -water
extern sensor_entry_st collect_sens[NBR_COLLECTED_SENSORS];

// Task handler definitions

atask_st debug_print_handle       = {"Debug Print    ", 5000,   0, 0, 255, 0, 1, debug_print_task};
atask_st kbd_scan_handle          = {"Kbd Scan       ", 10,     0, 0, 255, 0, 1, scan_kbd};
atask_st radio_receive_handle     = {"Radio Receive  ", 10,     0, 0, 255, 0, 1, radio_rx_handler};
atask_st display_handle           = {"Display        ", 30000,  0, 0, 255, 0, 1, tft_disp_update};
atask_st reset_radio_handle       = {"Reset Radio    ", 60000,  0, 0, 255, 0, 1, reset_radio_task};

void initialize_tasks(void)
{
    atask_initialize();
    atask_add_new(&debug_print_handle);
    atask_add_new(&kbd_scan_handle);
    atask_add_new(&radio_receive_handle);
    atask_add_new(&display_handle);
    atask_add_new(&reset_radio_handle);

}

/**
 * @brief  Scan Analog Keyboard, pressed keys are stored in object buffer
 * @param  -
 * @retval -
 */
void scan_kbd (void)
{
  kbd3x4.scan();
}

/**
 * @brief Arduino setup function
 * @param -
 * @retval -
 */
void setup() {
  delay(3000);
  Serial.begin(9600);
  Serial.print(APP_NAME); Serial.print(" Compiled: ");
  Serial.print(__DATE__);Serial.print(" - "); Serial.print(__TIME__);
  //menu_init();
  radio433_init();
  radio433_send_msg(APP_NAME);
  init_light_msg();
  
  kbd3x4.set_nbr_keys(KBD_NBR_KEYS);
  kbd3x4.enable_on_off(true);
  
  for(uint8_t i = 0;i < KBD_NBR_KEYS; i++){ 
      kbd3x4.set_aval(i, kbd_values[i]);
  }
  initialize_tasks();
  tft_disp_init();
  init_light_msg();
  sensor_initialize();
}
/**
 * @brief Run tasks  via scheduler 
 * @param -
 * @retval -
 */
void loop() {
  char btn;
  atask_run();
  
  btn = kbd3x4.read();
  if (btn) {
    Serial.print(btn);
    //uint16_t aval = kbd3x4.rd_analog();Serial.print(" ana: "); Serial.println(aval);
    light_msg_action(btn);
    //radio433_print_registers();
  }

 }

void radio_rx_handler(void)
{
     // Should be a message for us now   
      char buf[RADIO433_MAX_MSG_LEN+1];
      uint8_t len = sizeof(buf);
  
      len = radio433_read_msg(buf, RADIO433_MAX_MSG_LEN);
      //if (rf69.recv(buf, &len)) {
      if (len>0)
      {
          buf[len] = 0;
          Serial.print("Received [");
          Serial.print(len);
          Serial.print("]: ");
          Serial.println((char*)buf);
          //Serial.print("RSSI: ");
          //Serial.println(rfm69.lastRssi(), DEC);
          AddRow((char*)buf);
          parse_msg((char*)buf);
          //printMsgLog();
 
     } 
}

void reset_radio_task(void)
{
     radio433_reset();
}


void debug_print_task(void)
{
  // atask_print_status(true);
}

