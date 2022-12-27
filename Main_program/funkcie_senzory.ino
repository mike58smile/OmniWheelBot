float spd1 = 0;
float spd2 = 0;
float spd3 = 0;
float spd4 = 0;

int ReadLineBumper(int address, int sensor) {
  int sensorvalue;
  Wire.requestFrom(address, 18);
  for (int x = 0; x < 9; x++) {
    if (x == sensor) {
      sensorvalue = (Wire.read() | Wire.read() << 8);
    }
    (Wire.read() | Wire.read() << 8);
  }
  return sensorvalue;
}

float ReadGyro(String axis, int mode) {
  mpu6050.update();
  if (mode == "angle") {
    if (axis == "x" || axis == "X") {
      return int(mpu6050.getGyroAngleX() - last_reset_x);
    }
    else if (axis == "y" || axis == "Y") {
      return int(mpu6050.getGyroAngleY() - last_reset_y);
    }
    else if (axis == "z" || axis == "Z") {
      return int(mpu6050.getGyroAngleZ() - last_reset_z);
    }
  }
  else {
    if (axis == "x" || axis == "X") {
      return mpu6050.getAccX();
    }
    else if (axis == "y" || axis == "Y") {
      return mpu6050.getAccY();
    }
    else if (axis == "z" || axis == "Z") {
      return mpu6050.getAccZ();
    }
  }
}
float ReadGyro_blank(String axis, int mode) {
  mpu6050.update();
  if (mode == "angle") {
    if (axis == "x" || axis == "X") {
      return int(mpu6050.getGyroAngleX());
    }
    else if (axis == "y" || axis == "Y") {
      return int(mpu6050.getGyroAngleY());
    }
    else if (axis == "z" || axis == "Z") {
      return int(mpu6050.getGyroAngleZ());
    }
  }
  else {
    if (axis == "x" || axis == "X") {
      return mpu6050.getAccX();
    }
    else if (axis == "y" || axis == "Y") {
      return mpu6050.getAccY();
    }
    else if (axis == "z" || axis == "Z") {
      return mpu6050.getAccZ();
    }
  }
}
void ResetGyro() {
  last_reset_x = ReadGyro_blank("x", "angle");
  last_reset_y = ReadGyro_blank("y", "angle");
  last_reset_z = ReadGyro_blank("z", "angle");
}

int ReadUltra(int ultra) {

  if (ultra == 0) {
    digitalWrite(A14, LOW);
    digitalWrite(A14, HIGH);
    return analogRead(A15) * 1.1;
  }
  else if (ultra == 1) {
    digitalWrite(A12, LOW);
    digitalWrite(A12, HIGH);
    return analogRead(A13) * 1.1;
  }
  else if (ultra == 2) {
    digitalWrite(A10, LOW);
    digitalWrite(A10, HIGH);
    return analogRead(A11) * 1.1;
  }
  else if (ultra == 3) {
    digitalWrite(A8, LOW);
    digitalWrite(A8, HIGH);
    return analogRead(A9) * 1.1;
  }

}

