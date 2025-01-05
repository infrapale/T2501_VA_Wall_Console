#include <SPI.h>
#include <RH_RF69.h>
#include "secrets.h"
#include "io.h"



/*
 * RFM69 Definitions for the M0+RFM69 Feather
 */
#define FREQUENCY     RF69_434MHZ
#define RFM69_CS      PIN_RFM69_CS
#define RFM69_INT     PIN_RFM69_INT
#define RFM69_RST     PIN_RFM69_RST
#define RFM69_FREQ    434.0 
#define RFM69_TX_IVAL_100ms  20;

struct rfm69_struct {
    byte rfm_cs_pin; 
    byte rfm_irqn_pin; 
    byte rfm_rst_pin;
    float rfm_freq;
    bool  is_ok;
};

uint8_t rd_buf[RH_RF69_MAX_MESSAGE_LEN+1];
rfm69_struct rfm69;

// RH_RF69(uint8_t slaveSelectPin = SS, uint8_t interruptPin = 2, RHGenericSPI& spi = hardware_spi);
// radio_init(RFM69_CS,RFM69_INT,RFM69_RST, RFM69_FREQ);
RH_RF69 rf69(RFM69_CS, RFM69_INT);

bool radio433_reset(void)
{
    // manual reset
    digitalWrite(rfm69.rfm_rst_pin, HIGH); delay(100);
    digitalWrite(rfm69.rfm_rst_pin, LOW); delay(100);

    if (!rf69.init()) 
    {
       Serial.println("RFM69 rf69 init failed");
       rfm69.is_ok = false;      
    }
    else
    {
        Serial.println("RFM69 rf69 init OK!");
        rfm69.is_ok = true;
        // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
        // No encryption
        if (!rf69.setFrequency(rfm69.rfm_freq)) 
        {
            rfm69.is_ok = false;
            Serial.println("setFrequency failed");
        }
    
        // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
        // ishighpowermodule flag set like this:
        rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
    
        rf69.setEncryptionKey((uint8_t*)RFM69_KEY);
        
    }
    return rfm69.is_ok;
}
  

void radio_init(byte rfm_cs_pin, byte rfm_irqn_pin, byte rfm_rst_pin,float rfm_freq)
{   
    //static RH_RF69 _rf69(rfm_cs_pin, rfm_irqn_pin);
    //rfm69.rfm =  new RH_RF69(rfm_cs_pin, rfm_irqn_pin);
    rfm69.rfm_cs_pin = rfm_cs_pin;
    rfm69.rfm_irqn_pin = rfm_irqn_pin;
    rfm69.rfm_rst_pin = rfm_rst_pin;
    rfm69.rfm_freq = rfm_freq;
    pinMode(rfm69.rfm_rst_pin, OUTPUT);
    digitalWrite(rfm69.rfm_rst_pin, LOW);
    //rf69.RH_RF69(rfm_cs_pin, rfm_irqn_pin);
    if (radio433_reset())
    {
       Serial.print("RFM69 rf69 @");  Serial.print((int)rfm_freq,DEC);  Serial.println(" MHz");    
    }
} 

void radio433_init(void)
{
   radio_init(RFM69_CS,RFM69_INT,RFM69_RST, RFM69_FREQ);
}


void radio433_send_msg( char *data ) {
    if (rfm69.is_ok)
    {
        uint8_t msg_len;
        msg_len = strlen(data);
        Serial.print("radio433_send_msg: ");
        Serial.print(data);Serial.print(" len = ");Serial.println(msg_len);
        rf69.send((uint8_t*)data, msg_len);
        rf69.waitPacketSent();       
    }
    else
    {
        Serial.println("Radio 433 initaialization fialed!! ");
    }
}


uint8_t radio433_read_msg(char *inp_buf, uint8_t max_len){
   uint8_t len = max_len;          
   boolean result = false;
   if (rfm69.is_ok)
   {
       if (rf69.available()) {
           if (rf69.recv(rd_buf, &len)) {
               Serial.print("Read available, len="); Serial.println(len);
               uint8_t i = 0;
               do {
                   inp_buf[i] = (char)rd_buf[i];
                   i++;
                } while ((rd_buf[i] != 0) && (i < max_len));
                inp_buf[i] = 0; 
                len = i;
                
                if (len> 0){
                    Serial.print("Received ["); Serial.print(len); Serial.print("]: ");
                    Serial.println(inp_buf); Serial.print("RSSI: "); Serial.println(rf69.lastRssi(), DEC);
               }
           }       
           radio433_reset();
       }
       else
       {
         len = 0;
       }   
   }
   else 
   {
      radio433_reset();
      len = 0;    
   }
   return len;
}

void radio433_print_registers(void)
{
  rf69.printRegisters();
}
