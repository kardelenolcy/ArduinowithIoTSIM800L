
#include "Nextion.h" // for Nextion Display
#include "HX711.h" // for load amplifier

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;
HX711 scale;



String get_loadcell();
void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(String data);
boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000);


#include <SoftwareSerial.h>
//GSM Module RX pin to Arduino 11
//GSM Module TX pin to Arduino 10
#define rxPin 11
#define txPin 10
SoftwareSerial SIM800(rxPin,txPin);

const String APN  = "ENTER_APN";
const String USER = "ENTER_USER_NAME";
const String PASS = "ENTER_PASSWORD";

const String FIREBASE_HOST  = "ENTER_FIREBASE_HOST_URL";
const String FIREBASE_SECRET  = "ENTER_FIREBASE_SECRET_KEY";


#define USE_SSL true
#define DELAY_MS 500

void setup() {
  
    Serial.begin(9600);
    nexInit();
   
      delay(1000);
      
   Serial.begin(115200); // for HX711
   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

   SIM800.begin(9600); // starting SIM800
   Serial.println("Initializing SIM800...");
   init_gsm();

}
}
void loop() {
  String data = get_loadcell();
  if (scale.is_ready()) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found.");
  }
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff); // Always add 3 full bytes
  delay(1000);

  request_url = THING_SPEAK_API_URL;
  request_url += "?key=" + THING_SPEAK_API_KEY;
  request_url += "&field1=";
  request_url += scale;

  
  if(!is_gprs_connected()){
    gprs_connect();
 }
 
 //Start HTTP connection
  SIM800.println("AT+HTTPINIT");
  waitResponse();
  delay(DELAY_MS);

   if(USE_SSL == true){
    SIM800.println("AT+HTTPSSL=1");
    waitResponse();
    delay(DELAY_MS);
  }

  SIM800.println("AT+HTTPPARA=\"URL\","+request_url);
  waitResponse();
  delay(DELAY_MS);

  SIM800.println("AT+HTTPACTION=0");
  for (uint32_t start = millis(); millis() - start < 20000;){
    while(SIM800.available() > 0){
      String response = SIM800.readString();
      Serial.println(response);
      if(response.indexOf("+HTTPACTION:") > 0){
        goto OutFor;
      }
    }
  }
  OutFor:
  delay(DELAY_MS);

  //Read the response
  SIM800.println("AT+HTTPREAD");
  waitResponse("OK");
  delay(DELAY_MS);

  //Stop HTTP connection
  SIM800.println("AT+HTTPTERM");
  waitResponse("OK",1000);
  delay(DELAY_MS);

  delay(5000);
}
void init_gsm()
{
  //Testing AT Command
  SIM800.println("AT");
  waitResponse();
  delay(DELAY_MS);
 
  //Checks if the SIM is ready
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
  //Turning ON full functionality
  SIM800.println("AT+CFUN=1");
  waitResponse();
  delay(DELAY_MS);

  SIM800.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);

  //Connect to the internet
void gprs_connect()
{
  // attach or detach from GPRS service 
  SIM800.println("AT+CGATT?");
  waitResponse("OK",2000);
  delay(DELAY_MS);
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);

  SIM800.println("AT+SAPBR=3,1,\"APN\","+APN);
  waitResponse();
  delay(DELAY_MS);
 //sets the user name settings for your sim card network provider.
  if(USER != ""){
    SIM800.println("AT+SAPBR=3,1,\"USER\","+USER);
    waitResponse();
    delay(DELAY_MS);

    //sets the password settings for your sim card network provider.
  if(PASS != ""){
    SIM800.println("AT+SAPBR=3,1,\"PASS\","+PASS);
    waitResponse();
    delay(DELAY_MS);
  }

   SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);

   SIM800.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);

  boolean is_gprs_connected()
{
  SIM800.println("AT+SAPBR=2,1");
  if(waitResponse("0.0.0.0") == 1) { return false; }

  return true;
}

* Function: gprs_disconnect()
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network

boolean gprs_disconnect()
{
  
  //Disconnect GPRS
  SIM800.println("AT+CGATT=0");
  waitResponse("OK",60000);
  //delay(DELAY_MS);
  
  //DISABLE GPRS
  //SIM800.println("AT+SAPBR=0,1");
  //waitResponse("OK",60000);
  //delay(DELAY_MS);
  return true;
}
  boolean waitResponse(String expected_answer, unsigned int timeout)
{
  uint8_t x=0, answer=0;
  String response;
  unsigned long previous;
    
  //Clean the input buffer
  while( SIM800.available() > 0) SIM800.read();
  
 
  previous = millis();
  do{
    //if data in UART INPUT BUFFER, reads it
    if(SIM800.available() != 0){
        char c = SIM800.read();
        response.concat(c);
        x++;
        //checks if the (response == expected_answer)
        if(response.indexOf(expected_answer) > 0){
            answer = 1;
        }
    }
  }while((answer == 0) && ((millis() - previous) < timeout));
  
  
  Serial.println(response);
  return answer;
}
