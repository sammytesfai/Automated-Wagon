#include <ArduinoBLE.h> 
#include <Wire.h> 

#define BLE_UUID_BUFFER                    "1A3AC132-31ED-758C-BC52-54A61958EF82"
#define BLE_UUID_STRING                    "F7A99D03-F2CF-4D55-BCF5-5C0BDFC9E055"

volatile int rssi1 = 0;
volatile int rssi2 = 0;
volatile int distance = 0;
volatile int output = 0;

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

BLEStringCharacteristic greetingCharacteristic("2A56", BLERead | BLENotify, 13); 
BLEStringCharacteristic stringCharacteristic( BLE_UUID_STRING, BLERead | BLENotify,200 );
bool conn = false;
void receiveOne() 
{
  //Serial.println(output);
  if(conn)
    Wire.write(output);
  else
  {
    Wire.write(200);
  }
}

bool SOS = false;
void receiveEvent(int bytes)
{
  while(Wire.available() > bytes)
  {
    SOS = Wire.read();
  }
  stringCharacteristic.writeValue("SOS");
}

// Defining the pins used for input and output 
void setup(void)
{

  Serial.begin(9600); 
  Wire.begin(0x9);
  Wire.onRequest(receiveOne); 
  Wire.onReceive(receiveEvent);
  
  // Set up the BLE module and advertise the greeting service. 
  if (!BLE.begin()) { 
   Serial.println("starting BLE failed!");
   while(1);
  }
  BLE.setLocalName("Nano33");
  BLE.setDeviceName("Nano33");  
  BLE.setAdvertisedService(greetingService); 
  greetingService.addCharacteristic(greetingCharacteristic);
  greetingService.addCharacteristic(stringCharacteristic); 
  BLE.addService(greetingService); 
  stringCharacteristic.writeValue("Running"); 

  BLE.advertise();  // Start advertising
}

volatile int grab(int a[]){
  int count = 0;
  //Serial.print("(");
  for (int i = 0; i < 10; i++) {
    //Serial.print(" ");
    //Serial.print(a[i]);
    //Serial.print(",");
    if(a[i] == true){
      count = count + 1;
    } 
  }
//  Serial.print(")");
  
  if(count>=9){
    output = true;
//    Serial.print(" OUT");
  } else {
    output = false;
//    Serial.print(" IN");
  }
  
  //Serial.print("\r\n");
  return output;
}


void loop(void)
{
  int filtered_rssi;
  int raw_rssi;
  //int txpower = -59;
  //int n = 3;
  int thresh = -70;
  bool B;
  int arr[10] = {};
  
  BLEDevice central = BLE.central();  

  if (central) {
    Serial.print("Connected to central MAC: ");
    conn = true;
    Serial.println(central.address());
    
  while(central.connected()){
      raw_rssi  = central.rssi();  
     
      if (raw_rssi != 0){
        filtered_rssi = kalmanFilter(raw_rssi);
        //distance = (pow(10.0, (txPower - kalmanFilter(rssi)) / (10.0 * n))) * 100 ;   
  
        int rssi1 = 0;
         
        ///char bleBuffer[200];
        //Serial.print(filtered_rssi);
        if (filtered_rssi <= thresh){
          B = true;
          //Serial.print("  OUT OF RANGE!");
          //Serial.print("\r\n"); 
        }
        else{
          B = false;
          //Serial.print("  Within Range");
          //Serial.print("\r\n"); 
        }
  
        int curr;
        curr = B;
        arr[9] = arr[8];
        arr[8] = arr[7];
        arr[7] = arr[6];
        arr[6] = arr[5];
        arr[5] = arr[4];
        arr[4] = arr[3];
        arr[3] = arr[2];
        arr[2] = arr[1];
        arr[1] = arr[0];
        arr[0] = curr;
        output = grab(arr);
      }  
      delay(100);
    }

    Serial.print("Disconnected from central MAC: ");
    Serial.println(central.address());
  }
}