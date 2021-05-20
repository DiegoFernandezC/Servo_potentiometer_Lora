// Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

int pin_pot = 18;    //Potentiometer pin.
int valor_pot;       //Potentiometer value.

// Set frequency.
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  analogReference(EXTERNAL);       //Set config for AREF pin. 5V input to 3.3V input.
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

  delay(100);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(5);
}

void loop() {
  delay(100); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting...");
  
  valor_pot = analogRead(pin_pot);               //Read potentiometer value.
  valor_pot = map(valor_pot, 0, 1023, 0, 255);   //Map to servo range.
  uint8_t radiopacket[1];
  radiopacket[0] = valor_pot;
  Serial.print("Value: "); 
  Serial.println(radiopacket[0]); //Print send data.
  delay(500);
  rf95.send(radiopacket, 1);

  Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  //Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  
  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(100)) { 
    if (rf95.recv(buf, &len)) {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
       Serial.println("Receive failed");
  }    
  else
    Serial.println("No reply.");
}
