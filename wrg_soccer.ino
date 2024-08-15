#include <POP32.h>
#include <Wire.h>
#include <POP32_Pixy2.h>
POP32_Pixy2 pixy;
unsigned long loopTimer = millis();
#define degToRad 0.0174f
#define sin30 sin(30.f * degToRad)
#define cos30 cos(30.f * degToRad)
// rotate controller
#define rot_Kp 1.5//1.5
#define rot_Ki 0.0
#define rot_Kd 0.0
#define idleSpd 30  // speed when not discovered ball
#define rotErrorGap 15
#define sp_rot 160
float rot_error, rot_pError, rot_i, rot_d, rot_w;
#define Gsp_rot 160
float Grot_error, Grot_pError, Grot_i, Grot_d, Grot_w;
int ballPosX;
int goalPosX;
int goalPosY;
// fling controller
#define fli_Kp 2
#define fli_Ki 0.0
#define fli_Kd 0.0
#define flingErrorGap 15
float spFli = 240;
float GspFli = 200;
float fli_error, fli_pError, fli_i, fli_d, fli_spd;
float Gfli_error, Gfli_pError, Gfli_i, Gfli_d, Gfli_spd;
int ballPosY;
int count = 0;
int restate = 0;
float thetaRad, vx, vy, spd1, spd2, spd3;

uint8_t rxCnt = 0, rxBuf[8];
float pvYaw;
#define head_Kp 1.5f//2.3
#define head_Ki 0.0f
#define head_Kd 0.5f
float head_error, head_pError, head_w , head_d, head_i;
void wheel(int s1, int s2, int s3) {
  motor(1, s1);
  motor(2, s2);
  motor(3, s3);
}
void holonomic(float spd, float theta, float omega) {
  thetaRad = theta * degToRad;
  vx = spd * cos(thetaRad);
  vy = spd * sin(thetaRad);
  spd1 =   vy * cos30 - vx * sin30 + omega;
  spd2 = - vy * cos30 - vx * sin30 + omega;
  spd3 =   vx + omega;
  wheel(spd1, spd2, spd3);
}
void zeroYaw() {
  Serial1.begin(115200); delay(100);
  // Sets data rate to 115200 bps
  Serial1.write(0XA5); Serial1.write(0X54); delay(100);
  // pitch correction roll angle
  Serial1.write(0XA5); Serial1.write(0X55); delay(100);
  // zero degree heading
  Serial1.write(0XA5); Serial1.write(0X52); delay(100);
  // automatic mode
}
bool getIMU() {
  while (Serial1.available()) {
    rxBuf[rxCnt] = Serial1.read();
    if (rxCnt == 0 && rxBuf[0] != 0xAA) return false;
    rxCnt++;
    if (rxCnt == 8) { // package is complete
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) { // data package is correct
        pvYaw = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.f;
        return true;
      }
    }
  }
  return false;
}
void Auto_zero() {
  zeroYaw();
  getIMU();
  int timer = millis();
  oled.clear();
  oled.text(1, 2, "Setting zero");
  while (abs(pvYaw) > 0.02) {
    if (getIMU()) {
      oled.text(3, 6, "Yaw: %f  " , pvYaw);
      oled.show();
      //beep();
      if (millis() - timer > 5000) {
        zeroYaw();
        timer = millis();
      }
    }
  }
  oled.clear();
  oled.show();
}
void heading(float spd, float theta, float spYaw) {
  head_error = spYaw - pvYaw;
  head_i = head_i  + head_error;
  head_i = constrain(head_i , -180, 180);
  head_d =  head_error - head_pError;
  head_w = (head_error * head_Kp) + (head_i * head_Ki) + (head_d * head_Kd);
  head_w = constrain(head_w , -100, 100);
  holonomic(spd, theta, head_w);
  head_pError = head_error;
}

void setup() {
  Serial.begin(115200);
  pinMode(PA0, INPUT);
  pinMode(PA1, INPUT);
  pinMode(PA2, INPUT);
  pixy.init();
  zeroYaw();

  Auto_zero();

  waitSW_A_bmp();
}

