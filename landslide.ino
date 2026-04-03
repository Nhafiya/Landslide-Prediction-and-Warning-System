#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3
#define D10 1

#include <WiFi.h>
#include "ThingSpeak.h"  // Include ThingSpeak library
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <math.h>

String chatId;

// WiFi Credentials
#define SECRET_SSID "Landslide"
#define SECRET_PASS "12345678"

// ThingSpeak Information
WiFiClient client;
#define  SECRET_CH_ID_WEATHER_STATION  2901130;   // Weatherstation related (Thingspeak Channel ID)

unsigned long weatherStationChannelNumber =2901130;
const char* readAPIKey = "AZT1X9ZQJ13GKOJC";  // Replace with your ThingSpeak Read API Key

int keyIndex = 0;            // your network key Index number (needed only for WEP)

#define ON 1
#define CONNECTED 1
#define OFF 2
#define DISCONNECTED 1
#define CONNECTING 3
#define BUZZER D6

#define BOT_TOKEN "7912679222:AAFfOknAt9bqMGJKI_PgexoHGDv6MoaF080"
X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
int statusCode = 0;

#define IS ==
#define CHANCE(d) slidingChance(d, moisture, displacement, force)
//int slidingChance(int Day, float moisture, float displacement, float force){

#define MOISTURE_SENSOR 35
#define FORCE_SENSOR 34
#define DISPLACEMENT_SENSOR 32

#define VERY_LOW_RISK 0
#define LOW_RISK 1
#define MODERATE_RISK 2
#define HIGH_RISK 3
#define VERY_HIGH_RISK 4

#define TODAY 0
#define TOMORROW 1
#define DAY_AFTER_TOMORROW 2
#define DAY3 3
#define THIRD_DAY 3


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~SETTINGS OR VALUES FOR CALCULATIONS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// Constants (Replace with actual values from studies)
/*const*/ float cohesion = 70.0; // Increased cohesion for more soil strength (kPa)
const float frictionAngle = 35.0; // Higher friction angle for improved shear strength (degrees)
const float soilUnitWeight = 18.0; // Same unit weight of soil in kN/m³
/*const*/ float slopeAngle = 70.0; // Reduced slope angle to represent normal conditions (degrees)
const float porePressureFactor = 0.3; // Lowered pore pressure effect (kPa/mm)
const float soilPermeability = 0.001; // Same soil permeability in m/s
const float depthFailureSurface = 3.0; // Increased depth of failure surface (meters)
const float criticalFoS = 1.0; // Same critical Factor of Safety threshold
const float area = 2.0; // Same unit area for force and displacement measurement

int rain[10];
int temp[10];
unsigned int field[8] = {1,2,3,4,5,6,7,8};

// Chat Settings (Telegram)
#define CHAT_ID "1371409037" //My Number 

#define CHAT_ID1 "" //Second Number if needed
#define CHAT_ID2 "" //Third  Number if needed
#define CHAT_ID3 "" //Fourth  Number if needed
#define CHAT_ID4 "" //Fifth Number if needed
#define CHAT_ID5 "" //Sixth  Number if needed

float tempToday;
void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}
/*************************************Moisture Sensor*****************************/

float getMoisture(){
  float moistureLevel = analogRead(MOISTURE_SENSOR);
  moistureLevel = (760 - moistureLevel)/760.0;
  return moistureLevel;
//   

  
}
/*==========================End of Moisture Sensor =============================*/
/************************Force Sensor************************************/
float getForceApplied(){
  float frc = analogRead(FORCE_SENSOR);
  frc = 0.02 *frc + 160.9;
    Serial.print("Force: ");  Serial.println(frc);
    return(frc);
}

/*====================End of Force Sensor ===============*/
/*********************Soil DisplacemenT*******************************/
float getSoilDisplacement(){
  float dsp= analogRead(DISPLACEMENT_SENSOR);
  dsp = 1.3 - 0.0286*pow(dsp,0.56);
    Serial.print("Displacement: ");     Serial.println(dsp);
    return(dsp);
  
}
/*===================End of soil displacement========================*/
//`https://api.thingspeak.com/update?api_key=${apiKey}&field1=${temperatureToday}&field2=${rainToday}&field3=${rain1 + temp1 / 100}&field4=${rain2 + temp2 / 100}&field5=${rain3 + temp3 / 100}`;


