#define SERIAL_PRINT

#include <Wire.h>
#include <i2cdetect.h>
#include <Encoder.h>
#include <PID_v1.h>
//Motor1
#define M1_LPWM 5
#define M1_RPWM 6
#define M1_REN 4
#define M1_LEN 7
//Motor2
#define M2_RPWM 10
#define M2_LPWM 11
#define M2_REN 8
#define M2_LEN 9

#define address 0x10
int wireread;
int setpoint,position,speed;
double RP, Output,Setpoint;
double RP_2, Output_2,Setpoint_2;
//-------------synchro 2 motorov----------------------
double vystup, kraj;
double vstup = 0;
int cas=20; //v milisekundach
int position1_syn,position2_syn;
double Kp_syn=0, Ki_syn=250/cas, Kd_syn=0;
double nrychlost;
unsigned long previousMillis_1 = 0;
unsigned long previousMillis_2 = 0;
long v1,v2;
//---------------------------------------------------
// const speed
double vystup_sp1, kraj_sp1;
double vstup_sp1 = 0;
double vystup_sp2, kraj_sp2;
double vstup_sp2 = 0;
double Kp_sp=1, Ki_sp=2.4, Kd_sp=0;
bool rozhodovac2;
String data, dataN,dataPlot;
//-----------------------------------
double Kp=15, Ki=2, Kd=5;
PID pid(&RP, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
PID pid2(&RP_2, &Output_2, &Setpoint_2, Kp, Ki, Kd, DIRECT);
PID syn(&vstup, &vystup, &kraj, Kp_syn, Ki_syn, Kd_syn, DIRECT); //na synchro
PID const_speed1(&vstup_sp1, &vystup_sp1, &kraj_sp1, Kp_sp, Ki_sp, Kd_sp, DIRECT);
PID const_speed2(&vstup_sp2, &vystup_sp2, &kraj_sp2, Kp_sp, Ki_sp, Kd_sp, DIRECT);

bool motor_go1,motor_go2;
bool motorcontrol1,motorcontrol2;
int motorspeed1,motorspeed2;
int position1,position2;
int setpoint1,setpoint2;
//synchro
bool syn_rozhodovac = false;
int motorspeed1_synchro,motorspeed2_synchro;

Encoder enc1(2, 12);//motor 1
Encoder enc2(3, A0);//motor 2
void setup() {
  Serial.begin(115200);
  pinMode(M1_RPWM,OUTPUT);
  pinMode(M1_LPWM,OUTPUT);
  pinMode(M1_REN,OUTPUT);
  pinMode(M1_LEN,OUTPUT);

  pinMode(M2_RPWM,OUTPUT);
  pinMode(M2_LPWM,OUTPUT);
  pinMode(M2_REN,OUTPUT);
  pinMode(M2_LEN,OUTPUT);
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-60,60);
  pid2.SetMode(AUTOMATIC);
  pid2.SetOutputLimits(-60,60);
  syn.SetMode(AUTOMATIC);
  syn.SetOutputLimits(-50,50);
  const_speed1.SetMode(AUTOMATIC);
  const_speed1.SetOutputLimits(-150,150);
  const_speed2.SetMode(AUTOMATIC);
  const_speed2.SetOutputLimits(-150,150);
  Wire.begin(address);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveData);
  digitalWrite(M1_REN,HIGH);
  digitalWrite(M1_LEN,HIGH);
  digitalWrite(M2_REN,HIGH);
  digitalWrite(M2_LEN,HIGH);
}