void loop() {
  int L = analogRead(PA3);
  int R = analogRead(PA1);
  int M = analogRead(PA0);
  int Mid = analogRead(PA5);
  //  oled.text(3, 6, "M - %d  " , M);
  //  oled.show();
  //  Serial.print(L);
  //  Serial.print("--------");
  //  Serial.print(R);
  //  Serial.print("--------");
  //  Serial.println(M);
  int initialYaw = 90;
  getIMU();
  motor(4, 100);

  if (pixy.updateBlocks() && pixy.sigSize[1]) {


    ballPosX = pixy.sigInfo[1][0].x;
    ballPosY = pixy.sigInfo[1][0].y;
    Serial.println(ballPosY);
    goalPosX = pixy.sigInfo[2][0].x;
    goalPosY = pixy.sigInfo[2][0].y;
    //    Serial.print(ballPosX);
    //    Serial.print("---------");
    Grot_error = Gsp_rot - goalPosX;
    Grot_i = Grot_i + Grot_error;
    Grot_i = constrain(Grot_i, -100, 100);
    Grot_d = Grot_error - Grot_pError;
    Grot_pError = Grot_error;
    Grot_w = (Grot_error * rot_Kp) + (Grot_i * rot_Ki) + (Grot_d * rot_Kd);
    Grot_w = constrain(Grot_w, -100, 100);

    rot_error = sp_rot - ballPosX;
    rot_i = rot_i + rot_error;
    rot_i = constrain(rot_i, -100, 100);
    rot_d = rot_error - rot_pError;
    rot_pError = rot_error;
    rot_w = (rot_error * rot_Kp) + (rot_i * rot_Ki) + (rot_d * rot_Kd);
    rot_w = constrain(rot_w, -100, 100);


    //    Serial.println(ballPosY);
    fli_error = spFli - ballPosY;
    fli_i = fli_i + fli_error;
    fli_i  = constrain(fli_i , -100, 100);
    fli_d = fli_error - fli_pError;
    fli_pError = fli_error;
    fli_spd = fli_error * fli_Kp + fli_i * fli_Ki + fli_d * fli_Kd;
    fli_spd = constrain(fli_spd , -100, 100);

    Gfli_error = GspFli - goalPosY;
    Gfli_i = Gfli_i + Gfli_error;
    Gfli_i  = constrain(Gfli_i , -100, 100);
    Gfli_d = Gfli_error - Gfli_pError;
    Gfli_pError = Gfli_error;
    Gfli_spd = Gfli_error * fli_Kp + Gfli_i * fli_Ki + Gfli_d * fli_Kd;
    Gfli_spd = constrain(Gfli_spd , -100, 100);
    while (M >= 1500 && restate == 1 ) {
      loopTimer = millis();  // ตั้งค่า loopTimer ใหม่ครั้งแรก

      while (millis() - loopTimer < 400) {  // ตรวจสอบเวลาที่ผ่านไป
        getIMU();
        motor(4, -100);
        heading(100, 90, 0);
      }

      while (millis() - loopTimer >= 400 && millis() - loopTimer < 800) {  // ตรวจสอบเวลาที่ผ่านไป
        getIMU();
        motor(4, 100);
        heading(100, -90, 0);
      }
      restate = 0;
      loopTimer = millis();
    }
    while (M >= 1000 && restate == 0) {
      getIMU();
      motor(4, -100);
      heading(100, -90, 0);
      if (millis() - loopTimer >= 500)break;
    }
    //loopTimer = millis();
    while ( R >= 1500 && restate == 0 ) {
      getIMU();
      heading(100, 180, 0);
      if (millis() - loopTimer >= 500)break;
    }
    loopTimer = millis();
    while ( L >= 1500 && restate == 0 ) {
      getIMU();
      heading(100, 0, 0);
      if (millis() - loopTimer >= 500)break;
    }
    loopTimer = millis();



    if (ballPosY < 192) {
      getIMU();
      restate = 0;
      heading(fli_spd, initialYaw + rot_w , 0);

    }

    else if (ballPosY >= 192 && pixy.updateBlocks() && pixy.sigSize[2] && goalPosY >= 45) {
      getIMU();
      restate = 1;
      // ถ้าโกล์มีขนาดใหญ่หรือใกล้มากให้วิ่งตรงไป
      if (goalPosX > 100 && goalPosX < 200) {
        loopTimer = millis();
        getIMU();
        restate = 1;
        heading(100, 90, 0);  // วิ่งตรงไปในทิศ 0 องศา
        while (millis() - loopTimer >= 500)break;

      }

      else {
        loopTimer = millis();
        getIMU();
        restate = 1;
        heading(Gfli_spd, initialYaw + Grot_w , 0);
      }
    }
    else if (ballPosY >= 192 && pixy.updateBlocks() && pixy.sigSize[2] && goalPosY < 45) {
      getIMU();
      restate = 0;
      heading(100, 90, 0);
    }


    //    if (ballPosY >= 85 && ballPosY <= 95 && ballPosX >= (sp_rot - 2) && ballPosX <= (sp_rot + 2)) {
    //      heading(100, 90, 0);
    //      delay(100);
    //      shoot();
    //      reload();
    //    }
  }
  else if (Mid > 1000) {

    getIMU();
    restate = 0;//1

    heading(100, 90, 0);

  }
  else {
    getIMU();
    restate = 0;
    heading(80, -90, 0);
    while (R >= 1500 || L >= 1500) {
      getIMU();
      heading(100, 90, 0);
      if (millis() - loopTimer >= 600)break;
    }
    loopTimer = millis();


  }
}
