#include <OneWire.h>
#include <DallasTemperature.h>
#define D7 13
// Pin donde se conecta el bus 1-Wire
const int pinDatosDQ = D7;


// Definir el pin al que está conectado el FSR
const int fsrPin = 36; // Pin analógico A0

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



void setup() {
  Serial.begin(9600); // Iniciar la comunicación serial
  sensorDS18B20.begin();

}

void loop() {
  int fsrValue = analogRead(fsrPin); // Leer el valor del FSR (0-1100)
  
  // Imprimir el valor en el monitor serial
  Serial.print("Valor FSR: ");
  Serial.println(fsrValue);
  
  delay(1000); // Esperar medio segundo
 
  int analogValue = analogRead(TdsSensorPin);
  float voltage = analogValue * Vref / 1024.0; // Convert analog reading to voltage
  float tdsValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * TdsFactor; // Convert voltage to TDS value
  Serial.print("TDS Value: ");
  Serial.print(tdsValue);
  Serial.println(" ppm");
  readTdsAndEc();
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






// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);
 
void setup() {
    // Iniciamos la comunicación serie
    Serial.begin(9600);
    // Iniciamos el bus 1-Wire
    
}
 
void loop() {
    // Mandamos comandos para toma de temperatura a los sensores
    sensorDS18B20.requestTemperatures();
 
    // Leemos y mostramos los datos de los sensores DS18B20
    Serial.print("Temperatura sensor: ");
    Serial.print(sensorDS18B20.getTempCByIndex(0));
    Serial.println(" Cº");
 
   
    delay(500);
}
