#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ThingSpeak.h>

const char *ssid = "vodafone9750";
const char *password = "FHHWW2FU54DKJM";


unsigned long channnelID = 2884708;
const char* WriteAPIKey = "30KJQI9S7F9IZSDW";


// Pin donde se conecta el bus 1-Wire
const int pinDatosDQ = 4;
const int fsrPin = 36; // pin conectado el FSR Pin analógico A0 presion
const int TdsSensorPin = 34; // Analog input pin that the sensor is attached to
const float Vref = 5.0;      // Analog reference voltage(Volt) of the ADC
const float TdsFactor = 0.5; // TDS factor


//Tech Trends
// Pin configuration
const byte TDS_SENSOR_PIN = 34;

// Calibration and offset values
const float AREF = 3.3;          // Vref for ESP32
const float EC_CALIBRATION = 1; // Sensor calibration factor
const float OFFSET = 0.14;       // Offset for dry sensor reading

// Variables for TDS and EC
float tdsValue = 0;
float ecValue = 0;
float temperature = 0;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

WiFiClient  client;
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);

void setup() {
  Serial.begin(9600); // Iniciar la comunicación serial
  sensorDS18B20.begin();

  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  
  conexionWifi();
}

void loop() {
  float fsrValue = analogRead(fsrPin); // Leer el valor del FSR (0-1100)
 
  // Mandamos comandos para toma de temperatura a los sensores
  sensorDS18B20.requestTemperatures();

  int analogValue = analogRead(TdsSensorPin);
  float voltage = analogValue * Vref / 1024.0; // Convert analog reading to voltage
  float tdsValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * TdsFactor; // Convert voltage to TDS value
  temperature = sensorDS18B20.getTempCByIndex(0);
  
  Serial.print("TDS Value: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");
  Serial.print("Valor FSR: ");
  Serial.println(fsrValue);
  
  // Leemos y mostramos los datos de los sensores DS18B20
  Serial.print("Temperatura sensor: ");
  Serial.print(sensorDS18B20.getTempCByIndex(0));
  Serial.println(" Cº");
  //readTdsAndEc();

  envioThingSpeak(2,tdsValue);

  delay(10000);
  envioThingSpeak(1, fsrValue);
  delay(10000);
  //envioThingSpeak(3, );
  envioThingSpeak(4, temperature);

  delay(10000); // Esperar medio segundo
}

void readTdsAndEc() {
    // Read analog value and calculate voltage
    float rawEc = analogRead(TDS_SENSOR_PIN) * AREF / 1024.0;

    // Apply calibration and offset
    ecValue = (rawEc * EC_CALIBRATION) - OFFSET;
    if (ecValue < 0) ecValue = 0;

    // Convert EC to TDS using a cubic equation
    tdsValue = (133.42 * pow(ecValue, 3) - 255.86 * ecValue * ecValue + 857.39 * ecValue) * 0.5;

    


    // Debug output
    Serial.print("TDS: ");
    Serial.println(tdsValue);
    Serial.print("EC: ");
    Serial.println(ecValue, 2);

}

int conexionWifi(){
   // We start by connecting to a WiFi network
  // To debug, please enable Core Debug Level to Verbose

  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  // Auto reconnect is set true as default
  // To set auto connect off, use the following function
  //    WiFi.setAutoReconnect(false);

  // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;

  // Wait for the WiFi event
  while (true) {

    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL: Serial.println("[WiFi] SSID not found"); break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return 0;
        break;
      case WL_CONNECTION_LOST: Serial.println("[WiFi] Connection was lost"); break;
      case WL_SCAN_COMPLETED:  Serial.println("[WiFi] Scan is completed"); break;
      case WL_DISCONNECTED:    Serial.println("[WiFi] WiFi is disconnected"); break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return 1;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.print("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      return 0;
    } else {
      numberOfTries--;
    }
  }
}

void envioThingSpeak(int parameterNumber, float dataParameter){
  
    Serial.print("Enviando datos: ");
    Serial.print(parameterNumber);
    Serial.print(" ");
    Serial.print(dataParameter);
    Serial.println("");
    int x = ThingSpeak.writeField(channnelID, parameterNumber, dataParameter , WriteAPIKey);
    //uncomment if you want to get temperature in Fahrenheit
    //int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  
}
