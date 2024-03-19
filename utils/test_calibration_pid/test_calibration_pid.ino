
int x = 0;
String str;

#define PID_D 0
#define PID_P 1
#define PID_I 2

int currentCalibrationType = PID_D;

// In format {min, max, multiplicator}
unsigned int serialToPIDMapper[3][3] = {
    {0, 500, 0.1}, // D
    {0, 500, 0.1},       // P
    {0, 500, 0.1}, // I
};

float pidValues[3] = {
    0.0, // D
    0.0,       // P
    0.0, // I
};

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);
}

// the loop routine runs over and over again forever:
void loop() {

  if (Serial.available() > 0) {
    x = Serial.parseInt();
    str = Serial.readStringUntil('\n');

    if (x < 0) {
      moveNextValueType();
    }else{
      setValue(currentCalibrationType, x);
    }
  }
}

void moveNextValueType() {
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

void setValue(int valueType, int intValueFromSerial){
  if(isInValueRange(valueType, intValueFromSerial)){

    int nextValue = map(intValueFromSerial, 1000, 2000, serialToPIDMapper[valueType][0], serialToPIDMapper[valueType][1]);

    pidValues[valueType] = nextValue * 0.1;

    logPids();

  }else{
    Serial.print("Out of range: ");
    Serial.println(intValueFromSerial);
  }
}

void logPids(){
  Serial.print("D: ");
  Serial.print(pidValues[0]);
  Serial.print(" | ");
  Serial.print("P: ");
  Serial.print(pidValues[1]);
  Serial.print(" | ");
  Serial.print("I: ");
  Serial.print(pidValues[2]);
  Serial.print(" | ");
  Serial.println();
}

bool isInValueRange(int valueType, int intValueFromSerial){
  return intValueFromSerial >= 1000 && intValueFromSerial <= 2000;
}
