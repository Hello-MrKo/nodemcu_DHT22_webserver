
// 결선 방법 -------------------------------------------------------------
// DHT22 온습도 센서
// VCC -> 3.3V (NodeMCU)
// GND -> GND (NodeMCU)
// Data -> D5 (GPIO14)

// OLED 디스플레이 (0.96인치 I2C OLED 디스플레이 (128x64))
// VCC -> 3.3V (NodeMCU)
// GND -> GND (NodeMCU)
// SCL -> D1 (GPIO5)
// SDA -> D2 (GPIO4)


// 라이브러리 설치  -----------------------------------------------------
// Adafruit GFX Library (Adafruit GFX Library)
// Adafruit SSD1306 Library (Adafruit SSD1306)
// DHT sensor library
// PubSubClien
//------------------------------------------------------------------------

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN D5         // DHT 센서 데이터 핀 연결 (GPIO14)
#define DHTTYPE DHT22     // DHT11 센서 사용
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi 설정
const char* ssid = "farmersday";          // Wi-Fi SSID
const char* password = "a88888888";  // Wi-Fi 비밀번호

// 웹 서버 초기화
ESP8266WebServer server(80);

// 온도 및 습도 값을 저장할 변수
float temperature = 0.0;
float humidity = 0.0;

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


/*
void handleRoot() {
  // 웹 페이지에 표시될 HTML 콘텐츠
  String html = "<html><head><meta http-equiv='refresh' content='5'/>";
  html += "<title>Temperature & Humidity</title></head><body>";
  html += "<h1>Temperature and Humidity</h1>";
  html += "<p>Temperature: " + String(temperature) + " &#8451;</p>";
  html += "<p>Humidity: " + String(humidity) + " %</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
*/


void handle_OnConnect() {

 temperature = dht.readTemperature(); // Gets the values of the temperature
 humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(temperature,humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperaturestat,float humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";

  
  ptr +="<h1>ESP8266 NodeMCU</h1>\n";
  
  ptr +="<p id='temp'>Temperature: ";
  ptr +=(float)temperature;
  ptr +=" Cel</p>";
  
  
  ptr +="<p id='humi'>Humidity: ";
  ptr +=(float)humidity;
  ptr +=" %</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}





void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);

  // DHT 센서 초기화
  dht.begin();

  // OLED 디스플레이 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // 진행하지 않고 멈춤
  }

  // Wi-Fi 연결 설정
  setup_wifi();

  // 웹 서버 핸들러 설정
  //server.on("/", handleRoot);
  server.on("/", handle_OnConnect);

  server.begin();
  Serial.println("HTTP server started");

  // OLED 디스플레이 초기화 메시지
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Initializing..."));
  display.display();
  delay(2000);
}

void loop() {
  // 웹 서버 요청 처리
  server.handleClient();

  // DHT 센서에서 온도 및 습도 값 읽기
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // 유효한 값을 읽었는지 확인
  if (isnan(humidity) || isnan(temperature)) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("Failed to read from DHT sensor!"));
    display.display();
    return;
  }

  // OLED 디스플레이에 온도 및 습도 값 표시
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Farmersday"));
  display.print(F("  "));
  display.println(WiFi.localIP());
  display.println(F("  "));
  display.print(F("Temp: "));
  display.print(temperature);
  display.println(F(" C"));
  display.print(F("Humidity: "));
  display.print(humidity);
  display.println(F(" %"));
  
  display.display();

  // 2초마다 새로고침
  delay(2000);
}
