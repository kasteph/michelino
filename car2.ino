/*************************************************************
Motor Shield 2-Channel DC Motor Demo
by Randy Sarafan

For more information see:
http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/


#define BACKWARD HIGH
#define FORWARD LOW

#define trigPin 4 
#define echoPin 7
#define led 2

void motor1(int direction, int brake, int speed) {
  digitalWrite(12, direction); //Establishes forward direction of Channel A
  digitalWrite(9, brake);   //Disengage the Brake for Channel A
  analogWrite(3, speed);   //Spins the motor on Channel A at full speed  
}


void motor2(int direction, int brake, int speed) {
  digitalWrite(13, direction);  //Establishes backward direction of Channel B
  digitalWrite(8, brake);   //Disengage the Brake for Channel B
  analogWrite(11, speed);    //Spins the motor on Channel B at half speed
}


void move(int direction) {
  motor1(direction, LOW, 255);
  motor2(direction, LOW, 235);
}

void turn() {
  motor1(FORWARD, LOW, 255);
  motor2(BACKWARD, LOW, 235);
  delay(768);
}

void setup() {
  
  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT);  //Initiates Brake Channel A pin
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
}


void loop(){
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 12) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH); // When the Red condition is met, the Green LED should turn off
    turn();
}
  else {
    digitalWrite(led,LOW);
    move(FORWARD);
  }
}
