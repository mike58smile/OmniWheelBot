//Debuging setup
//#define LINE
//#define DEBUG_GYRO
//#define DEBUG_LINEBUMPER
//#define DEBUG_ULTRA
//#define IR_TESTER
//#define kamerax
#define LINEBUMPER_ADDRESS 0x20
//-----------------------------
#include <Wire.h>
#include <i2cdetect.h>
#include <MPU6050_tockn.h>
#include <IRremote.h>
//PID-------------------------------------------
#include <PID_v1.h>
double Kp = 1, Ki = 0, Kd = 0; //Gyro - Kp 2 ostatne nula
double vstup, vystup, Setpoint;
double Kp2 = 1.2, Ki2 = 0, Kd2 = 0;
double vstup2, vystup2, Setpoint2;
PID gyro(&vstup, &vystup, &Setpoint, Kp, Ki, Kd, DIRECT);
PID kam_platform(&vstup2, &vystup2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);
MPU6050 mpu6050(Wire);
//----------------------------------------------
//IR
const int RECV_PIN = 40;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;
char last_case;
int sp = 50;
bool spinac;
int gyro_motory = 1;
//------------------------------------
//gyro
float last_reset_x;
float last_reset_y;
float last_reset_z;
int rychlost_gyro = 15;
int rychlost = 45;
int zrych_const = 1;
unsigned long previous_millis = 0;
unsigned long current_millis = 0, current_millis2 = 0;
char smer_old;

//kamera
int rozsah = 0;
int rychlost_stopu = 60;
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.setTimeout(10);
  Serial3.begin(19200);
  Serial3.setTimeout(100);

  //Ultrasonic
  pinMode(A14, OUTPUT);
  digitalWrite(A14, HIGH);
  pinMode(A12, OUTPUT);
  digitalWrite(A12, HIGH);
  pinMode(A10, OUTPUT);
  digitalWrite(A10, HIGH);
  pinMode(A8, OUTPUT);
  digitalWrite(A8, HIGH);

  //IR
  irrecv.enableIRIn();
  irrecv.blink13(true);

  //gyro
  mpu6050.begin();
  mpu6050.setGyroOffsets(-0.87, -1.02, -1.52); //old: (-0.88, -1.23, -1.4)

  //pid
  gyro.SetMode(AUTOMATIC);
  gyro.SetOutputLimits(-80, 80);

  kam_platform.SetMode(AUTOMATIC);
  kam_platform.SetOutputLimits(-170, 170);
  kam_platform.SetSampleTime(3);



  //bakalarka kinematika
#define DEG_TO_RAD 0.017453292519943295769236907684886;
#define RAD_TO_DEG 57.295779513082320876798154814105;
#define B 0.14 //%polomer robota v metroch
#define R 0.025 //%polomer kolesa v metroch
#define alfa1 HALF_PI 
#define alfa2 PI
#define alfa3 -HALF_PI
#define alfa4 0
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))

}
//stuff
int Kp_sp = 0, Kd_sp = 0, Ki_sp = 0;
bool Pid_konst = true;

String serialInput;
int data;
uint8_t UltraData[4];
unsigned int UltraValue = 0;
uint8_t EnUltraCmd[4] = {0x22, 0x00, 0x00, 0x22};

//motor1
int position1, position2;
int setpoint1, setpoint2, smer;
int motor_go1, motor_go2;
int rychlost1, rychlost2;
//motor2
int position1_2, position2_2;
int setpoint1_2, setpoint2_2, smer_2;
int motor_go1_2, motor_go2_2;
int rychlost1_2, rychlost2_2;
//Motor3
int position_3;
int setpoint_3, smer_3;
int motor_go_3;

int smer1 = 1;
int smer2 = 1;
int smer3 = 1;
int smer4 = 1;
int v;
char pohyb_gyro_smer_loop;
int StartingAngle;
int error_angle = 0;
int kameravalue = 0;
unsigned long newCas = 0;

//set movement
bool startLoopSetup = 1; //povoli setup sekvenciu na zaciatku programu




int  alfa = 0;//dat precccccccccc
void loop() {
if(startLoopSetup){
  STOP();
  ResetSetpoint(0);
  ResetSetpoint(1);
  digitalWrite(13,HIGH);
  delay(1000);
  //ConstSpeed(0,40,5,40,9);    //SetSpeed(0,motor 1, motor 2)
//ConstSpeed(1,40,20,40,13);    //SetSpeed(1,motor 4, motor 3)*/
  startLoopSetup = 0;
  digitalWrite(13,LOW);
}
#ifdef DEBUG_GYRO
  while (true) {
    GyroInfo();
  }
#endif
#ifdef IR_TESTER
  while (true) {
    IR_tester();
  }
#endif
#ifdef LINE
while(true){
  LineBumperInfo(LINEBUMPER_ADDRESS);
}
#endif
#ifdef kamerax
  while (true) {
    Serial.println(kamera());
  }
#endif
//SetSpeed(1, 50,50);
  IR_read();
//ConstSpeed(1,20,20,20,20);
//MotorInfo();

//SetSetpoint(1,1080,1080);
}