void loop() {
  #ifdef SERIAL_PRINT
  //data=("KP: "+String(Kp_sp,3)+" Kd: "+String(Kd_sp,3)+" Ki: "+String(Ki_sp, 3)+" v1: "+String(v1)+" vystup1: "+String(vystup_sp1)+"vystup2: "+String(vystup_sp1)+" kraj1: "+String(kraj_sp1)+" kraj2: "+String(kraj_sp2)+" msp1: "+String(motorspeed1)+" msp2: "+String(motorspeed2));
   float t = millis()/1000.0; // get program time
   dataPlot = (String(Kp_sp,3)+" "+String(Kd_sp,3)+ " "+String(Ki_sp,3)+" "+String(v2)+" "+String(vystup_sp2)+" "+String(kraj_sp2) +" "+String(wireread)+" "+String(rozhodovac2)+" "+String(motorcontrol2)+" "+String(motorspeed1));
  dataN=("KP:"+String(Kp_sp,3)+ "\t"+"Kd:"+String(Kd_sp,3)+ ","+"Ki:"+String(Ki_sp,3)+ ","+"v2(vstup):"+String(v2)+","+"vystup2:"+String(vystup_sp2)+","+"kraj2(w-pozadovana_hodnota):"+String(kraj_sp2)+","+"msp2:"+String(motorspeed2));
  Serial.println(dataPlot);
  #endif
if(!syn_rozhodovac){
  //motor 1
  if(motorcontrol1 == 0){           //riadenie o uhol
    position1 = enc1.read()/2.72;
    Setpoint = setpoint1;
    RP = position1;
    pid.Compute();
    if(setpoint1 < position1){
      analogWrite(M1_LPWM,0);
      analogWrite(M1_RPWM,abs(Output));
      motor_go1 = 1;
    }
    else if(setpoint1 > position1){
      analogWrite(M1_RPWM,0);
      analogWrite(M1_LPWM,abs(Output));
      motor_go1 = 1;
    }
    else {
      analogWrite(M1_LPWM,0);
      analogWrite(M1_RPWM,0);
      motor_go1 = 0;
    }
  }
  else if(motorcontrol1 != 3){
    unsigned long currentMillis_1 = millis();
    if(!vstup_sp1 && !kraj_sp1){
      const_speed1.SetMode(MANUAL);
      vystup_sp1 = 0;
    } else const_speed1.SetMode(AUTOMATIC);
    if(motorspeed1 > 0){            //riadenie iba rychlosti
      if(rozhodovac2){
        if((abs(motorspeed1)+vystup_sp1) <= 0 || (!vstup_sp1 && !kraj_sp1))
          analogWrite(M1_RPWM,0);
        else
        analogWrite(M1_RPWM,(abs(motorspeed1)+vystup_sp1));
      }
      else
        analogWrite(M1_RPWM,abs(motorspeed1));

      analogWrite(M1_LPWM,0);
      motor_go1 = 1;
    }
    else if(motorspeed1 < 0){
      if (rozhodovac2){
         if((abs(motorspeed1)+vystup_sp1)<=0 || (!vstup_sp1 && !kraj_sp1))
            analogWrite(M1_LPWM,0);
        else
          analogWrite(M1_LPWM,(abs(motorspeed1)+vystup_sp1));
      }
      else
        analogWrite(M1_LPWM,abs(motorspeed1));

      analogWrite(M1_RPWM,0);
      motor_go1 = 1;
    }
    else{
      analogWrite(M1_RPWM,0);
      analogWrite(M1_LPWM,0);
    }
    if ((unsigned long)(currentMillis_1 - previousMillis_1) >= cas) {
      v1 = abs(enc1.read());
      enc1.write(0);
      previousMillis_1 = currentMillis_1;
      if(rozhodovac2){
        vstup_sp1 = v1;
        const_speed1.Compute();
      }
    }
  }

//motor2
  if(motorcontrol2 == 0){           //riadenie o uhol
    position2 = enc2.read()/2.72;
    Setpoint_2 = position1;
    RP_2 = position2;
    pid2.Compute();

    if(setpoint2 < position2){
      analogWrite(M2_LPWM,0);
      analogWrite(M2_RPWM,abs(Output));
      motor_go2 = 1;
    }
    else if(setpoint2 > position2){
      analogWrite(M2_RPWM,0);
      analogWrite(M2_LPWM,abs(Output));
      motor_go2 = 1;
    }
    else {
      analogWrite(M2_LPWM,0);
      analogWrite(M2_RPWM,0);
      motor_go2 = 0;
    }
  }
  else if(motorcontrol2 == 3);
  else{
    unsigned long currentMillis_2 = millis();
    if(!vstup_sp2 && !kraj_sp2){
      const_speed2.SetMode(MANUAL);
      vystup_sp2 = 0;
    } else const_speed2.SetMode(AUTOMATIC);
    if(motorspeed2 > 0){            //riadenie iba rychlosti
      if(rozhodovac2){
        if((abs(motorspeed2)+vystup_sp2)<=0 || (!vstup_sp2 && !kraj_sp2))
          analogWrite(M2_RPWM,0);
        else
          analogWrite(M2_RPWM,(abs(motorspeed2)+vystup_sp2));
      }
      else
        analogWrite(M2_RPWM,abs(motorspeed2));
        
      analogWrite(M2_LPWM,0);
      motor_go2 = 1;
    }
    else if(motorspeed2 < 0){
      if(rozhodovac2){
        if((abs(motorspeed2)+vystup_sp2)<=0 || (!vstup_sp2 && !kraj_sp2)){
          analogWrite(M2_LPWM,0);
        }
        else
          analogWrite(M2_LPWM,(abs(motorspeed2)+vystup_sp2));
      }
      else
        analogWrite(M2_LPWM,abs(motorspeed2));
        
      analogWrite(M2_RPWM,0);
      motor_go2 = 1;
    }
    else{
      analogWrite(M2_RPWM,0);
      analogWrite(M2_LPWM,0);
    }
    if ((unsigned long)(currentMillis_2 - previousMillis_2) >= cas) {
      v2 = abs(enc2.read());
      enc2.write(0);
      previousMillis_2 = currentMillis_2;
      if(rozhodovac2){
        vstup_sp2 = v2;
        const_speed2.Compute();
      }
    }
  }
}
else
  synchro(motorspeed1_synchro,motorspeed2_synchro);
}
void requestEvent(){
  //Motor1
  Wire.write(position1);
  Wire.write((position1 >> 8));
  Wire.write(position2);
  Wire.write((position2 >> 8));
  Wire.write(setpoint1);
  Wire.write((setpoint1 >> 8));
  Wire.write(setpoint2);
  Wire.write((setpoint2 >> 8));
  Wire.write(int(vystup_sp1));
  Wire.write((int(vystup_sp1) >> 8));
  Wire.write(int(vystup_sp2));
  Wire.write((int(vystup_sp2) >> 8));
  Wire.write(v1);
  Wire.write((v1 >> 8));
  Wire.write(v2);
  Wire.write((v2 >> 8));
}
void receiveData(){
    int mode = Wire.read();
    wireread = mode;
    if(mode == 0){ //funkcia ResetSetpoint v Main arduine
      syn_rozhodovac = false;
      rozhodovac2 = false;
      stopmotor();
      motorcontrol1 = 3;
      motorcontrol2 = 3;
      Setpoint = 0;
      enc1.write(0);
      enc2.write(0);
      position1,position2,setpoint2,setpoint1=0;
    }

    else if(mode == 1){ //funkcia SetSpeed v Main arduine
      syn_rozhodovac = false;
      rozhodovac2 = false;
      motorcontrol1 = 1;
      motorcontrol2 = 1;
      motorspeed1 = (Wire.read() | Wire.read() << 8);
      motorspeed2 = (Wire.read() | Wire.read() << 8);
    }
    else if(mode == 10){
      syn_rozhodovac = true;
      rozhodovac2 = false;
      motorspeed1_synchro = (Wire.read() | Wire.read() << 8);
      motorspeed2_synchro = (Wire.read() | Wire.read() << 8);
    }
    else if(mode == 8){
    /*Serial.print("motor speed1: ");Serial.print(motorspeed1); Serial.print(" ");
    Serial.print("kraj 1: ");Serial.println(kraj_sp1);*/
      syn_rozhodovac = false;
      rozhodovac2 = true;
      motorcontrol2 = 2;
      motorcontrol1 = 2;
      motorspeed1 = (Wire.read() | Wire.read() << 8);
      kraj_sp1 = (Wire.read() | Wire.read() << 8);
      motorspeed2 = (Wire.read() | Wire.read() << 8);
      kraj_sp2 = (Wire.read() | Wire.read() << 8);
    }
    else if(mode == 16){
      Kp_sp += (double)(Wire.read() | Wire.read() << 8) / 10;
      Kd_sp += (double)(Wire.read() | Wire.read() << 8) / 1000;
      Ki_sp += (double)(Wire.read() | Wire.read() << 8) / 10;
      const_speed1.SetTunings(Kp_sp, Ki_sp, Kd_sp);
      const_speed2.SetTunings(Kp_sp, Ki_sp, Kd_sp);
    }
    else{ //funkcia Setsetpoint v Main arduine
      syn_rozhodovac = false;
      rozhodovac2 = false;
      motorcontrol1 = 0;
      motorcontrol2 = 0;
      setpoint1 = (Wire.read() | Wire.read() << 8);
      setpoint2 = (Wire.read() | Wire.read() << 8);
    }
}
void stopmotor(){
  analogWrite(M1_RPWM,0);
  analogWrite(M2_RPWM,0);
  analogWrite(M1_LPWM,0);
  analogWrite(M2_LPWM,0);
  delay(100);
}
void synchro(int rychlost1, int rychlost2){
  unsigned long currentMillis_2 = millis();
    if(rychlost2 < 0){
      analogWrite(M2_LPWM,abs(rychlost2));
      analogWrite(M2_RPWM,0);
      motor_go2 = 1;
    }
    else if(rychlost2 > 0){
      analogWrite(M2_LPWM,0);
      analogWrite(M2_RPWM,abs(rychlost2));
      motor_go2 = 1;
    }
    else{
      analogWrite(M2_LPWM,0);
      analogWrite(M2_RPWM,0);
    }
    position2_syn = abs(enc2.read());
    if ((unsigned long)(currentMillis_2 - previousMillis_2) >= cas) {
      v2 = position2_syn;
      enc2.write(0);
      previousMillis_2 = currentMillis_2;
    }

    unsigned long currentMillis_1 = millis();
    nrychlost = abs(rychlost1) + vystup;
    if(rychlost1 < 0){
      analogWrite(M1_LPWM,nrychlost);
      analogWrite(M1_RPWM,0);
      motor_go1 = 1;
    }
    else if(rychlost1 > 0){
      analogWrite(M1_LPWM,0);
      analogWrite(M1_RPWM,nrychlost);
      motor_go1 = 1;
    }
    else{
      analogWrite(M1_LPWM,0);
      analogWrite(M1_RPWM,0);
    }
    position1_syn = abs(enc1.read());
    if ((unsigned long)(currentMillis_1 - previousMillis_1) >= cas) {
      v1 = position1_syn;
      //Serial.println((v2-v1));
      enc1.write(0);
      previousMillis_1 = currentMillis_1;
    }
  kraj = v2;
  vstup = v1;
  syn.Compute();
}