void IR_tester() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.decode_type) {
      case NEC: Serial.println("NEC"); break ;
      case SONY: Serial.println("SONY"); break ;
      case RC5: Serial.println("RC5"); break ;
      case RC6: Serial.println("RC6"); break ;
      case DISH: Serial.println("DISH"); break ;
      case SHARP: Serial.println("SHARP"); break ;
      case JVC: Serial.println("JVC"); break ;
      case SANYO: Serial.println("SANYO"); break ;
      case MITSUBISHI: Serial.println("MITSUBISHI"); break ;
      case SAMSUNG: Serial.println("SAMSUNG"); break ;
      case LG: Serial.println("LG"); break ;
      case WHYNTER: Serial.println("WHYNTER"); break ;
      case AIWA_RC_T501: Serial.println("AIWA_RC_T501"); break ;
      case PANASONIC: Serial.println("PANASONIC"); break ;
      case DENON: Serial.println("DENON"); break ;
      default:
      case UNKNOWN: Serial.println("UNKNOWN"); break ;
    }
    irrecv.resume();
  }
}
//IMPORTANT - STLACIT NET/USB NA OVLADACI DENONE PRED ZACIATKOM OVLADANIA
void IR_read() {
  if (irrecv.decode(&results)) {
    if (results.value == 0XFFFFFFFF)
      results.value = key_value;
    switch (results.value) {
      case 0x7FA4C83F://0x7FA4C83F denon 0x58 tv 
        Serial.println("UP");
        if (spinac == true) {
          pohyb_gyro_smer_loop = 'u';
        }
        else {
          pohyb('u', sp);
        }
        last_case = 'u';
        break;
      case 0x46F93A05://0x46F93A05 denon 0x59 tv
        Serial.println("DOWN");
        if (spinac == true) {
          pohyb_gyro_smer_loop = 'd';
        }
        else {
          pohyb('d', sp);
        }
        last_case = 'd';
        break;
      case 0x6650050F://0x66050F denon 0x5B tv
        Serial.println("RIGHT");
        if (spinac == true) {
          pohyb_gyro_smer_loop = 'r';
        }
        else {
          pohyb('r', sp);
        }
        last_case = 'r';
        break;
      case 0x1304A6AD://0x1304A6AD denon 0x5A tv
        Serial.println("LEFT");
        if (spinac == true) {
          pohyb_gyro_smer_loop = 'l';
        }
        else {
          pohyb('l', sp);
        }
        last_case = 'l';
        break;
      case 0x63D38652://klavesa return
        Serial.println("Tocenie v smere");
        pohyb('+', sp);
        last_case = '+';
        break;
      case 0xE9DFFC5E://klavesa search
        Serial.println("Tocenie proti smeru");
        pohyb('-', sp);
        last_case = '-';
        break;
      case 0x29F1D777://0x29F1D777 denon 0x5C tv
        Serial.println("STOP");
        Serial.println("gyro program OFF");
        STOP();
        Serial.print("Povodny uhol: ");
        Serial.println(ReadGyro("z", "angle"));
        Serial.print("Novy uhol: ");
        ResetGyro();
        Serial.println(ReadGyro("z", "angle"));
        last_case = 's';
        spinac = false;
        break;
      case 0x924FD4DC://VOLUME+
        Serial.println("sp hore");
        sp += 10;
        rychlost_gyro += 10;
        if (spinac == true) {
          pohyb_gyro_smer_loop = last_case;
        }
        else {
          pohyb(last_case, sp);
        }
        //  Serial.println(sp);
        delay(50);
        break;
      case 0x6497D4E4://VOLUME-
        Serial.println("sp dole");
        sp -= 10;
        rychlost_gyro -= 10;
        if (spinac == true) {
          pohyb_gyro_smer_loop = last_case;
        }
        else {
          pohyb(last_case, sp);
        }
        // Serial.println(sp);
        delay(50);
        break;
      case 0xB1D174DE: //num1
        if (Pid_konst == true) {
          Kp += 0.1;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Kp_sp += 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Kp_sp = 0;
        }
        delay(50);
        break;
      case 0x27D54AE8: //num 7
        if (Pid_konst == true) {
          Kp -= 0.1;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Kp_sp -= 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Kp_sp = 0;
        }
        delay(50);
        break;
      case 0xC3EC6C6: //num 2
        if (Pid_konst == true) {
          Kd += 0.01;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Kd_sp += 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Kd_sp = 0;
        }
        delay(50);
        break;
      case 0xFF848A58: //num 8
        if (Pid_konst == true) {
          Kd -= 0.01;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Kd_sp -= 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Kd_sp = 0;
        }
        delay(50);
        break;
      case 0xF18640E8: //num 3
        if (Pid_konst == true) {
          Ki += 0.01;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Ki_sp += 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Ki_sp = 0;
        }
        delay(50);
        break;
      case 0x2A4C917D: //num 9
        if (Pid_konst == true) {
          Ki -= 0.01;
          gyro.SetTunings(Kp, Ki, Kd);
        }
        else {
          Ki_sp -= 1;
          IRpid(0, Kp_sp, Kd_sp, Ki_sp);
          IRpid(1, Kp_sp, Kd_sp, Ki_sp); Ki_sp = 0;
        }
        delay(50);
        break;
      case 0xE234AF28: //mute
        Serial.println("gyro program ON");
        spinac = true;
        rychlost = 20;
        d = 1;
        pohyb_gyro_smer_loop = last_case;
        delay(100);
        break;
      case 0x210F46E7: //CH+
        rychlost_gyro += 10;
        delay(50);
        break;
      case 0x470C3CAB: //CH-
        rychlost_gyro -= 10;
        delay(50);
        break;
      case 0xFB58D49F: //movie
        gyro_motory = 1;
        break;
      case 0x78F5AEE3: //music
        gyro_motory = 2;
        break;
      case 0x52213642: //play btn
        Pid_konst = !Pid_konst;
        delay(200);
    }
    key_value = results.value;
    irrecv.resume();
  }
  else {
    if (spinac == true) {
      //pohyb_GYRO(pohyb_gyro_smer_loop, 10, gyro_motory);
      //----------uprav------------
      float vy = 0.1, vx = -0.1, w = 0;
      alfa = 225;
      //---------------------------
      bool invert = 0;
      if((alfa<=-90 && alfa>-270)|| (alfa>=90 && alfa<270))
        invert = 1; //kvoli nespojitostiam tangensu
      float alfaR = alfa * DEG_TO_RAD;
      /*int alfa = 90;
      alfa = (alfa + 180)%360;
      float alfaR = float(alfa)*DEG_TO_RAD;*/
     
      float v = 0.3;
      float tg_alfaR = tan(alfaR);
      vx = v/(sqrt(pow(tg_alfaR,2) + 1));
      vy = v*tg_alfaR/(sqrt(pow(tg_alfaR,2) + 1));
      if(invert){
        (vx != 0) ? vx *= -1 : vx = 0;
        (vy != 0) ? vy *= -1 : vy = 0;
      }
      
      float w1 = (B*w + vy*cos(alfa1) - vx*sin(alfa1))/R;
      float w2 = (B*w + vy*cos(alfa2) - vx*sin(alfa2))/R;
      float w3 = (B*w + vy*cos(alfa3) - vx*sin(alfa3))/R;
      float w4 = (B*w + vy)/R;
      spd1 = (9.792*w1)/PI; spd2 = (9.792*w2)/PI; spd3 = (9.792*w3)/PI; spd4 = (9.792*w4)/PI;
      float t = millis()/1000.0; // get program time
      //Serial.println(String(t)+" "+String(alfa)+" "+"spd1:"+String(spd1)+"spd2:"+String(spd2)+"spd3:"+String(spd3)+"spd4:"+String(spd4)+"w1:"+String(w1)+"w2:"+String(w2)+"w3:"+String(w3)+"w4:"+String(w4)+"vy:"+String(vy)+"vx"+String(vx));//+"tg_alfaR:"+String(tg_alfaR)+"pow(tg_alfaR,2) + 1:"+String(pow(tg_alfaR,2) + 1)+"sqrt(pow(tg_alfaR,2) + 1):"+String(sqrt(pow(tg_alfaR,2) + 1))+"sin(alfa3):"+String(sin(alfa3))
      Serial.println(String(t)+" "+String(alfa)+" "+" "+String(spd1)+" "+String(spd2)+" "+String(spd3)+" "+String(spd4)+" "+String(w1)+" "+String(w2)+" "+String(w3)+" "+String(w4)+" "+String(vy)+" "+String(vx));
      ConstSpeed(0,sign(spd1),abs(spd1),sign(spd2),abs(spd2));    //SetSpeed(0,motor 1, motor 2)
      //ConstSpeed(1,sign(spd4),abs(spd4),sign(spd3),abs(spd3));    //SetSpeed(1,motor 4, motor 3) -toto som skusal
       /*alfa += 1;
       if(alfa>360)
        alfa = 0;*/
    }
  }
}
void STOP(){
  //startLoopSetup = 1;
  //ConstSpeed(0,1,0,1,0);    //SetSpeed(0,motor 1, motor 2)
  //ConstSpeed(1,1,0,1,0);
  ConstSpeed(0,sign(spd1),0,sign(spd2),0);    //SetSpeed(0,motor 1, motor 2)
  ConstSpeed(1,sign(spd4),0,sign(spd3),0); 
  //SetSpeed(0,0,0);
  //SetSpeed(1,0,0);
}

/*
  prvy stlpec
  NUM 1 0xB1D174DE
  NUM 7 0x27D54AE8

  druhy stlpec
  NUM 2 0xC3EC6C6
  NUM 8 0xFF848A58

  treti stlpec
  NUM 3 0xF18640E8
  NUM 9 0x2A4C917D
*/

//kamera..............................................................................................
void getline(char *buffer, int max_len)
{
  uint8_t idx = 0;
  char c;
  do
  {
    if (idx >= max_len) return;
    if (Serial3.available() != 0){
      c = Serial3.read();
      buffer[idx++] = c;
    }
    }
  
  while (c != '\n' && c != '\r');
  if (idx >= max_len) return;
  buffer[idx] = 0;

}

int kamera() {
  char buffer [63 + 1];
  int max_len = 63;
  getline(buffer, max_len);
  int kam = atoi(buffer);
  if(kam==0){return;}
  return kam;
}
