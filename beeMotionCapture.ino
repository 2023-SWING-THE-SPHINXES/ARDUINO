#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

int inMotionPin = A6;             // 벌통 내부에 설치되는 모션 인식 센서
int outMotionPin = A5;            // 벌통 외부에 설치되는 모션 인식 센서

float humidity;                   // 벌통 내부 습도
float temperature;                // 벌통 내부 온도
int inMotionValue;                // 벌통 내부에서 감지된 움직임
int outMotionValue;               // 벌통 외부에서 감지된 움직임

// previousState와 currentState의 비교를 통해 꿀벌의 이동 방향을 결정함.
// direction: IN -> OUT 또는 OUT -> IN
int currentState;                 // 현재 상태 : IN(1) or OUT(0)
int previousState = -1;           // 이전 상태 : IN(1) or OUT(0), 초기값은 -1
String direction = "unknown";     // 이전 상태와 현재 상태의 비교로 결정된 이동 방향

bool updateDisplay;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  CARRIER_CASE = true;
  carrier.begin();
  Serial.println("temperature, humidity, direction");
}

void loop() {
  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();

  // 모션 인식 센서에서 읽어온 raw data -> 0 ~ 1023의 scale을 0 ~ 100의 scale로 변환
  int rawInMotionValue = analogRead(inMotionPin);
  int rawOutMotionValue = analogRead(outMotionPin);
  inMotionValue = map(rawInMotionValue, 0, 1023, 100, 0);
  outMotionValue = map(rawOutMotionValue, 0, 1023, 100, 0);

  // 현재 인식된 꿀벌의 위치
  if (inMotionValue > outMotionValue) {
    currentState = 1;         // IN
  } else if (inMotionValue < outMotionValue) {
    currentState = 0;         // OUT
  }

  // 방향 결정
  if (previousState == 1 and currentState == 0) {           // IN -> OUT
    direction = "IN -> OUT";
  } else if (previousState == 0 and currentState == 1) {    // OUT -> IN
    direction = "OUT -> IN";
  } else {
    direction = "UNKNOWN";
  }

  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(currentState);
  Serial.print(",");
  Serial.println(direction);
  onUpdateDisplayChange();

  previousState = currentState;
  delay(2000);    // 센서 딜레이 조절하면 수정하기
}


void onUpdateDisplayChange() {
  // Do something
  carrier.display.fillScreen(ST77XX_WHITE);
  carrier.display.setTextColor(ST77XX_RED);
  carrier.display.setTextSize(2);

  carrier.display.setCursor(20, 30);
  carrier.display.print("Temp: ");
  carrier.display.print(temperature);
  carrier.display.print(" C");

  carrier.display.setCursor(20, 50);
  carrier.display.print("Humi: ");
  carrier.display.print(humidity);
  carrier.display.print(" %");

  carrier.display.setTextColor(ST77XX_BLUE);
  carrier.display.setCursor(20, 70);
  carrier.display.print("Dir: ");
  carrier.display.print(direction);

  updateDisplay = false;
}
