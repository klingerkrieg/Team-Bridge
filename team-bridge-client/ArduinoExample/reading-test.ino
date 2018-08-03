//Default baud speed for communication
#include <Servo.h>
#define BAUD 9600
//led 
#define led 13
//macro for on/off
#define on (digitalWrite(led, HIGH))
#define off (digitalWrite(led, LOW))
#define SERVO3 3
#define SERVO4 2
int pos = 0;



Servo s3;
Servo s4;

int input;
String str;
String buff = "";
int motor = -1;
int angulo;


void setup(){
  Serial.begin(BAUD);
  pinMode(led, OUTPUT);
  s3.attach(SERVO3);
  s4.attach(SERVO4);
}

void loop(){
  
  
  //If any input is detected in arduino
  if(Serial.available() > 0){
    //1=180;
    input = Serial.read();

    str = (char)input;

    //str ;
    //buff
    //motor 1
    //angulo 180
    Serial.println(str);
    if (str == ";"){
      
      angulo = buff.toInt();

      
      Serial.print("Motor:");
      Serial.println(motor);
      Serial.print("Angulo:");
      Serial.println(angulo);
      

      if (motor == 1) {
        //Aqui vc faz o motor 1 ou 2 ou 3 oq for, obedecer o "angulo"
        

       braco(angulo);
       
       /* if (angulo < 20){
          digitalWrite(led, HIGH);
          Serial.println("Led is on");
        } else
        if (angulo > 90){
          digitalWrite(led, LOW);
          Serial.println("Led is off");
        }
        */
        
      }

      if(motor ==2){
        garra(angulo);
      
      }
      

      buff = "";
      motor = -1;
    } else
    if (str == "R"){
      motor = -1;
      buff = "";
      Serial.println("Reseted");
    } else
    if (str == "="){
      
      motor = buff.toInt();
      buff = "";
    } else {
      buff += str;
    }

    Serial.flush();

  }
}
 void garra(int angulo){
  angulo+=60;
  if(angulo > 140){
    angulo = 140;
    }
    s3.write(angulo);
 }

 void braco(int angulo){
  angulo+=60;
  if(angulo > 170){
    angulo = 170;
    }
    s4.write(angulo);
 }

/*void subirBraco(int angulo) {
 for (pos = (20); pos >=0; pos -= 1) { 
    
    s3.write(pos);              
    delay(30);                      
  }
  */


