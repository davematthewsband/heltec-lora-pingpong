// Shared V3 button/light demo. Recovered PingPong baseline is preserved in archive/.
#include <Arduino.h>
#include "LoRaWan_APP.h"
#include "HT_SSD1306Wire.h"
#include <esp_sleep.h>
#include <driver/rtc_io.h>
// Unambiguous SDK path; Heltec also has a driver/gpio.h.
#include <include/driver/gpio.h>
#include "demo_protocol.h"

namespace {
constexpr uint8_t kButton = 0, kBatteryAdc = 1, kBatteryEnable = 37;
constexpr uint32_t kFrequency = 868000000; // Retained setting; confirm variant/antenna before upload.
constexpr int8_t kPower = 10;
SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
RadioEvents_t events;
demo::Node node(1, 1);
demo::Button button;
bool transmitting = false;
uint8_t tx[demo::kPacketSize];
uint32_t nextStatus = 0, nextBattery = 0, nextDisplay = 0, txStarted = 0;
uint32_t releasedAt = 0;
bool released = false;
int16_t peerRssi = 0;
uint32_t jitter(uint32_t base, uint32_t range) { return base + esp_random() % range; }
void listen() { transmitting = false; Radio.Rx(0); }
void onTxDone() { listen(); }
void onTxTimeout() { Serial.println("TX timeout"); Radio.Sleep(); listen(); }
void onRxError() { listen(); }
void onRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t) {
  demo::Packet p;
  if (demo::decode(payload, size, p)) {
    node.receive(p, millis());
    if (p.source == node.peer && (!p.target || p.target == node.id)) peerRssi = rssi;
  }
  listen();
}
String shortId(uint64_t id) {
  char out[7]; snprintf(out, sizeof(out), "%06lX", (unsigned long)(id & 0xffffff));
  return String(out);
}
String batteryText(uint16_t mv) {
  int percent = demo::batteryPercent(mv);
  if (percent < 0) return "--";
  return String(mv / 1000.0f, 2) + "V ~" + String(percent) + "%";
}
void readBattery() {
  // Original V3: P-channel switch, active LOW; 390k/100k divider.
  digitalWrite(kBatteryEnable, LOW);
  delay(10);
  uint32_t sum = 0;
  for (unsigned i = 0; i < 8; ++i) sum += analogReadMilliVolts(kBatteryAdc);
  digitalWrite(kBatteryEnable, HIGH);
  uint32_t mv = (sum * 49 + 40) / 80;
  node.battery = (mv >= 2500 && mv <= 4500) ? uint16_t(mv) : 0;
}
void draw(uint32_t now) {
  display.clear();
  display.drawString(0, 0, "Device " + shortId(node.id));
  display.drawString(0, 10, "Me: " + batteryText(node.battery));
  display.drawString(0, 20, node.peer ? "Peer: " + batteryText(node.peerBattery) : "Looking for friend...");
  String link = node.peer ? shortId(node.peer) + " " + String(peerRssi) + "dBm " +
      String(uint32_t(now - node.lastSeen) / 1000) + "s" : "Press other to wake";
  display.drawString(0, 30, link);
  display.drawString(0, 40, "LED me:" + String(node.light ? "ON" : "OFF") +
      " peer:" + (node.peer ? String(node.peerLight ? "ON" : "OFF") : "--"));
  String action = "Press: peer light";
  if (node.pending) action = node.command.type == demo::Sleep ? "Asking peer to sleep" : "Sending...";
  else if (node.sleeping) action = node.failed ? "No reply; sleeping" :
      (node.confirmed ? "Both sleeping..." : "Peer asked: sleeping");
  else if (node.failed) action = "No reply / no peer";
  else if (node.confirmed) action = "Peer light confirmed";
  display.drawString(0, 50, action);
  display.display();
}
void sleepNow() {
  Serial.println("Deep sleep: PRG button wakes this device");
  display.displayOff();
  Radio.Sleep();
  SPI.end();
  digitalWrite(LED, LOW);
  digitalWrite(kBatteryEnable, HIGH);
  digitalWrite(Vext, HIGH);
  // Hold non-RTC power controls across deep sleep to avoid floating gates.
  gpio_hold_en((gpio_num_t)LED);
  gpio_hold_en((gpio_num_t)Vext);
  gpio_hold_en((gpio_num_t)kBatteryEnable);
  gpio_deep_sleep_hold_en();
  rtc_gpio_pullup_en(GPIO_NUM_0);
  rtc_gpio_pulldown_dis(GPIO_NUM_0);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);
  Serial.flush();
  esp_deep_sleep_start();
}
} // namespace

