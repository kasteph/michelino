#define BACKWARD HIGH
#define FORWARD LOW
#define LEFT 0
#define RIGHT 1

#define trigPin 0 
#define echoPin 1
#define led 7

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include "utility/socket.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed

#define WLAN_SSID       "Hacker School"
#define WLAN_PASS       "nevergraduate!"
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80 
#define MAX_ACTION            10      // Maximum length of the HTTP action that can be parsed.

#define MAX_PATH              64      // Maximum length of the HTTP request path that can be parsed.
                                      // There isn't much memory available so keep this short!

#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20  // Size of buffer for incoming request data.
                                                          // Since only the first line is parsed this
                                                          // needs to be as large as the maximum action
                                                          // and path plus a little for whitespace and
                                                          // HTTP version.

#define TIMEOUT_MS            500    // Amount of time in milliseconds to wait for
                                     // an incoming request to finish.  Don't set this
                                     // too high or your server could be slow to respond.




/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/

uint32_t ip;

Adafruit_CC3000_Server httpServer(LISTEN_PORT);
uint8_t buffer[BUFFER_SIZE+1];
int bufindex = 0;
char action[MAX_ACTION+1];
char path[MAX_PATH+1];
bool moving = false;
int direction = FORWARD;

void motor1(int direction, int speed) {
  digitalWrite(8, direction); //Establishes forward direction of Channel A
  analogWrite(6, speed);   //Spins the motor on Channel A at full speed  
}


void motor2(int direction, int speed) {
  digitalWrite(2, direction);  //Establishes backward direction of Channel B
  analogWrite(9, speed);    //Spins the motor on Channel B at half speed
}

void move(int direction) {
  motor1(direction, 255);
  motor2(direction, 235);
}

void stop() {
  motor1(FORWARD, 0);
  motor2(FORWARD, 0);
}

void turn(int direction) {
  motor1(direction == LEFT ? FORWARD : BACKWARD, 255);
  motor2(direction == LEFT ? BACKWARD : FORWARD, 235);
  delay(314);
}

void wifiSetup() {
  if (!cc3000.begin())
  {    
    while(1);
  }

  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {    
    while(1);
  }
   
  while (!cc3000.checkDHCP())
  {
    delay(100);
  }
  
  httpServer.begin();
}

void wifiLoop(void) {
  Adafruit_CC3000_ClientRef client = httpServer.available();
  if (client) {
    bool turn_left = false, turn_right = false, change_direction = false;

    bufindex = 0;
    memset(&buffer, 0, sizeof(buffer));
    memset(&action, 0, sizeof(action));
    memset(&path,   0, sizeof(path));

    // Set a timeout for reading all the incoming data.
    unsigned long endtime = millis() + TIMEOUT_MS;
    
    // Read all the incoming data until it can be parsed or the timeout expires.
    bool parsed = false;
    while (!parsed && (millis() < endtime) && (bufindex < BUFFER_SIZE)) {
      if (!client.available())
        break;

      buffer[bufindex++] = client.read();
      parsed = parseRequest(buffer, bufindex, action, path);
    }

    // Handle the request if it was parsed.
    if (parsed) {
      // Check the action to see if it was a GET request.
      if (strcmp(action, "GET") == 0) {
        // Respond with the path that was accessed.
        // First send the success response code.
        client.fastrprintln(F("HTTP/1.1 200 OK"));
        // Then send a few headers to identify the type of data returned and that
        // the connection will not be held open.
        client.fastrprintln(F("Content-Type: text/plain"));
        client.fastrprintln(F("Connection: close"));
        client.fastrprintln(F("Server: Adafruit CC3000"));
        // Send an empty line to signal start of body.
        client.fastrprintln(F(""));
        // Now send the response data.
        if (strcmp(path, "/start") == 0) {
          moving = true;
          client.fastrprintln(F("starting"));
        } else if (strcmp(path, "/stop") == 0) {
          moving = false;
          client.fastrprintln(F("stopping"));
        } else if (strcmp(path, "/left") == 0) {
          turn_left = true;
          client.fastrprintln(F("turning left"));
        } else if (strcmp(path, "/right") == 0) {
          turn_right = true;
          client.fastrprintln(F("turning right"));
        } else if (strcmp(path, "/backward") == 0) {
          change_direction = true;
          direction = BACKWARD;
          client.fastrprintln(F("going backward"));
        } else if (strcmp(path, "/forward") == 0) {
          change_direction = true;
          direction = FORWARD;
          client.fastrprintln(F("going forward"));
        } else {
          client.fastrprint(F("unknown command: ")); client.fastprintln(path);
        }
      }
      else {
        // Unsupported action, respond with an HTTP 405 method not allowed error.
        client.fastrprintln(F("HTTP/1.1 405 Method Not Allowed"));
        client.fastrprintln(F(""));
      }
    }

    // Wait a short period to make sure the response had time to send before
    // the connection is closed (the CC3000 sends data asyncronously).
    delay(100);

    // Close the connection when done.
    client.close();

    if (turn_left) {
      turn(LEFT);
    }
    else if (turn_right) {
      turn(RIGHT);
    }
    else if (change_direction) {
      stop();
    }
  }
}

void setup(void) {
  
  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  
  wifiSetup();
  digitalWrite(led, HIGH);
}


void loop(){
  wifiLoop();
  long duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 12) {  // This is where the LED On/Off happens
    digitalWrite(led, LOW); // When the Red condition is met, the Green LED should turn off
    turn(LEFT);
  }
  else {
    digitalWrite(led, HIGH);
    if (moving) {
      move(direction);
    } else {
      stop();
    }
  }
}

// Parse the action and path from the first line of an HTTP request.
bool parseFirstLine(char* line, char* action, char* path) {
  // Parse first word up to whitespace as action.
  char* lineaction = strtok(line, " ");
  if (lineaction == NULL)
    return false;
  strncpy(action, lineaction, MAX_ACTION);

  // Parse second word up to whitespace as path.
  char* linepath = strtok(NULL, " ");
  if (linepath == NULL)
    return false;
  strncpy(path, linepath, MAX_PATH);

  return true;
}

bool parseRequest(uint8_t* buf, int bufSize, char* action, char* path) {
  // Check if the request ends with \r\n to signal end of first line.
  if (bufSize < 2)
    return false;
  if (buf[bufSize-2] != '\r' || buf[bufSize-1] != '\n')
    return false;

  return parseFirstLine((char*)buf, action, path);
}
