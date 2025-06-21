/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP32 chip.

  Note: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include<Stepper.h>
#include"HX711.h"
#include<EEPROM.h>
#include "esp_camera.h"
#include "camera_pins.h"
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define DT 12
#define SCK 13
#define IN1 0
#define IN2 2
#define IN3 14
#define IN4 15
#define relaypin 16
#define EEPROM_SIZE 6

BlynkTimer timer;
WidgetRTC rtc;
WidgetTerminal terminal(V0);
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "UBEgszMClbBZb-kCJ0N_5Judz3z_xsFg";

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "JTKWIFI";
//char pass[] = "RuangJTK-2021";

char ssid[] = "HUAWEI-tteZ";
char pass[] = "wardaddy";

//========================= variabel ===========================
int mass = 0, beratblynk;
boolean jalan = 0, jalansblm = 0;
byte pembagian;
unsigned long told, t;
byte jamskr,   mntskr,
     jampagi,  mntpagi,
     jamsiang, mntsiang,
     jammalam, mntmalam;
//const int stepsPerRevolution = 2038;
const int stepsPerRevolution = 4095;
String local_IP="";

HX711 scale;
Stepper myStepper = Stepper(stepsPerRevolution, IN1, IN2, IN3, IN4);
//=============================================================
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}
//==========================UPDATE RTC=========================
void jamnow() {
  jamskr = hour();
  mntskr = minute();
  Serial.print("jam sekarang =  ");
  Serial.print(jamskr); 
  Serial.print(":");
  Serial.println(mntskr);
  terminal.print("jam sekarang =  ");
  terminal.print(jamskr); Serial.print(":");
  terminal.println(mntskr);
  terminal.flush();
}
//==========================motordc============================
void motordc(boolean kondisi) {
  digitalWrite(relaypin, kondisi);
}
void inisialisasi_kamera();
void takephoto();
void startCameraServer();
//=============================SETUP===========================
void setup()
{
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  inisialisasi_kamera();
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.println("START");
  Blynk.begin(auth, ssid, pass);
  setSyncInterval(60 * 10);
  timer.setInterval(10000L, jamnow);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH);
  scale.begin(DT, SCK);
  scale.set_scale(397.50);
  scale.tare();
  terminal.clear();
  terminal.println("Alat sudah konek ke blynk");
  terminal.flush();
  digitalWrite(4, LOW);
  jampagi =EEPROM.read(0);
  mntpagi =EEPROM.read(1);
  jamsiang=EEPROM.read(2);
  mntsiang=EEPROM.read(3);
  jammalam=EEPROM.read(4);
  mntmalam=EEPROM.read(5);
}


