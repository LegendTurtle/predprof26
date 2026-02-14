#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

#define MODE_START 1
#define MODE_DATA 2
#define LASER_PIN 4

Servo hor;
Servo ver;

const int h0 = 81;
const int v0 = 65;

int targets[36][2] = { { -2, -46 }, { -1, -33 }, { 0, -22 }, { 0, -10 }, { 0, 0 }, { 0, 12 }, { 0, 25 }, { 0, 36 }, { 0, 48 }, { -45, -1 }, { -33, -1 }, { -21, 0 }, { -10, 0 }, { 0, 0 }, { 1, 12 }, { 25, 5 }, { 36, 5 }, { 50, 6 }, { -47, -40 }, { -34, -30 }, { -22, -22 }, { -11, -10 }, { 0, 0 }, { 11, 13 }, { 24, 27 }, { 35, 35 }, { 48, 46 }, { -43, 37 }, { -38, 30 }, { -21, 22 }, { -11, 18 }, { 8, 0 }, { 11, -10 }, { 25, -21 }, { 38, -28 }, { 50, -33 } };

uint8_t peerMac[] = { 0x80, 0xB5, 0x4E, 0xC7, 0x3E, 0x08 };

typedef struct __attribute__((packed)) {
  int32_t id;
  int32_t hor;
  int32_t ver;
  int32_t mode;
} esp_packet_t;

esp_packet_t outgoing;
esp_packet_t incoming;

bool sendBurst = false;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(esp_packet_t)) return;

  memcpy(&incoming, data, sizeof(incoming));

  if (incoming.mode == MODE_START) {
    Serial.println("START RECEIVED");
    sendBurst = true;
  }
}

void setup() {
  for (int i = 4; i < 6; i++) {
    pinMode(1, OUTPUT);
  }

  hor.attach(5);
  ver.attach(6);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  esp_now_add_peer(&peerInfo);
}
void loop() {
  hor.write(h0);
  ver.write(v0);
  digitalWrite(LASER_PIN, HIGH);
  if (sendBurst) {
    digitalWrite(LASER_PIN, HIGH);
    for (auto x : targets) {
      outgoing.id = 2;
      outgoing.hor = x[0];
      outgoing.ver = x[1];
      outgoing.mode = MODE_DATA;
      hor.write(h0 + x[0]);
      ver.write(v0 + x[1]);
      esp_now_send(peerMac, (uint8_t *)&outgoing, sizeof(outgoing));
      delay(3000);
      digitalWrite(LASER_PIN, LOW);
      sendBurst = false;
    }
  }
}