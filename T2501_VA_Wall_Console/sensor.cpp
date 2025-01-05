#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "sensor.h"
#include "atask.h"



// {"Z":"Dock","S":"P_bmp180","V":997.00,"R":""}
#define MSG_ATTRIBUTES 4

void sensor_task(void);

typedef struct
{
  uint8_t task_indx;
} sensor_st;


sensor_entry_st sensor_info;

sensor_entry_st collect_sens[NBR_COLLECTED_SENSORS] = {
    {"Tupa","Tupa","Temp",24.2,"C",1},
    {"Tupa","Tupa","Hum",22.1,"%",0},
    {"Ulko", "Dock","T_bmp180",-1.0,"C",1},
    {"Vesi ","Dock","T_Water",5.0,"C",1}
    
};

sensor_st zenzor;

Adafruit_BME680 bme; // I2C
atask_st th        = {"Sensor         ", 1000,    0, 0, 255, 0, 1, sensor_task};


// " hPa"

void sensor_initialize(void)
{

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    // while (1);
  }  
  zenzor.task_indx = atask_add_new(&th);

}



void test_sens_db(void){
    Serial.println("Test sens_db");
    for( int i = 0; i < NBR_COLLECTED_SENSORS; i++){          
        Serial.print(collect_sens[i].name);
        Serial.print(" - ");
        Serial.print(collect_sens[i].zone);
        Serial.print(" - ");
        Serial.print(collect_sens[i].sensor);
        Serial.print(" - ");
        Serial.print(collect_sens[i].value);
        Serial.println();
    }
}

void parse_msg(char *rad_msg){
   int attr_pos[MSG_ATTRIBUTES];
   int attr_end[MSG_ATTRIBUTES];
   String attributes[MSG_ATTRIBUTES];
   String msg_str;

   
   msg_str = rad_msg;
   Serial.print("String= "); Serial.println(msg_str);
   attr_pos[0] = msg_str.indexOf("{\"Z\":") + 5;
   attr_pos[1] = msg_str.indexOf(",\"S\":") + 5;
   attr_pos[2] = msg_str.indexOf(",\"V\":") + 5;
   attr_pos[3] = msg_str.indexOf(",\"R\":") + 5;
   boolean attr_found = true;
   Serial.println("Search Attributes");
   for(int i = 0; i<MSG_ATTRIBUTES;i++){
      if(attr_pos[i] < 0){
        attr_found = false;
        Serial.println("Attribute[i] not found");
      } 
   }
   if (attr_found)
   {
     for(int i = 0; i < MSG_ATTRIBUTES;i++){
       int end_pos = -1;
       Serial.println(i);
       if (msg_str.charAt(attr_pos[i]) == '\"'){
         attr_pos[i]++;
         end_pos = msg_str.indexOf('\"',attr_pos[i]);
       }  
       else {  //not a string value
         end_pos = msg_str.indexOf(',',attr_pos[i]);
         if (end_pos < 0) end_pos = msg_str.indexOf('}',attr_pos[i]);
       }
       if (end_pos < attr_pos[i]){
         attr_found = false;
         Serial.println("missing double qoute");
       } else {
         attributes[i] = msg_str.substring(attr_pos[i],end_pos);
         Serial.print(i); Serial.print(" -- ");
         Serial.println(attributes[i]);
       }
     }
     for( int i = 0; i < NBR_COLLECTED_SENSORS; i++)
     {   
         if( attributes[0].equals(collect_sens[i].zone) &&
            attributes[1].equals(collect_sens[i].sensor))
         {
            collect_sens[i].value = attributes[2].toFloat();
            break;   
         }
     }    
   }
   else {
     Serial.println("Missing JSON tags");
   }   
   
   //Serial.println(msg_str.indexOf("{\"Z\":"));
   //Serial.println(msg_str.indexOf("\"S\":"));
   //Serial.println(msg_str.indexOf("\"V\":"));
   //Serial.println(msg_str.indexOf("\"R\":"));
   //Serial.println(msg_str.indexOf("Dock"));
   //Serial.println(msg_str.indexOf("OD_1"));
   
   //NBR_COLLECTED_SENSORS
}

void sensor_task(void)
{
  switch(th.state)
  {
    case 0:
      // Set up oversampling and filter initialization
      bme.setTemperatureOversampling(BME680_OS_8X);
      bme.setHumidityOversampling(BME680_OS_2X);
      bme.setPressureOversampling(BME680_OS_4X);
      bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
      bme.setGasHeater(320, 150); // 320*C for 150 ms

      th.state = 10;
      break;
    case 10:
      if (bme.performReading())
      {
          collect_sens[0].value = bme.temperature;
          collect_sens[1].value = bme.humidity;     
          //Serial.print("Temperature = "); Serial.print(bme.temperature); Serial.println(" *C");         
      }
      break;
    case 20:
      break;
  }
}


