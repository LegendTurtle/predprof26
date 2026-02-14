#include <esp_now.h>
#include <WiFi.h>
#define BTN_PIN 1
uint8_t cubeMac[] = { 0xDC, 0xB4, 0xD9, 0x0A, 0x07, 0x4C };
long lastPressTime = 0;
typedef struct __attribute__((packed)) {
  int32_t id;
  int32_t hor;
  int32_t ver;
  int32_t mode;
}
esp_packet_t;
esp_packet_t packet;
esp_packet_t incoming;
bool lastButtonState = false;
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(esp_packet_t)) return;
  memcpy(&incoming, data, sizeof(incoming));
  Serial.print("ID: ");
  Serial.print(incoming.id);
  Serial.print("\nУгол по горизонтали: ");
  Serial.print(incoming.hor);
  Serial.print("\nУгол по вертикали: ");
  Serial.print(incoming.ver);
  Serial.print("\nРежим: ");
  Serial.println(incoming.mode);
  delay(500);
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
}
void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, cubeMac, 6);
  esp_now_add_peer(&peerInfo);
}
void loop() {
  bool buttonState = !digitalRead(BTN_PIN);
  if (buttonState and lastButtonState and millis() - lastPressTime > 50) {
    lastPressTime = millis();
    packet.id = 1;
    packet.hor = 0;
    packet.ver = 0;
    packet.mode = 1;
    Serial.println("START");
    esp_now_send(cubeMac, (uint8_t *)&packet, sizeof(packet));
  }
  lastButtonState = buttonState;
}