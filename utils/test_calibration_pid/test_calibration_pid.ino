
#include <EEPROM.h>

int x = 0;
String str;

#define PID_D 0
#define PID_P 1
#define PID_I 2

int currentCalibrationType = PID_D;

const int address_var0 = 0;                  // Address to store variable 0
const int address_var1 = sizeof(float);      // Address to store variable 1
const int address_var2 = 2 * sizeof(float);  // Address to store variable 2

// In format {min, max, multiplicator}
unsigned int serialToPIDMapper[3][2] = {
  { 0, 500 },  // D
  { 0, 500 },  // P
  { 0, 500 },  // I
};

float pidValues[3] = {
  0.0,  // D
  0.0,  // P
  0.0,  // I
};

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);

  printSavedValues();
}

// the loop routine runs over and over again forever:
void loop() {

  if (Serial.available() > 0) {
    x = Serial.parseInt();
    str = Serial.readStringUntil('\n');

    if (x < 0) {
      moveNextValueType();
    } else {
      setValue(currentCalibrationType, x);
    }
  }
}

void moveNextValueType() {
  Serial.println("save value :");
  saveValue(currentCalibrationType, pidValues[currentCalibrationType]);


  Serial.print("new type :");

  switch (currentCalibrationType) {
    case PID_D:
      currentCalibrationType = PID_P;
      Serial.println("PID_P");
      break;
    case PID_P:
      currentCalibrationType = PID_I;
      Serial.println("PID_I");
      break;
    case PID_I:
      currentCalibrationType = PID_D;
      Serial.println("PID_D");
      break;
    // etc
    default: break;
  }
}

void setValue(int valueType, int intValueFromSerial) {
  if (isInValueRange(valueType, intValueFromSerial)) {

    int nextValue = map(intValueFromSerial, 1000, 2000, serialToPIDMapper[valueType][0], serialToPIDMapper[valueType][1]);

    pidValues[valueType] = nextValue * 0.1;

    logPids(pidValues);

  } else {
    Serial.print("Out of range: ");
    Serial.println(intValueFromSerial);
  }
}

void logPids(float _pidValues[]) {
  Serial.print("D: ");
  Serial.print(_pidValues[0]);
  Serial.print(" | ");
  Serial.print("P: ");
  Serial.print(_pidValues[1]);
  Serial.print(" | ");
  Serial.print("I: ");
  Serial.print(_pidValues[2]);
  Serial.print(" | ");
  Serial.println();
}

bool isInValueRange(int valueType, int intValueFromSerial) {
  return intValueFromSerial >= 1000 && intValueFromSerial <= 2000;
}

void saveValue(int variableIndex, float value) {
  int address;

  // Determine the address based on the variableIndex
  switch (variableIndex) {
    case 0:
      address = address_var0;
      break;
    case 1:
      address = address_var1;
      break;
    case 2:
      address = address_var2;
      break;
    default:
      Serial.println("Invalid variable index.");
      return;
  }

  // Write the float value to EEPROM
  EEPROM.put(address, value);
  Serial.print("Value ");
  Serial.print(value);
  Serial.print(" saved at address ");
  Serial.println(address);
}

float* readSavedValues() {
  static float values[3];  // Array to hold the read values

  // Read values from EEPROM
  EEPROM.get(address_var0, values[0]);
  EEPROM.get(address_var1, values[1]);
  EEPROM.get(address_var2, values[2]);

  // Check if values exist in EEPROM
  // If not, initialize the array with 0
  for (int i = 0; i < 3; i++) {
    if (isnan(values[i])) {  // Check if the value is NaN (which means it wasn't written to EEPROM)
      values[i] = 0.0;       // Initialize with 0
    }
  }

  // Return the array of values
  return values;
}

void printSavedValues() {
  Serial.print("Saved Values > ");
  float* savedValues = readSavedValues();  // Read saved values from EEPROM

  logPids(savedValues);
}
