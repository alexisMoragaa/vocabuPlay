// Libereias necesarias para el funciionamiento 👇👇
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>//Libreria para el manejo de lecturas y escrituras de las tarjetas rfid 👈
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"//Libreria para el manejo de DFPlayerMini 👈
// Libereias necesarias para el funciionamiento 👆👆


//Variables para controlar la reprocuccion y volumen 👇👇
boolean isPlaying = false;
const byte volumenPot = A0;
byte volumen;
int prevVolumen; 
SoftwareSerial mySoftwareSerial(5, 6);// pines de transmision rx y tx

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void ajustarVolumen();//inicializamos la funcion que ajusta el volumen
//Variables para controlar la reprocuccion y volumen 👆👆

void setup() {
  Serial.begin(115200);                                           // Initialize serial communications with the PC, COMMENT OUT IF IT FAILS TO PLAY WHEN DISCONNECTED FROM PC
  mySoftwareSerial.begin(9600);
  SPI.begin();

  Serial.println(F("Inicializando DFPlayer ... (Esto puede tomar algunos segundos)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
  }
  Serial.println(F("DFPlayer Mini online. Place card on reader to play a spesific song"));


  volumen = map(analogRead(volumenPot), 0,1023, 0,30);//tomamos el valor del potenciometro y lo mapeamos en la variable volumen
  prevVolumen = volumen;
  myDFPlayer.volume(volumen);//Inicializamos el volumen del dfplayer segun la lectura inicial delpotenciometro
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);//Establece la ecualizacion en normal 👈

}


void loop() {
  // put your main code here, to run repeatedly:

   volumen = map(analogRead(volumenPot), 0, 1023, 0, 30);   //scale the pot value and volume level

  if (volumen - prevVolumen >= 3 || prevVolumen - volumen >= 3 ){
    myDFPlayer.volume(volumen);
    Serial.println(volumen);  
    prevVolumen = volumen;
    delay(1);
  }

  
  if(!isPlaying){
    myDFPlayer.play(0001);
    isPlaying = true;
  }

  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
}


void ajustarVolumen(){
 
}