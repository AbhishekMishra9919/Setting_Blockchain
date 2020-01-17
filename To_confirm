#include<NeoSWSerial.h>
#include<AESLib.h>
#include<EEPROM.h>
NeoSWSerial esp8266(3,4);
// there has to be code in a loop
int state=0;
byte value;
char public_sys[32];
char public_sys_b[32]="0093861899c64a317ea9d0c6f188bc54948ec477bf8120ba108ad3970ff52d301e"; // another dummy string.
//for(int i=0;i<32;i++)
//{
  //public_sys_b[i]=test[i];
//}
int address=0;
void setup() {
  // must retrieve the information of the public_sys from EEPROM.
  aes256_enc_single(public_sys_b, public_sys);
  while(address<=EEPROM.length())
  {
    value=EEPROM.read(address);
    public_sys[address]=char(value);
    address++;
  }
  esp8266.begin();
  // now the thing must recieve a public key that it must use for this thing.
}
void loop() {
  // when the state is 0, it tries to listen for requests on a server for requests.
  if(state==0)
  {
    esp8266.println("AT\r\n");
    Serial.println("AT+ok");
    delay(1000);
    esp8266.println("AT+RST");
    delay(1000);
    Serial.println("AT+rst-OK");
    esp8266.println("AT+CWPMODE=3");
    delay(1000);
    Serial.println("AT+cwpmode-OK");
    esp8266.println("AT+CIPAP=192.168.1.200");
    delay(1000);
    Serial.println("AT+ip-OK");
    esp8266.println("AT+CIPMUX=1");
    delay(1000);
    Serial.println("AT+mlti-OK");
    esp8266.println("AT+CIPSERVER=1,12345");
    delay(10000);
    Serial.println("AT+server-OK");// a 10 seconds break to enable for making it.
  // assumed 'A' connects first then 'B' connects.
    String g=String(public_sys_b);
    String p="AT+CIPSEND=0,";
    String q=p+g;
    if(esp8266.available()){
      esp8266.println(q); 
      Serial.println("Sent code-came.");
      delay(1000);
      esp8266.println(tran);
      Serial.println("Tran_code_came");
      state=1;
    }
  }
  if(state == 1)
  // when the state is 1, it expects to listen to a new signal.
  {
    // either esp8266.println("AT+IPD"); if we can succesfully do this.
    // or we do some thing we get a message in the form of a string let us say t now we have to change t;
    // now it must be decrypted.
    uint8_t t_stir[t.length()];
    for(int i=0;i<t.length();i++)
    {
      t_stir[i]=char(t[i]);
    }
    aes_dec_single(t_stir,public_sys_b);
    int i=0;
    for(i=0;i<32;i++)
    {
      if(t_stir[i]!=public_sys[i]) break; 
    }        
    if(i!=32) 
    {
      state=2; // implying the looping must stop.
    }
    else 
    {
      state=0;
      Serial.println("Successfully done.")
    }
  }
  if(state==2)
  {
    Serial.println("Looping_has_been_stopped");
    return; 
  }
}
