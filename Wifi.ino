#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "NodeMCU";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1); //each router has gateway, make devoce connect to router
IPAddress subnet(255,255,255,0); //masking (up to 255 decice can connect to the router)

ESP8266WebServer server(80);  //default port number

uint8_t LED1pin = D0;
bool LED1status = LOW; //ACTIVE LOW



String temp="";
String day="";
String Final_Date="";
String Final_Time="";





void setup() {
  Serial.begin(9600);  //baudrate
  pinMode(LED1pin, OUTPUT);
  
  WiFi.softAP(ssid, password); 
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/DATEANDTIME", handle_DATE_TIME);
  
  server.onNotFound(handle_NotFound);
  
  server.begin();
  //Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}

  
}

void handle_OnConnect() {
  LED1status = LOW;

  server.send(200, "text/html", SendHTML(LED1status)); 
}

void handle_led1on() {
  Serial.print('O');
  LED1status = LOW;//ACTIVE LOW
  //Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true)); 
  
}

void handle_led1off() {
  Serial.print('F');
  LED1status = HIGH;//ACTIVE HIGH
  //Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false));
  
}


  void handle_DATE_TIME() {
  //Serial.println("GPIO6 Status: OFF");
    Serial.print('D');
    delay(1000);
    while(Serial.available()>0)
    {
    temp+= char(Serial.read());
    }

   if(temp[5]=='1')
   {day=="Sunday";}
   else if(temp[5]=='2')
   {day=="Monday";}
   else if(temp[5]=='3')
   {day="Tuesday";}
      else if(temp[5]=='4')
   {day=="Wednesday";}
   else if(temp[5]=='5')
   {day="Thursday";}
      else if(temp[5]=='6')
   {day=="Friday";}
   else 
   {day="Saturday";}

   Final_Date="Day="+day;
   for (int k =6 ; k < 16; k++)
   {Final_Date
   +=temp[k];
   }

   
   for (int j =16 ; j < 30; j++)
   {Final_Time+=temp[j];
   }


   


  server.send(200, "text/html", SendHTML(!LED1status)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat){

  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica;background-color:#33ccff; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #ffffff;margin: 50px auto 30px;} h3 {color: #ffffff;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 120px;background-color: #1a1aff;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +="p {font-size: 16px;color: #0000cc;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  


  
   if(led1stat)
  {ptr +="<p>LED Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  ptr +="<a class=\"button button-off\" href=\"/DATEANDTIME\">DATE&Time</a>\n"; 

    String date = "<h3>" +Final_Date+ "</h3>\n";
    ptr +=date;
    
    String Time = "<h3>" +Final_Time+ "</h3>\n";
    ptr +=Time;
    
    temp="";
    day="";
    Final_Time="";
    Final_Date="";
  
    


    
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
