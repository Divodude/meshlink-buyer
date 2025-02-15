#include<RF24.h>
#include<SPI.h>
#include<RF24Audio.h>
RF24 radio(D4,D2);
RF24Audio rfAudio(radio,0);
int talkButton=3;
void setup(){
  Serial.begin(115200);
  radio.begin();
  rfAudio.begin();
  rfAudio.receive();


}
void talk(){
  rfAudio.transmit();
}
void loop(){

}

