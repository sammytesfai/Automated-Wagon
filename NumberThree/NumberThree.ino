#include <ArduinoBLE.h> 
#include <Wire.h> 

#define BLE_UUID_BUFFER                    "1A3AC132-31ED-758C-BC52-54A61958EF82"
#define BLE_UUID_STRING                    "F7A99D03-F2CF-4D55-BCF5-5C0BDFC9E055"

volatile int rssi1 = 0;
volatile int rssi2 = 0;
volatile int distance = 0;
int kalmanFilter(int measurement)
{
  const int measurementNoise = 1;
  const int processNoise = 1;

  static int estimate = 0;
  static int errorCovariance = 1000;

  // Prediction
  int predictedEstimate = estimate;
  int predictedErrorCovariance = errorCovariance + processNoise;

  // Update
  int innovation = measurement - predictedEstimate;
  int innovationCovariance = predictedErrorCovariance + measurementNoise;
  int kalmanGain = (predictedErrorCovariance << 10) / innovationCovariance; // Fixed-point division

  estimate = predictedEstimate + ((kalmanGain * innovation) >> 10); // Fixed-point multiplication and division
  errorCovariance = ((1024 - kalmanGain) * predictedErrorCovariance) >> 10; // Fixed-point multiplication and division

  return estimate;
}

// Creating BLE service with a UUID of 9A48ECBA... 
static const char* greeting = "Hello World!";
BLEService greetingService("9A48ECBA-2E92-082F-C079-9E75AAE428B1");  

BLEStringCharacteristic greetingCharacteristic("2A56",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, 13); 
BLEStringCharacteristic stringCharacteristic( BLE_UUID_STRING, BLERead | BLENotify,200 );

void receiveOne() {
  Wire.write(distance);
  Serial.print("DISTANCE: "); 
  Serial.print(distance); 
  Serial.print("\r\n");
}

// Defining the pins used for input and output 
void setup(void)
{

  Serial.begin(9600); 
  Wire.begin(0x9);
  Wire.onRequest(receiveOne); 
  
  // Set up the BLE module and advertise the greeting service. 
  if (!BLE.begin()) { 
   Serial.println("starting BLE failed!");
   while(1);
  }
  BLE.setLocalName("Nano33BLENumberThree");  
  BLE.setAdvertisedService(greetingService); 
  greetingService.addCharacteristic(greetingCharacteristic); 
  greetingService.addCharacteristic(stringCharacteristic);
  BLE.addService(greetingService); 
  greetingCharacteristic.setValue(greeting); 

  BLE.advertise();  // Start advertising
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");
}

void loop(void)
{
  
  BLEDevice central = BLE.central();  

  if (central) {
    Serial.print("Connected to central MAC: ");
    Serial.println(central.address());
    
    while(central.connected()){
      int rssi = central.rssi();  
      int txPower = -59;
      int n = 3; 
//      distance = (pow(10.0, (txPower - rssi) / (10.0 * n))) * 100 ;
      if (rssi != 0){
        distance = kalmanFilter(rssi);
      }
      Serial.print("DISTANCE: "); 
      Serial.print(distance); 
      Serial.print("\r\n");
      Serial.print("RSSI: "); 
      Serial.print(rssi); 
      Serial.print("\r\n");
      int rssi1 = 0;
       
      char bleBuffer[200];
      if (distance < -70){
        sprintf(bleBuffer, "OUT OF RANGE!");
      }
      else{
        sprintf(bleBuffer, "Within Range");
      }
      
//      Serial.print("  RSSI3: "); 
//      Serial.print(rssi);
//      Serial.print("\r\n");

      stringCharacteristic.writeValue(bleBuffer);
      memset(bleBuffer, 0, 200); 
      delay(250);
    }

    Serial.print("Disconnected from central MAC: ");
    Serial.println(central.address());
  }
}
