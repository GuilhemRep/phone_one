#include <SoftwareSerial.h>

// Ecran : Arduino
//    RX (jaune) : 11
//    TX (bleu)  : 10 

SoftwareSerial nextion(10, 11);

// En cas d'appel entrant, pour que la page correspondante ne s'affiche qu'une fois
bool sonneries = true;

// Pour terminer le démarage et accéder au menu
bool boot = false;

// Broche optocoupleur (pour maintenir l'alimentation après relachement du bouton d'allumage)
#define opto 8

// Broche allumage modem
#define simTrigger 9

// Broche extinction de l'ensemble
#define offBt 10

// Envoi des caractères ÿÿÿ pour marquer la fin d'une commande
void yyy(){
  for(int i=0; i<=2; i++){
    nextion.write(0xff);
  }
}

// Transmission (directe) de Nextion à SIM900
void nextion2sim(){
  if (nextion.available()) {
    Serial.write(nextion.read());
  }
}

// Transmission "traduite" de SIM900 à Nextion
void sim2nextion(){
  String msg;
  // Appel entrant
  if (Serial.available()) {
    msg = Serial.readStringUntil('\r');
    
    if(msg == "RING" || msg == "\nRING"){
      if(sonneries){
        nextion.print("page appelE");
        yyy();
        sonneries = false;
      } else{}
    }
    
    // Appel manqué (fuck)
    if(msg == "NO CARRIER" || msg == "\nNO CARRIER"){
      nextion.print("page menu");
      yyy();
      sonneries = true;
    }
    
    else{}
  }
}

// Extinction modem et coupure alimentation si broche offBt à l'état bas
void turnOff(){
  digitalWrite(simTrigger, HIGH);
  delay(1000);
  digitalWrite(simTrigger, LOW);
  delay(2000);
    // Coupure alimentation
  digitalWrite(opto, LOW);
  // Bye Bye XXX
}

//Démarage
void setup() {
  pinMode(opto, OUTPUT);
  pinMode(offBt, INPUT_PULLUP);
  digitalWrite(opto, HIGH);
  
  // Allumage modem
  digitalWrite(simTrigger, HIGH);
  delay(1000);
  digitalWrite(simTrigger, LOW);
  
  Serial.begin(19200);
  nextion.begin(19200);

  String msg;
  while(!boot){
    if (Serial.available()) {
      msg = Serial.readStringUntil('\r');
      if(msg == "Call Ready" || msg == "\nCall Ready"){
        nextion.print("page menu");
        yyy();
        boot = true;
      }
      else{}
    }
    else{}
  }
}

//Boucle
void loop(){
  nextion2sim();
  sim2nextion();
  if(!digitalRead(offBt)) turnOff();
}
