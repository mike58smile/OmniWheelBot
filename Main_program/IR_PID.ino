void IR_pid_change(){
  if (irrecv.decode(&results)){
    if (results.value == 0XFFFFFFFF)
      results.value = key_value;
      switch(results.value){
        case 0xB1D174DE: //num1
        Kp2+=0.1;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        case 0x27D54AE8: //num 7
        Kp2-=0.1;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        case 0xC3EC6C6: //num 2
        Kd2+=0.001;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        case 0xFF848A58: //num 8
        Kd2-=0.001;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        case 0xF18640E8: //num 3
        Ki2+=0.01;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        case 0x2A4C917D: //num 9
        Ki2-=0.01;
        kam_platform.SetTunings(Kp2, Ki2, Kd2);
        delay(50);
        break;
        }
      key_value = results.value;
      irrecv.resume(); 
  }
}
