//#include <FS.h>
//#include <ArduinoJson.h>

// Function Definitions

void handleNotFound();
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette);
void effects();
void showTime(int hr, int mn, int sec);


struct strConfig {
  String seconds; // use strtol(colorString.c_str(), NULL, 16); to convert from Hex string to color
  String minutes;
  String hours;
  String bg;
  String lines;
  int light_low;
  int light_high;
  int rain;
  byte gCurrentPaletteNumber;
	String ntpServerName;
	long Update_Time_Via_NTP_Every;
	long timezoneoffset;
  boolean autoTimezone;
	boolean daylight;
  byte switch_off;
  byte switch_on;
  String MQTTServer;
  boolean MQTT;
  String IPGeoKey;
}   config;


bool  saveConfig ( )  { 
  SPIFFS.begin();
  const size_t capacity = JSON_OBJECT_SIZE(12);
  DynamicJsonDocument doc(capacity);

  doc["seconds"] = config.seconds;
  doc["minutes"] = config.minutes;
  doc["hours"] = config.hours;
  doc["bg"] = config.bg;
  doc["lines"] = config.lines;
  doc["light_low"] = config.light_low;
  doc["light_high"] = config.light_high;
  doc["rain"] = config.rain;
  doc["gCurrentPaletteNumber"] = config.gCurrentPaletteNumber;
  doc["ntpServerName"] = config.ntpServerName;
  doc["Update_Time_Via_NTP_Every"] = config.Update_Time_Via_NTP_Every;
  doc["timezoneoffset"] = config.timezoneoffset;
  doc["autoTimezone"] = config.autoTimezone;
  doc["switch_off"] = config.switch_off;
  doc["switch_on"] = config.switch_on;
  doc["MQTTServer"] = config.MQTTServer;
  doc["MQTT"] = config.MQTT;
  doc["IPGeoKey"] = config.IPGeoKey;

  File configFile  =  SPIFFS.open("config.json","w"); 
  if  ( !configFile )  { 
    Serial.println( "Failed to open config file for writing" ); 
    return  false ; 
  }
  serializeJsonPretty(doc, configFile);
  SPIFFS.end();
  return  true ; 
}

bool  loadConfig() { 
  SPIFFS.begin();
  if (!SPIFFS.exists("config.json")) return false;
  File  configFile  =  SPIFFS.open("config.json","r"); 
  if(!configFile)  { 
    Serial.println( "Failed to open config file" ); 
    return  false ; 
  }

  size_t size  =  configFile.size(); 
  if(size > 1024 )  { 
    Serial.println( "Config file size is too large" ); 
    return  false ; 
  }

  // Allocate a buffer to store contents of the file. 
  std::unique_ptr<char[]>  buf(new char[size]) ;

  // We don't use String here because ArduinoJson library requires the input 
  // buffer to be mutable. If you don't use ArduinoJson, you may as well 
  // use configFile.readString instead. 
  configFile.readBytes( buf.get(), size ) ;
  message+= buf.get();
  const size_t capacity = JSON_OBJECT_SIZE(12) + 170;
  DynamicJsonDocument doc(capacity);
  //const char* json = "{\"seconds\":\"\",\"minutes\":\"\",\"hours\":\"\",\"bg\":\"\",\"light_low\":0,\"light_high\":100,\"rain\":30,\"gCurrentPaletteNumber\":1,\"ntpServerName\":\"\",\"Update_Time_Via_NTP_Every\":3600,\"timezoneoffset\":3600,\"autoTimezone\":1}";
  
  if  ( !deserializeJson(doc, buf.get()))  { 
    Serial.println( "Failed to parse config file" ) ; 
    return  false; 
  }

  config.seconds = doc["seconds"].as<String>(); 
  config.minutes = doc["minutes"].as<String>(); 
  config.hours = doc["hours"].as<String>(); 
  config.bg = doc["bg"].as<String>(); 
  config.lines = doc["lines"].as<String>();
  config.light_low = doc["light_low"]; 
  config.light_high = doc["light_high"]; 
  config.rain = doc["rain"]; 
  config.gCurrentPaletteNumber = doc["gCurrentPaletteNumber"]; 
  config.ntpServerName = doc["ntpServerName"].as<String>(); 
  config.Update_Time_Via_NTP_Every = doc["Update_Time_Via_NTP_Every"]; 
  config.timezoneoffset = doc["timezoneoffset"]; 
  config.autoTimezone = doc["autoTimezone"];
  config.switch_off = doc["switch_off"];
  config.switch_on = doc["switch_on"];
  config.MQTTServer = doc["MQTTServer"].as<String>();
  config.MQTT = doc["MQTT"];
  config.IPGeoKey = doc["IPGeoKey"].as<String>();

  seconds = strtol(config.seconds.c_str(), NULL, 16);
  minutes = strtol(config.minutes.c_str(), NULL, 16);
  hours = strtol(config.hours.c_str(), NULL, 16);
  lines = strtol(config.lines.c_str(), NULL, 16);
  bg = strtol(config.bg.c_str(), NULL, 16);
  IPGeolocation IPG(config.IPGeoKey);
  //if (config.MQTT) server.fromString(config.MQTTServer);

  if(config.autoTimezone){
        IPGeolocation IPG(config.IPGeoKey);
        IPG.updateStatus();
        config.timezoneoffset = IPG.getOffset();
        timeClient.setTimeOffset(config.timezoneoffset*3600);
        //timeClient.setPoolServerName(config.ntpServerName);
        timeClient.setUpdateInterval(config.Update_Time_Via_NTP_Every);
        timeClient.forceUpdate();
      }

  return  true ; 
}

void resetSettings(){
  SPIFFS.remove("/config.json");
  delay(3000);
  ESP.reset();
}