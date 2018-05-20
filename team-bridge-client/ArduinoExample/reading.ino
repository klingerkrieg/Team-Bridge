//Default baud speed for communication
#define BAUD 9600
//led 
#define led 13
//macro for on/off
#define on (digitalWrite(led, HIGH))
#define off (digitalWrite(led, LOW))

void setup(){
  Serial.begin(BAUD);
  pinMode(led, OUTPUT);
}

void loop(){
  int input;
  //If any input is detected in arduino
  if(Serial.available() > 0){
    //read the whole string until '\n' delimiter is read
    //input = Serial.readStringUntil('-');
    //input = Serial.readString();
    input = Serial.read();
    
    //If input == "ON" then turn on the led 
    //and send a reply
    /*Serial.println("Recebido:");
    Serial.println(input);
    Serial.println("-------");*/
    
    if (input == 65){
      digitalWrite(led, HIGH);
      Serial.println("Led is on");
    } 
    //If input == "OFF" then turn off the led 
    //and send a reply
    else if (input == 66){
      digitalWrite(led, LOW);
      //Serial.println("Led is off");
    }
  }
}
