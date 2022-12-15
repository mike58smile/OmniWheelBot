//Nastavenie setpointu motoru
void SetSetpoint(int motor_select, int setpoint1,int setpoint2){
  if(motor_select == 0){
    Wire.beginTransmission(0x10);
    Wire.write(2);
    Wire.write(setpoint1);
    Wire.write((setpoint1 >> 8));
    Wire.write(setpoint2);
    Wire.write((setpoint2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(2);
    Wire.write(setpoint1);
    Wire.write((setpoint1 >> 8));
    Wire.write(setpoint2);
    Wire.write((setpoint2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 2){
    Wire.beginTransmission(0x12);
    Wire.write(2);
    Wire.write(setpoint1);
    Wire.write((setpoint1 >> 8));
    Wire.endTransmission();
  }
}
//Nastavenie rychlosti motoru
void SetSpeed(int motor_select, int speed1,int speed2){
  if(motor_select == 0){
    Wire.beginTransmission(0x10);
    Wire.write(1);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(1);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 2){
    Wire.beginTransmission(0x12);
    Wire.write(1);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.endTransmission();
  }
}
void SetSpeed_syn(int motor_select, int speed1,int speed2){
  if(motor_select == 0){
    Wire.beginTransmission(0x10);
    Wire.write(10);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(10);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.endTransmission();
  }
}
void IRpid(int motor_select,int kp,int kd,int ki){
  if(motor_select == 0){
    Serial.println("xxx");
    Wire.beginTransmission(0x10);
    Wire.write(16);
    Wire.write(kp);
    Wire.write((kp >> 8));
    Wire.write(kd);
    Wire.write((kd >> 8));
    Wire.write(ki);
    Wire.write((ki >> 8));
    Wire.endTransmission();
  }
  if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(16);
    Wire.write(Kp_sp);
    Wire.write((Kp_sp >> 8));
    Wire.write(Kd_sp);
    Wire.write((Kd_sp >> 8));
    Wire.write(Ki_sp);
    Wire.write((Ki_sp >> 8));
    Wire.endTransmission();
  }
}
void ConstSpeed(int motor_select, int speed1, int kraj1, int speed2, int kraj2){
  if(motor_select == 0){
    Wire.beginTransmission(0x10);
    Wire.write(8);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(kraj1);
    Wire.write((kraj1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.write(kraj2);
    Wire.write((kraj2 >> 8));
    Wire.endTransmission();
  }
  else if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(8);
    Wire.write(speed1);
    Wire.write((speed1 >> 8));
    Wire.write(kraj1);
    Wire.write((kraj1 >> 8));
    Wire.write(speed2);
    Wire.write((speed2 >> 8));
    Wire.write(kraj2);
    Wire.write((kraj2 >> 8));
    Wire.endTransmission();
  }
}
//Zresetuje setpoint motoru
void ResetSetpoint(int motor_select){
  if(motor_select == 0){
    Wire.beginTransmission(0x10);
    Wire.write(0);
    Wire.endTransmission();
  }
  else if(motor_select == 1){
    Wire.beginTransmission(0x11);
    Wire.write(0);
    Wire.endTransmission();  
  }
  else if(motor_select == 2){
    Wire.beginTransmission(0x12);
    Wire.write(0);
    Wire.endTransmission();
  }
}

//Ziska poziciu enkodera z motoru
void GetPosition(int motor_select){
  if(motor_select == 0){
    Wire.requestFrom(0x10,16);
    position1 = (Wire.read() | Wire.read() << 8);
    position2 = (Wire.read() | Wire.read() << 8);
    setpoint1 = (Wire.read() | Wire.read() << 8);
    setpoint2 = (Wire.read() | Wire.read() << 8);
    motor_go1 = (Wire.read() | Wire.read() << 8);
    motor_go2 = (Wire.read() | Wire.read() << 8);
    rychlost1 = (Wire.read() | Wire.read() << 8);
    rychlost2 = (Wire.read() | Wire.read() << 8);
  }
  else if(motor_select == 1){
    Wire.requestFrom(0x11,16);
    position1_2 = (Wire.read() | Wire.read() << 8);
    position2_2 = (Wire.read() | Wire.read() << 8);
    setpoint1_2 = (Wire.read() | Wire.read() << 8);
    setpoint2_2 = (Wire.read() | Wire.read() << 8);
    motor_go1_2 = (Wire.read() | Wire.read() << 8);
    motor_go2_2 = (Wire.read() | Wire.read() << 8);
    rychlost1_2 = (Wire.read() | Wire.read() << 8);
    rychlost2_2 = (Wire.read() | Wire.read() << 8);
  }
  else if(motor_select == 2){
    Wire.requestFrom(0x12,6);
    position_3 = (Wire.read() | Wire.read() << 8);
    setpoint_3 = (Wire.read() | Wire.read() << 8);
    motor_go_3 = (Wire.read() | Wire.read() << 8);
  }
}
