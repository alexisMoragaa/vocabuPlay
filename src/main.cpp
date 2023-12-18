// Libereias necesarias para el funciionamiento 👇
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>//Manejo de lecturas y escrituras de las tarjetas rfid
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"//Manejo de DFPlayerMini
#include "PreguntasRespuestas.h"
// Libereias necesarias para el funciionamiento 👆


//Variables para controlar la reprocuccion y volumen 👇
boolean isPlaying = false;
const byte volumenPot = A0;
byte volumen;
int prevVolumen; 
SoftwareSerial mySoftwareSerial(5, 6);// pines de transmision rx y tx

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
//Variables para controlar la reprocuccion y volumen 👆


//Variables para controlar el modulo RFID 👇
#define RST_PIN 9 
#define SS_PIN  10
MFRC522 mfrc522(SS_PIN, RST_PIN);
//Variables para controlar el modulo RFID 👆


//Variables para gestionar la lectura de la tarjeta 👇
#define ELEMENTOS_TARJETA 3
#define MAX_LARGO_TARJETA 10

// char valoresTarjeta[ELEMENTOS_TARJETA][MAX_LARGO_TARJETA];
// char* valoresTarjeta[ELEMENTOS_TARJETA];
//Variables para gestionar la lectura de la tarjeta 👆


bool preguntasSeleccionadas[500] =  {false};
int getRamdomQuestion();//inicializamos la funcion que retorna una pregunta de forma aleatoria
bool checkPlayStatus(); //inicializamos la funcion que determina el status de reproducción


int randomQuestion;
const int nextQuestion = 3; //usamos el pin 3 para leer el boton que cambiara la pregunta👈
int contadorPreguntasSeleccionadas = 0;//inicializamos el contador de preguntas respondidas

void controlarVolumen(); //Declaramos la funcion que se usara en el control de volumen 👈
String leerTarjetas(); //Declaramos la funcion que gestionara la lectura de tarjetas

void setup() {
  //Itera el numero de preguntas existentes en la matriz para establecer el largo final del arreglo
  for (int i = 0; i < tamano; i++) {
        preguntasSeleccionadas[i] = false;
  }

  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();   

  pinMode(nextQuestion, INPUT_PULLUP);

  Serial.println(F("Inicializando DFPlayer... "));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Error al inizar DFPlayer mini"));
  }
  Serial.println(F("DFPlayer mini se inicializo con exito"));

  volumen = map(analogRead(volumenPot), 0,1023, 0,30);//tomamos el valor del potenciometro y lo mapeamos en la variable volumen
  prevVolumen = volumen;
  myDFPlayer.volume(volumen);//Inicializamos el volumen del dfplayer segun la lectura inicial del potenciometro
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);//Establece la ecualizacion en normal 👈

  randomSeed(analogRead(A6));//inicializa un seeder para crear un numero aleatorio

}



void loop() {

  controlarVolumen(); //Gestiona el volumen
  
  //Cambiamos a la siguiente pregunta solo cuando la reproduccion de la pregunta anterior finalice
  if(digitalRead(nextQuestion) == LOW){
    bool playing = checkPlayStatus();
    if(!playing){//chequeamos el estado del reproductor y reproducimos una nueva pregunta solo si no se esta reproduciendo nada
      randomQuestion = getRamdomQuestion();
      myDFPlayer.play(preguntas[randomQuestion].pregunta);
      isPlaying = true;
      delay(250);
    }
  }

  
  String datosTarjeta =  leerTarjetas();// Lee las tarjetas
  if (datosTarjeta != "") {
      Serial.println(datosTarjeta);
    }
}


void controlarVolumen() {
  volumen = map(analogRead(volumenPot), 0, 1023, 0, 30);
  if (abs(volumen - prevVolumen) >= 3) {
    myDFPlayer.volume(volumen);
    Serial.println(volumen);
    prevVolumen = volumen;
  }
}

//creamos una funcion que retorna un numero aleatorio de pregunta
int  getRamdomQuestion(){
  // Verifica si todas las preguntas ya han sido seleccionadas
  if (contadorPreguntasSeleccionadas >= tamano) {
    return 999; 
  }

  int rq;
  do{
    rq = random(0, tamano);
  }while(preguntasSeleccionadas[rq]);

  preguntasSeleccionadas[rq] = true;
  Serial.println(rq);
  contadorPreguntasSeleccionadas++;
  return rq;
}


//Creamos una funcion que nos permite saver si un audio se esta reproduciendo o no
bool checkPlayStatus() {
  if (myDFPlayer.available()) {
    if (myDFPlayer.readType() == DFPlayerPlayFinished) {
      isPlaying = false; // Actualiza el estado a no reproduciendo
    }
  }

  return isPlaying;
}


String leerTarjetas() {

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block, len;
  MFRC522::StatusCode status;

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println(F("**Card Detected:**"));
    // mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

    byte buffer2[18];
    block = 1;
    len = 18;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(1000);
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return "";
    }

    status = mfrc522.MIFARE_Read(block, buffer2, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(1000);
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      return "";
    }

    String lectura = "";
    for (uint8_t i = 0; i < 16; i++) {
      if (buffer2[i] != 0) {
        lectura += (char)buffer2[i];
      }
    }

    delay(200);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    return lectura;
  }
  return "";

}
