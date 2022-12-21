unsigned int hexToDec(String hexString) {
  
  unsigned int decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}
void STOP(){
  SetSpeed(0,0,0);
  SetSpeed(1,0,0);
}
//premenne na motory
int smer_motorov[4];
int poradie_motorov[4];
double gyro_plus[4];
double gyro_minus[4];
bool mot_syn_rozhodovac = false;
void pohyb(char smer, int rychlost){
  rozhodovac_smeru(smer);
  if(mot_syn_rozhodovac == false){
    SetSpeed(0,smer_motorov[0]*rychlost,smer_motorov[1]*rychlost);
    SetSpeed(1,smer_motorov[2]*rychlost,smer_motorov[3]*rychlost);
  }
  else{
    SetSpeed_syn(0,smer_motorov[0]*rychlost,smer_motorov[1]*rychlost);
    SetSpeed_syn(1,smer_motorov[2]*rychlost,smer_motorov[3]*rychlost);
  }
}
int d=1;
void pohyb_GYRO(char smer,int rychlost_max, int mot_config){
  unsigned long predosly_millis2;
  unsigned long previous_millis3;
  int dezrych_const=0;
  current_millis2 = millis();
  if(smer!=smer_old && rychlost2 > 0){
    if((current_millis2 - predosly_millis2)>=20){
      Serial.println("spomalovaine");
      dezrych_const=1;
      rychlost-=1;
      predosly_millis2 = current_millis2;
    }
    else{
      dezrych_const=0;
    }
  }
  if(dezrych_const==0){
      smer_old=smer;
  }
  else{
    smer=smer_old;
  }
  Setpoint = 0;
  //vstup = error_angle;
  vstup = (ReadGyro("z","angle"));
  gyro.Compute();
  //Serial.print("pid vystup, error: ");
 /* Serial.print(abs(vystup));
  Serial.print("  ");
  Serial.println(vstup);*/
  rozhodovac_smeru(smer);
  
  for(int i=0;i<4;i++){   //dosadenie nul do vsetkych prvkov pola
    gyro_plus[i] = 0;
    gyro_minus[i] = 0;
  }
  switch(mot_config){
    case 1:
      gyro_plus[poradie_motorov[1]-1] = abs(vystup);
      gyro_minus[poradie_motorov[0]-1] = abs(vystup);
      break;
    case 2:
      gyro_plus[poradie_motorov[1]-1] = abs(vystup);
      gyro_plus[poradie_motorov[3]-1] = abs(vystup);
      gyro_minus[poradie_motorov[0]-1] = abs(vystup);
      gyro_minus[poradie_motorov[2]-1] = abs(vystup);
      break;
  }
  //unsigned long predosly_millis = current_millis;
  current_millis = millis();
  //unsigned long dlzka_opakovania = current_millis - predosly_millis;
  //Serial.println(dlzka_opakovania);
   // musi byt vynulovane v pripade nedokoncenia akceleracie 
  GetPosition(0);
  GetPosition(1);
  Serial.println(rychlost);
  /*if(rychlost2==0 && rychlost>20 && zrych_const==0 || dlzka_opakovania>400 && zrych_const==1){
    Serial.println("xxxx");
    rychlost=20;
    d=1;
  }*/
  if(rychlost2 < rychlost_max && dezrych_const==0){
    zrych_const=1;
      if(rychlost2==0 && (current_millis - previous_millis3)>=2){
      rychlost+=2;
      previous_millis3 = current_millis;
      }
    if(rychlost2!=0 && (current_millis - previous_millis)>=20){
     // Serial.println(120);
      rychlost+=1;
      previous_millis = current_millis;
    }
  }
  if(zrych_const==1 || dezrych_const==1){
  if(vystup>0){
    SetSpeed(0,smer_motorov[0]*(rychlost+gyro_plus[0]),smer_motorov[1]*(rychlost+gyro_plus[1]));    //SetSpeed(0,motor 1, motor 2)
    SetSpeed(1,smer_motorov[2]*(rychlost+gyro_plus[3]),smer_motorov[3]*(rychlost+gyro_plus[2]));    //SetSpeed(1,motor 4, motor 3)
  }
  else{
    SetSpeed(0,smer_motorov[0]*(rychlost+gyro_minus[0]),smer_motorov[1]*(rychlost+gyro_minus[1]));
    SetSpeed(1,smer_motorov[2]*(rychlost+gyro_minus[3]),smer_motorov[3]*(rychlost+gyro_minus[2]));
    }
    v=rychlost2;
  }
  else{
    zrych_const=0;
   // d=0;
    if(vystup>0){
    ConstSpeed(0,smer_motorov[0]*rychlost,(v+gyro_plus[0]),smer_motorov[1]*rychlost,(v+gyro_plus[1]));
    ConstSpeed(1,smer_motorov[2]*rychlost,(v+gyro_plus[3]),smer_motorov[3]*rychlost,(v+gyro_plus[2]));
    }
    else{
      ConstSpeed(0,smer_motorov[0]*rychlost,(v+gyro_minus[0]),smer_motorov[1]*rychlost,(v+gyro_minus[1]));
      ConstSpeed(1,smer_motorov[2]*rychlost,(v+gyro_minus[3]),smer_motorov[3]*rychlost,(v+gyro_minus[2]));
    }
  }
}
void zmen_pole(double pole[], double a, double b, double c, double d){
  double zoznam[4] = {a,b,c,d};
  for(int i=0;i<4;i++){
    pole[i] = zoznam[i];
  }
}
void zmen_pole(int pole[], int a, int b, int c, int d){
  int zoznam[4] = {a,b,c,d};
  for(int i=0;i<4;i++){
    pole[i] = zoznam[i];
  }
}
/*
 * int smer_motorov_u[] = {1,-1,1,-1};
  int smer_motorov_d[] = {-1,1,-1,1};
  int smer_motorov_r[] = {1,1,-1,-1};
  int smer_motorov_l[] = {-1,-1,1,1};
  int smer_motorov_vsmere[] = {-1,-1,-1,-1};
  int smer_motorov_proti[] = {1,1,1,1};
 */
void rozhodovac_smeru(char smer){
    switch(smer){
    case 'u':
      zmen_pole(smer_motorov,1,-1,1,-1);
      zmen_pole(poradie_motorov,1,2,4,3);
      break;
    case 'd':
      zmen_pole(smer_motorov,-1,1,-1,1);
      zmen_pole(poradie_motorov,3,4,2,1);
      break;
    case 'r':
      zmen_pole(smer_motorov,1,1,-1,-1);
      zmen_pole(poradie_motorov,2,3,1,4);
      break;
    case 'l':
      zmen_pole(smer_motorov,-1,-1,1,1);
      zmen_pole(poradie_motorov,4,1,3,2);
      break;
    case '+':
      zmen_pole(smer_motorov,-1,-1,-1,-1);
      break;
    case '-':
      zmen_pole(smer_motorov,1,1,1,1);
      break;
  }
}
