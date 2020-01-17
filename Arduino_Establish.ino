#include<AESLib.h>
#include<Crypto.h>
#include<eHealth.h>
#include <BLAKE2s.h>
#include <Curve25519.h>
#include <RNG.h>
#include <Ed25519.h>
#include<NeoSWSerial.h>
#include<EEPROM.h>
NeoSWSerial esp8266(3,4);
NeoSWSerial ss(6,5);
#define HASH_SIZE 16
#define KEY_SIZE 32
// using this software serial we could generate the key send over the software serial and do things.
int a=0;
int b=0;
int cont=0;
const char *ID="SBIN00201250";  // an ID if required.
static char string_bpm[16]={0};
static char string_osat[16]={0};
String msg_bpm;
String msg_osat;
uint8_t private_node[32];
uint8_t public_node[32];
char public_sys_temp[32]="76a8737161ed4ac14c352e0bab9268e6c1744d1d4ca6ab966c2d89febc5b0963"; //just a dummy public key.
uint8_t public_sys[32];
int eeAddress=0;
void setup() {
  for(int i=0;i<32;i++) public_sys[i]=public_sys_temp[i];
  Serial.begin(115200);   // to debug.
  eHealth.initPulsioximeter(); // for eHealth
  ss.begin(); // for the interrupt.
  //begin being a server.
  esp8266.begin(115200);
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
  delay(20000);
  Serial.println("AT+server-OK");// a 10 seconds break to enable for making it.
  Serial.println("Quickly connect in 20 seconds."); // to comment later.
  // now the thing must recieve a public key that it must use for this thing.
  //+IPD,id,len:data, required to be used to receieve the packet of data.
  Curve25519::dh1 (private_node, public_node); //generates a k and sends it. 
  Curve25519::dh2 (public_sys, public_node);  //generates a shared key 32 byte key (by signing with the private key).
  // this is all this required to be done. 
  EEPROM.put(eeAddress, public_sys);
}
void loop() {
  // Serial is only for debugging.
  Serial.println("Take data in 10 seconds.");
  ss.attachInterrupt(readPulsioximeter);
  delay(10000);
  ss.detachInterrupt();
  a=eHealth.getBPM();
  b=eHealth.getOxygenSaturation();
  //void *hash // implement a bit later.
  // to encrypt now.
  sprintf(string_bpm,"%02X",a);
  sprintf(string_osat,"%02X",b);
  Serial.println("");
  Serial.print(string_bpm);
  Serial.print(" ");
  Serial.print(string_osat);
  Serial.println(" ");
  aes256_enc_single(public_sys, string_bpm);
  aes256_enc_single(public_sys, string_osat);
  msg_bpm=String(string_bpm);
  msg_osat=String(string_osat);
  String tran=msg_bpm+" "+msg_osat;
  int tran_length=tran.length();;
  String g=(String)tran_length;
  String p="AT+CIPSEND=0,";
  String q=p+g;
  if(esp8266.available()){
    esp8266.println(q); 
    Serial.println("Sent code-came.");
    delay(1000);
    esp8266.println(tran);
    Serial.println("Tran_code_came");
  }
  else {Serial.println("esp8266_not_available");}
  delay(5000);
}
void readPulsioximeter(){  
  cont ++;
  if (cont == 50) { //Get only of one 50 measures to reduce the latency
    eHealth.readPulsioximeter();  
    cont = 0;
  }
}