//Getting Weather Reprt from WeatherApp (Demo Application for Project Purpose)

void getWeather(){
  float d0 = ThingSpeak.getFieldAsFloat(field[1]); //45.32
  float d1 = ThingSpeak.getFieldAsFloat(field[2]);
  float d2 = ThingSpeak.getFieldAsFloat(field[3]);
  float d3 = ThingSpeak.getFieldAsFloat(field[4]);
    //Extract Rain and Temperature if  day2, Day3, Day4

  rain[0] = (int) d0;
  rain[1] = (int) d1;
  rain[2] = (int) d2;
  rain[3] = (int) d3;

  temp[0] =  d0*100; //4532
  temp[1] =  d1*100;
  temp[2] =  d2*100;
  temp[3] =  d3*100;

  temp[0]%=100;
  temp[1]%=100;
  temp[2]%=100;
  temp[3]%=100;

  
  Serial.print("Today Temp:"); Serial.print(temp[0]) ; Serial.print("|    Rain:"); Serial.println(rain[0]);
  Serial.print("Day 1 Temp:"); Serial.print(temp[1]) ; Serial.print("|    Rain:"); Serial.println(rain[1]);
  Serial.print("Day 2 Temp:"); Serial.print(temp[2]) ; Serial.print("|    Rain:"); Serial.println(rain[2]);
  Serial.print("Day 3 Temp:"); Serial.print(temp[3]) ; Serial.print("|    Rain:"); Serial.println(rain[3]);  
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                                    # FoS Calculation #
 @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

 float calculateFoS(float rain, float temp, float moisture, float force, float displacement) {
  
  float rainImpact = exp(rain/250);
  // Calculate effective normal stress (σ')
  float sigma_prime = soilUnitWeight * depthFailureSurface *(0.3+5* (1/tan(radians(slopeAngle+0.0001))))/rainImpact;
  //B$3*$B$7*(0.3+5*COT(RADIANS($B$4+0.0001))) /T12

  // Adjust soil cohesion based on temperature (assuming higher temperatures strengthen cohesion slightly)
  float c = cohesion*(pow(2.3,(-50*moisture))) + 0.1*exp(temp/30); // Adjust cohesion based on temperature effect
//=$B$1*(POWER(2.3,(-0.5*J13)))+0.1*EXP(G13/30)

  // Calculate shear strength (S) using Mohr-Coulomb equation: S = c + σ' * tan(ϕ)
  float shearStrength = c + sigma_prime*tan(radians(frictionAngle));
  //=B13+A13*TAN(RADIANS($B$2))

  // Calculate shear stress (τ) from applied force
  float displacementImpact = 0.8 + 0.8*exp(4*(displacement-1.15));
  float shearStress = 20.0*area* displacementImpact; // Force per unit area (shear stress)
 

  Serial.print("Rain Impact: ");
  Serial.print(rainImpact);
    Serial.print("  |  displacementImpact: ");
  Serial.print(displacementImpact);
  Serial.print("  |  SigmaPrime: ");
  Serial.print(sigma_prime);
  Serial.print("  |  C: ");
  Serial.print(c);
  Serial.print("  |  shearStrength: ");
  Serial.print(shearStrength);
  Serial.print("  |  shearStress: ");
  Serial.print(shearStress);

  Serial.print("Force: ");
  Serial.println(force);
  // Calculate Factor of Safety (FoS)

  return (shearStrength / shearStress);
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ End of Fos @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                          * Sliding Chace of a Day*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int slidingChance(int Day, float moisture, float displacement, float force){
  
//Calculate FoS
 float FoS = calculateFoS(rain[Day], temp[Day],moisture, force, displacement);
 Serial.print("FoS : "); Serial.println(FoS);
// Determine sliding chance based on FoS
      if (FoS > 1.5) {
        return VERY_LOW_RISK; // Very Low risk
      } else if (FoS > 1.3) {
        return LOW_RISK; // Low risk
      } else if (FoS > 1.0) {
        return MODERATE_RISK; // Moderate risk
      } else if (FoS > 0.8) {
        return HIGH_RISK; // High risk
      } else {
        return VERY_HIGH_RISK; // Very High risk (landslide imminent)
        //bot.sendMessage(CHAT_ID, "Hi.. This is Muhsin Project", "");
      }

  } 
/*~~~~~~~~~~~ End of SlidingChance~~~~~~~~~~~~*/

/***************************************************************************************************
                                      * WiFi Connection Indiacator *
 ***********************************************************************************************/
 void WiFiStatusIndicator(int md){
  switch(md){
    case ON         : digitalWrite(D7,HIGH); digitalWrite(D8,LOW); break;
    case OFF        : digitalWrite(D8,HIGH); digitalWrite(D7,LOW); break;
    case CONNECTING : digitalWrite(D8,HIGH); digitalWrite(D7,LOW); delay(100); 
                      digitalWrite(D7,LOW); digitalWrite(D8,LOW); delay(100);
                      digitalWrite(D7,HIGH); digitalWrite(D8,LOW); delay(100);
                      digitalWrite(D7,LOW); digitalWrite(D8,LOW); delay(100); break;

  }

}

/********************************************* End of Connection Indicator ***************************************/
/*===============================================================================================
                                @@# MAIN BODY/ LOOP #@@
================================================================================================*/

void loop() {
 //Check Wifi Connecttion and Connect if not connected
 // Connect to thinspeak
 statusCode = ThingSpeak.readMultipleFields(weatherStationChannelNumber);
 float moisture;
  float force;
  float displacement;
  if(statusCode == 200)
    {
 
      getWeather(); // getWeather report
      moisture = getMoisture(); // Get current soil moisture
      force = getForceApplied(); // Get applied force
      displacement = getSoilDisplacement(); // Get soil displacement
      WiFiStatusIndicator(CONNECTED);
    }
  else
  {
    Serial.println("Couldn't get data from Weather Ap");
    WiFiStatusIndicator(CONNECTING);
  }
 
 
 
// Predict the Land Sliding Possibilty based on FoS
// 1. Today
if(CHANCE(TODAY) IS VERY_HIGH_RISK)
  {

    bot.sendMessageWithInlineKeyboard(chatId, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID1, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID2, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID3, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID4, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID5, "Be Alert.! Today There is a heavy risk for Land sliding", "", "");
    digitalWrite(BUZZER,HIGH);
    //dealy(5000);
    //digitalWrite(BUZZER,LOW);
  }
  else if(CHANCE(TODAY) IS HIGH_RISK)
  {
    bot.sendMessageWithInlineKeyboard(chatId, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID1, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID2, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID3, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID4, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID5, "Be Alert.! Today There is a 70% risk for Land sliding", "", "");
    
    digitalWrite(BUZZER,HIGH);
    delay(1000);
    digitalWrite(BUZZER,LOW);

  }
  else if(CHANCE(TODAY) IS MODERATE_RISK)
  {
 bot.sendMessageWithInlineKeyboard(chatId, "Be Alert.! Today There is a moderate risk for Land sliding", "", "");
  }
// 2. Tomorrow

if(CHANCE(TOMORROW) IS VERY_HIGH_RISK)
 {
bot.sendMessageWithInlineKeyboard(chatId, "Be Alert.! TOMORROW There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID1, "Be Alert.!  There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID2, "Be Alert.!  There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID3, "Be Alert.!  There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID4, "Be Alert.!  There is a 70% risk for Land sliding", "", "");
    bot.sendMessageWithInlineKeyboard(CHAT_ID5, "Be Alert.! There is a 70% risk for Land sliding", "", "");
    
 }
 else if(CHANCE(TOMORROW) IS HIGH_RISK)
 {

 }
  else if(CHANCE(TOMORROW) IS MODERATE_RISK)
 {
  
 }
// 3. Day after tomorrow
  //bot.sendMessage(CHAT_ID, "Bot started up", "");





 
  delay(10000); // Fetch data every 60 seconds
}