//==================set jam==========================
BLYNK_WRITE(V3) {
  TimeInputParam  tpagi(param);
  jampagi = tpagi.getStartHour();
  mntpagi = tpagi.getStartMinute();
  Serial.print("jadwal pagi =  ");
  Serial.print(jampagi); Serial.print(":");
  Serial.println(mntpagi);
  terminal.print("jadwal pagi =  ");
  terminal.print(jampagi); Serial.print(":");
  terminal.println(mntpagi);
  terminal.flush();
  EEPROM.write(0, jampagi);
  EEPROM.write(1, mntpagi);
  EEPROM.commit();
}
BLYNK_WRITE(V4) {
  TimeInputParam  tsiang(param);
  jamsiang = tsiang.getStartHour();
  mntsiang = tsiang.getStartMinute();
  Serial.print("jadwal siang =  ");
  Serial.print(jamsiang); Serial.print(":");
  Serial.println(mntsiang);
  terminal.print("jadwal siang =  ");
  terminal.print(jamsiang); Serial.print(":");
  terminal.println(mntsiang);
  terminal.flush();
  EEPROM.write(2, jamsiang);
  EEPROM.write(3, mntsiang);
  EEPROM.commit();
}
BLYNK_WRITE(V5) {
  TimeInputParam tmalam(param);
  jammalam = tmalam.getStartHour();
  mntmalam = tmalam.getStartMinute();
  Serial.print("jadwal malam =  ");
  Serial.print(jammalam); Serial.print(":");
  Serial.println(mntmalam);
  terminal.print("jadwal malam =  ");
  terminal.print(jammalam); Serial.print(":");
  terminal.println(mntmalam);
  terminal.flush();
  EEPROM.write(4, jammalam);
  EEPROM.write(5, mntmalam);
  EEPROM.commit();
}
//==================TERMINAL========================
BLYNK_WRITE(V0)
{
  if (String("jalansblm=1") == param.asStr()) {
    jalansblm = 1;
    terminal.write(param.getBuffer(), param.getLength());
  }
  else if (String("jalansblm=0") == param.asStr()) {
    jalansblm = 0;
    terminal.write(param.getBuffer(), param.getLength());
  }
  else if (String("jalan=1") == param.asStr()) {
    jalan = 1;
    terminal.write(param.getBuffer(), param.getLength());
  }
  else if (String("jalan=0") == param.asStr()) {
    jalansblm = 0;
    terminal.write(param.getBuffer(), param.getLength());
  }
  else if (String("motor=1") == param.asStr()) {
    terminal.println("motor dc dijalankan manual");
  }
  else if (String("motor=0") == param.asStr()) {
    terminal.println("motor dc berhenti");
  }
  terminal.flush();
}
//=================INPUT BERAT PAKAN=================
BLYNK_WRITE(V1) {
  beratblynk = param.asInt();
  if (beratblynk % 500 <= 100) { //jika kelipatan 500
    pembagian = (beratblynk - (beratblynk % 500)) / 500;
  } else if (beratblynk % 500 <= 500) {
    pembagian = ((beratblynk - (beratblynk % 500)) + 500) / 500;
  }
  Serial.print("====pembagian = ");
  Serial.print(pembagian);
  Serial.print("\t====\tBerat pakan masuk : ");
  Serial.print(beratblynk);
  Serial.println("====");
  terminal.print("====pembagian = ");
  terminal.print(pembagian);
  terminal.print("\t====\tBerat pakan masuk : ");
  terminal.print(beratblynk);
  terminal.println("====");
  terminal.flush();
}
//====================masukkan pakan=================
int filling() {
  terminal.println("masuk looping filling");
  t = millis();
  scale.tare();
  int beratnow = timbangan();
  Serial.print("berat pakan: ");
  Serial.println(beratnow);
  terminal.print("berat pakan: ");
  terminal.println(beratnow);
  //for (int i = 1; i <= pembagian; i++) {
  Serial.println("motor stepper dijalankan");
  terminal.println("motor stepper dijalankan");
  while (beratnow <= 500) {
    //t = millis();
    beratnow = timbangan();
    myStepper.setSpeed(10);
    myStepper.step(stepsPerRevolution);
    Serial.print("berat pakan sekarang: ");
    Serial.println(beratnow);
    terminal.print("berat pakan sekarang: ");
    terminal.println(beratnow);
    terminal.flush();
  }
  myStepper.step(0);
  //}
  terminal.println("motor stepper berhenti");
  pembagian = pembagian - 1;
  return pembagian;
  Serial.print("pembagian = ");
  Serial.println(pembagian);
  terminal.print("pembagian = ");
  terminal.println(pembagian);
  terminal.flush();
}

//===================Timbangan=======================
int timbangan() {
  int mass = scale.get_units(5);
  //Serial.print("berat pakan = ");
  //Serial.println(mass);
  return mass;
}
//======================LOOP=========================
unsigned long tbefore = 0;
void loop()
{
  //terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  if (millis() - tbefore > 1000) {
    tbefore = millis();
    if (jamskr == jampagi) {
      if (mntskr == mntpagi) {
        Serial.println("jadwal pakan  pagi");
        terminal.println("jadwal pakan  pagi");
        jalan = 1;
      }
      //else if (mntskr == mntpagi+1)
      else if (mntskr >= mntpagi + 1) jalansblm = 0;
    }
    else if (jamskr == jamsiang) {
      if (mntskr == mntsiang) {
        Serial.println("jadwal pakan  siang");
        jalan = 1;
        terminal.println("jadwal pakan  siang");
      }
      //else if (mntskr == mntsiang+1)
      else if (mntskr >= mntsiang + 1) jalansblm = 0;
    }
    else if (jamskr == jammalam) {
      if (mntskr == mntmalam) {
        Serial.println("jadwal pakan  malam");
        jalan = 1;
        terminal.println("jadwal pakan  malam");
      }
      //else if (mntskr == mntmalam+1)
      else if (mntskr >= mntmalam + 1) jalansblm = 0;
    }
  }
  //long t = millis();
  Blynk.run();
  timer.run();

  if ((jalan == 1) && (jalansblm == 0)) {
    jalansblm = 1;
    Serial.println("Alur alat jalan");
    terminal.println("Alur alat jalan");
    int sisa_filling = pembagian;
    while (pembagian > 0) {
      sisa_filling = filling();
      //long tnow = millis();
      //unsigned long told = tnow;
      terminal.println("motor dc  menyala");
      //while (tnow - told <= 5000) {
      // tnow = millis();
      motordc(0);
      delay(10000);
      //}
      motordc(1);
      terminal.println("motor dc mati");
      terminal.flush();
      //told = tnow;
    }
    takephoto();
    jalan = 0;
    terminal.print("jalan= ");
    terminal.println(jalan);
    terminal.print("jalansblm= ");
    terminal.println(jalansblm);
  }
  terminal.flush();
}

//=========================kamera==============================
void inisialisasi_kamera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
   if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
}

void takephoto()
{
  digitalWrite(4, HIGH);
  delay(200);
  uint32_t randomNum = random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+ (String)randomNum);
  Blynk.setProperty(V2, "urls", "http://"+local_IP+"/capture?_cb="+(String)randomNum);
  digitalWrite(4, LOW);
}
