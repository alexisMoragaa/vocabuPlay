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


bool preguntasSeleccionadas[500] =  {false};
int getRamdomQuestion();//inicializamos la funcion que retorna una pregunta de forma aleatoria
bool checkPlayStatus(); //inicializamos la funcion que determina el status de reproducción


int randomQuestion;
const int nextQuestion = 3; //usamos el pin 3 para leer el boton que cambiara la pregunta👈
int contadorPreguntasSeleccionadas = 0;//inicializamos el contador de preguntas respondidas


void setup() {
  for (int i = 0; i < tamano; i++) {
        preguntasSeleccionadas[i] = false;
  }
  Serial.begin(9600);                                           // Initialize serial communications with the PC, COMMENT OUT IF IT FAILS TO PLAY WHEN DISCONNECTED FROM PC
  mySoftwareSerial.begin(9600);
  SPI.begin();

  pinMode(nextQuestion, INPUT_PULLUP);

  Serial.println(F("Inicializando DFPlayer ... (Esto puede tomar algunos segundos)"));
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
  //Control del volumen 👇
  volumen = map(analogRead(volumenPot), 0, 1023, 0, 30);  
  if (volumen - prevVolumen >= 3 || prevVolumen - volumen >= 3 ){
    myDFPlayer.volume(volumen);
    Serial.println(volumen);  
    prevVolumen = volumen;
    delay(1);
  }
  //Control del volumen 👆
  
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

