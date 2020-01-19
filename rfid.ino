/*
Pin   Wiring to Arduino Uno
SDA   Digital 10
SCK   Digital 13
MOSI  Digital 11
MISO  Digital 12
IRQ   unconnected
GND   GND
RST   Digital 9
3.3V  3.3V

wifi_signal_receive A0 <- 12
wifi_signal_send A1 -> 14
*/

#include <SPI.h>
#include <MFRC522.h> // 引用程式庫
#include <Servo.h>

#define RST 9 // 讀卡機的重置腳位
#define SS 10 // 晶片選擇腳位
MFRC522 rfid(SS, RST); // 建立MFRC522物件

#define wifi_signal_receive A0
#define wifi_signal_send A1
#define led1 A3
#define led2 A5
#define led3 A2
#define led4 2
int leds[4] = {led1, led2, led3, led4};

Servo myservo;  // create servo object to control a servo


int is_rfid[4] = {0,0,0,0};
int rfid_count = 0;
unsigned long int rfid_num[4] = {0x139B2502,0xFC4FADFE,0x42452602,0x5A36A015};

void setup() {
  for(int i=0; i<4; i++){
    pinMode(leds[i], OUTPUT);
    is_rfid[i] = 0;
  }
  myservo.attach(3);
  myservo.write(145);
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init(); // 初始化MFRC522讀卡機模組
}
void loop() {
  if(analogRead(A0)>512){
    for(int i=0; i<sizeof(is_rfid)/sizeof(int); i++){
      is_rfid[i] = 0;
      digitalWrite(leds[i], LOW); 
    }
    myservo.write(145);
  }
  
  // 確認是否有新卡片
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    
    byte *id = rfid.uid.uidByte; // 取得卡片的UID
    byte idSize = rfid.uid.size; // 取得UID的長度
    
    Serial.print("PICC type: "); // 顯示卡片類型
    // 根據卡片回應的SAK值
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));
    
    Serial.print("UID Size: "); // 顯示卡片的UID長度值
    Serial.println(idSize);

    unsigned long int a = 0;
    for (byte i = 0; i < idSize; i++) { // 逐一顯示UID碼
      Serial.print(id[i], HEX); // 以16進位顯示UID值
      Serial.print(" "); 
      a = a*256 + id[i];
    }

    rfid_count = 0;
    for(int i=0; i<4; i++){
      if(a==rfid_num[i]){ 
        is_rfid[i] = 1;
      }
      if(is_rfid[i]==1){
        rfid_count += 1;
      }
    }
    Serial.println("www");
    Serial.println(rfid_count);

    for(int i=0; i<rfid_count; i++){
      digitalWrite(leds[i], HIGH); 
    }

    if(is_rfid[0]==1 && is_rfid[1]==1 && is_rfid[2]==1 && is_rfid[3]==1){
      Serial.print("open"); 
      digitalWrite(A1, HIGH); 
      myservo.write(65);
    }

    Serial.print("wifi ");
    Serial.print(analogRead(A0));
    if(analogRead(A0)>512){
      Serial.println(" HIGH");
    }else{
      Serial.println(" LOW");
    }
       
    Serial.println();
    rfid.PICC_HaltA(); // 讓卡片進入停止模式
  }
}
