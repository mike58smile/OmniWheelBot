void MotorInfo(){
  GetPosition(0);
  Serial.print("Motor_1: ");
  Serial.print("Position: ");
  Serial.print(position1);
  Serial.print(" ");
  Serial.print(position2);
  Serial.print("     Setpoint: ");
  Serial.print(setpoint1);
  Serial.print(" ");
  Serial.print(setpoint2);
  Serial.print("     motor_go: ");
  Serial.print(motor_go1);
  Serial.print(" ");
  Serial.print(motor_go2);
  Serial.print(" ");
  Serial.print("smer: ");
  Serial.print(smer);
  Serial.print("     rychlost: ");
  Serial.print(rychlost1);
  Serial.print(" ");
  Serial.print(rychlost2);
  Serial.print(" ");
  GetPosition(1);
  Serial.print("Motor_2: ");
  Serial.print("Position: ");
  Serial.print(position1_2);
  Serial.print(" ");
  Serial.print(position2_2);
  Serial.print("     Setpoint: ");
  Serial.print(setpoint1_2);
  Serial.print(" ");
  Serial.print(setpoint2_2);
  Serial.print("     motor_go: ");
  Serial.print(motor_go1_2);
  Serial.print(" ");
  Serial.print(motor_go2_2);
  Serial.print(" ");
  Serial.print("smer: ");
  Serial.print(smer_2);  
  Serial.print("     rychlost: ");
  Serial.print(rychlost1_2);
  Serial.print(" ");
  Serial.println(rychlost2_2);
  //Serial.print(" ");
  //Serial.println(ReadGyro("z","angle"));
}

void ListI2C(){
  i2cdetect();
  delay(2000);  
}
void GyroInfo(){
  Serial.print(" X: ");
  Serial.print(ReadGyro("x","angle"));
  Serial.print("  ");
  Serial.print(" Y: ");
  Serial.print(ReadGyro("y","angle"));
  Serial.print("  ");
  Serial.print(" Z: ");
  Serial.print(ReadGyro("z","angle"));
  Serial.print("  ");
  Serial.print(" X: ");
  Serial.print(ReadGyro("x","acc"));
  Serial.print("  ");
  Serial.print("g Y: ");
  Serial.print(ReadGyro("y","acc"));
    Serial.print("  ");
  Serial.print("g Z: ");
  Serial.println(ReadGyro("z","acc"));
  Serial.println("g");
}
void LineBumperInfo(int address){
  for(int x=0;x<9;x++){
    Serial.print(ReadLineBumper(address, x));
    Serial.print(" ");
  }
  Serial.println("");
}
void UltraInfo(){
  for(int x=0;x<4;x++){
    Serial.print(ReadUltra(x));
    Serial.print("  ");
    delay(40);
  }  
  Serial.println();
}
void Pid_print(){
  Serial.print("KP: ");
  Serial.print(Kp);
  Serial.print(" ");
  Serial.print("Kd: ");
  Serial.print(Kd);
  Serial.print(" ");
  Serial.print("Ki: ");
  Serial.println(Ki);
}
