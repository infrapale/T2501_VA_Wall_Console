
#define NBR_DISP_ROWS  4

#include "sensor.h"

extern sensor_entry_st collect_sens[NBR_COLLECTED_SENSORS];
uint16_t row_color[] = 
{
  ILI9341_RED,
  ILI9341_YELLOW,
  ILI9341_CYAN,
  ILI9341_MAGENTA
};

void tft_disp_init(void)
{
  tft.begin();
  if (!ts.begin()) { 
    Serial.println("Unable to start touchscreen.");
  } 
  else { 
    Serial.println("Touchscreen started."); 
    show_menu_millis = millis();
    menu_is_active = false;
    tft.setRotation(3);
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 26);
    tft.setTextColor(ILI9341_WHITE);  
    //tft.setTextSize(1);
    tft.setFont(&FreeSerif24pt7b);
    //tft.setFont(&FreeMonoBoldOblique12pt7b);
    tft.println("VA_3x4KBD_");
    tft.println("TFT_FWing");
    tft.print("2021");
    
    //tft.print("RST pin = "); tft.println(RFM69_RST,DEC);
    //tft.print("CS pin = ");tft.println(RFM69_CS,DEC);
    //tft.print("INT pin = ");tft.println(RFM69_INT,DEC);
    
  }
}

void tft_disp_update(void)
{
  //Serial.println("update_display");
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ILI9341_RED); 
  tft.setFont(&FreeSerif24pt7b); 
  //setTextColor(uint16_t c, uint16_t bg)
  for (int i = 0; i < NBR_DISP_ROWS; i++){
    tft.setTextColor(row_color[i]);
    tft.print(collect_sens[i].name);
    tft.print(" ");
    tft.print(String(collect_sens[i].value,collect_sens[i].nbr_decimals)); 
    tft.print(" ");
    tft.println(collect_sens[i].unit); 
  }
}

void tft_disp_print_msg_log(void)
{
  uint8_t i;
  uint8_t row;
  row = insert_at;
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);

  for(i=0;i< TEXT_VISIBLE_ROWS;i++){
    tft.println(text_buffer[row]);
    //Serial.println(text_buffer[row]);
    if(row == 0 ) {
      row = TEXT_VISIBLE_ROWS-1;
    }  
    else {
       row--;
    }   
  }
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  for (uint8_t row = 1;row < 100; row++){
     tft.print("Hello World! @ row ");
     tft.println(row);
  }
  
  return micros() - start;
}
