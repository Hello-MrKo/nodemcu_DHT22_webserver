// 결선  

// AM2302 (DHT22) 핀 연결:
// VCC → NodeMCU 3.3V
// GND → NodeMCU GND
// Data → NodeMCU D5 (GPIO14)

// OLED 디스플레이 (I2C) 핀 연결:
// VCC → NodeMCU 3.3V
// GND → NodeMCU GND
// SCL → NodeMCU D1 (GPIO5)
// SDA → NodeMCU D2 (GPIO4)




#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>

// WiFi 설정
const char* ssid = "farmersday";                                    // 공유기 주소 
const char* password = "88888888";                                  // 공유기 비밀번호              



// MQTT 설정
const char* mqtt_server = "192.168.0.103"; // Raspberry Pi의 IP 주소
const int mqtt_port = 1883;
const char* mqtt_user = "someone";     // MQTT 브로커 사용자 이름 (필요 시)
const char* mqtt_pass = "123456";     // MQTT 브로커 비밀번호 (필요 시)

// MQTT 클라이언트 설정
WiFiClient espClient;
PubSubClient client(espClient);

// OLED 설정
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT 설정
#define DHTPIN D5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQTT 재연결 함수
void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT 서버에 연결 중...");
    // MQTT 브로커에 연결 시도
    if (client.connect("NodeMCUClient", mqtt_user, mqtt_pass)) {
      Serial.println("연결 성공");
      // 연결 후 필요한 토픽 구독 가능
    } else {
      Serial.print("실패, rc=");
      Serial.print(client.state());
      Serial.println(" 5초 후 재시도");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED 초기화 실패"));
    for (;;);
  }
  display.clearDisplay();
  
  // WiFi 연결
  WiFi.begin(ssid, password);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 연결 성공!");
  
  // MQTT 브로커 설정
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // 센서 데이터 읽기
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // OLED에 IP 주소, 온도, 습도 표시
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Farmersday");
  display.print("IP: ");
  display.println(WiFi.localIP());
 
  display.print("Temp: ");
  display.print(temperature);
  display.println(" *C");
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");
  display.display();
  
  // MQTT로 데이터 전송
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("센서 데이터 읽기 실패!");
  } else {
    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    client.publish("home/sensors/temperature_humidity", payload.c_str());                                              //  Mqtt in 토픽 주소 (home/sensors/temperature_humidity)
    Serial.println("데이터 전송: " + payload);
  }
  
  delay(60000); // 1분 간격으로 전송
}