void setup() {
  Serial.begin(115200);
  rtc_gpio_deinit(GPIO_NUM_0);
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis((gpio_num_t)LED);
  gpio_hold_dis((gpio_num_t)Vext);
  gpio_hold_dis((gpio_num_t)kBatteryEnable);
  pinMode(kButton, INPUT_PULLUP);
  pinMode(LED, OUTPUT); digitalWrite(LED, LOW);
  pinMode(kBatteryEnable, OUTPUT); digitalWrite(kBatteryEnable, HIGH);
  analogReadResolution(12);
  analogSetPinAttenuation(kBatteryAdc, ADC_11db);
  pinMode(Vext, OUTPUT); digitalWrite(Vext, LOW);
  delay(100);
  display.init(); display.setFont(ArialMT_Plain_10);
  node = demo::Node(ESP.getEfuseMac() & 0xffffffffffffULL, esp_random());
  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  events.TxDone = onTxDone; events.TxTimeout = onTxTimeout;
  events.RxDone = onRxDone; events.RxError = onRxError; events.RxTimeout = onRxError;
  Radio.Init(&events);
  Radio.SetChannel(kFrequency);
  Radio.SetTxConfig(MODEM_LORA, kPower, 0, 0, 7, 1, 8, false, true, 0, 0, false, 3000);
  Radio.SetRxConfig(MODEM_LORA, 0, 7, 1, 0, 8, 0, false, 0, true, 0, 0, false, true);
  listen();
  nextStatus = millis() + jitter(500, 2000);
  Serial.printf("Device %012llX session %08lX\n", node.id, (unsigned long)node.session);
}

void loop() {
  Radio.IrqProcess();
  uint32_t now = millis();
  bool down = digitalRead(kButton) == LOW;
  if (down) released = false;
  else if (!released) { released = true; releasedAt = now; }
  demo::Press press = button.update(down, now);
  if (press == demo::Short) node.request(demo::Toggle, now);
  if (press == demo::Long) node.request(demo::Sleep, now);
  node.tick(now);
  digitalWrite(LED, node.light && !node.sleeping ? HIGH : LOW);
  // A transport watchdog returns to RX even if a driver callback is missed.
  if (transmitting && uint32_t(now - txStarted) > 4000) onTxTimeout();
  if (!transmitting) {
    demo::Packet p;
    bool send = false;
    if (node.ackReady) { p = node.ack; node.ackReady = false; send = true; }
    else if (node.nextCommand(now, p)) send = true;
    else if (!node.pending && !node.sleeping && demo::due(now, nextStatus)) {
      p = node.packet(demo::Status); send = true;
    }
    if (send) {
      demo::encode(p, tx);
      Radio.Sleep(); transmitting = true; txStarted = now;
      Radio.Send(tx, sizeof(tx));
      nextStatus = now + jitter(15000, 5000);
      // Independent retry jitter prevents two simultaneous commands staying in lockstep.
      if (p.type == demo::Toggle || p.type == demo::Sleep) node.retryAt += jitter(0, 750);
    }
  }
  if (!transmitting && demo::due(now, nextBattery)) {
    readBattery(); nextBattery = millis() + 10000;
  }
  if (demo::due(now, nextDisplay)) { draw(now); nextDisplay = now + 200; }
  if (node.canSleep(now, released && uint32_t(now - releasedAt) >= 100, !transmitting)) sleepNow();
  delay(2);
}
