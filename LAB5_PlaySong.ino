#define BUZZER 14
#define BUTTON 23

hw_timer_t *My_timer = NULL;
hw_timer_t *My_timer2 = NULL;
bool isMute = true;
int speed = 2;
bool isChange = false;
bool pressed = false;

void IRAM_ATTR IO_INT_ISR()
{   
  if(digitalRead(BUTTON) == LOW && !pressed){
    pressed = true;
    mute(true);
    digitalWrite(BUZZER, LOW);
    speed = (speed+1)%5;
  }
}

// This is the Timer ISR. It is automatically called 
// on a timer alarm event.
void IRAM_ATTR onTimer(){
  if(isMute){
    digitalWrite(BUZZER, LOW);
  }else {
    digitalWrite(BUZZER, !digitalRead(BUZZER));
  }
}

void IRAM_ATTR onTimer2(){
  if(digitalRead(BUTTON) == HIGH && pressed ){
    pressed = false;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT);
  
  attachInterrupt(BUTTON, IO_INT_ISR, FALLING);
  
  My_timer = timerBegin(0, 80, true);
  My_timer2 = timerBegin(1, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true); // point to the ISR
  timerAttachInterrupt(My_timer2, &onTimer2, true);
  timerAlarmWrite(My_timer, 1000, true);
  timerAlarmWrite(My_timer2, 50000, true);
  timerAlarmEnable(My_timer);  // enable the alarm'
  timerAlarmEnable(My_timer2); 
}

void loop() {
  // playSong("D6(h),D6(q),C6(e),G5(e),D6(e),D6(e),E6(q),C6(q),A5(e),A5(e),D6(e),D6(e),E6(q),C6(q),C6(e),A5(e),B5(e),C6(e),B5(q),A5(q),E6(h),D6(q),D6(e),A5(e),E6(e),F6(e),E6(q),D6(q),D6(e),C6(e)");
playSong("D5(e),D5(e),D5(e),G5(h),D6(h),C6(e),B5(e),A5(e),G6(h),D6(q),C6(e),B5(e),A5(e),G6(h),D6(q),C6(e),B5(e),C6(e),A5(h),D5(q),D5(e),G5(h),D6(h),C6(e),B5(e),A5(e),G6(h),D6(q),C6(e),B5(e),A5(e),G6(h),D6(q),C6(e),B5(e),C6(e),A5(h),D5(q),D5(e),E5(q),E5(e),E5(e),C6(e),B5(e),A5(e),G5(e),G5(e),A5(e),B5(e),A5(q),E5(e),F#5(q),D5(q),D5(e)E,E5(q),E5(e),E5(e),C6(e),B5(e),A5(e),G5(e),D6(q),A5(h),D5(q),D5(e)");
}

void mute(bool b){
  isMute = b;
}

void playSong(String noteStream){
  while (noteStream.length() > 0)
  {
    int index = noteStream.indexOf(',');
    if (index == -1) // No space found
    {
      playNote(noteStream);
      break;
    }
    else
    {
      playNote(noteStream.substring(0, index));
      noteStream = noteStream.substring(index+1);
    }
  }
}

void playNote(String note){
  int index = note.indexOf('(');
  String key = note.substring(0, index);
  String beat = note.substring(index+1,note.length()-1);
  mute(false);
  int timerS = calFreq(key);
  timerAlarmWrite(My_timer, timerS, true);
  // My_timer = timerBegin(0, 80, true);  // use tiemr 0 and set prescale to 80 so 1 tick is 1 uSec
  // timerAlarmWrite(My_timer, timerS, true);
  // timerAttachInterrupt(My_timer, &onTimer, true); // point to the ISR
  // timerAlarmEnable(My_timer);  // enable the alarm
  double divisors[] = {1,2,4,8,16};
  double speedMulti[] = {4.5,2.5,1,0.5,0.25};
  String beats[] = {"w","h","q","e","s"};
  
  int divisor = 1;
  for(int i = 0; i < sizeof( beats ) / sizeof( String ); i++) {
    if(beats[i] == beat) divisor = divisors[i];
  }
  
  isChange = false;
  // int i1 = 0;
  // int time1 = ;
  delay((1300.0*speedMulti[speed])/divisor);
  // while (i1 < time1) {
  //   if(200 > time1-i1){
  //     delay(time1-i1);
  //     i1 += time1-i1;
  //   }else {
  //     i1 += 200;
  //     delay(200);
  //   }
  //   if(isChange == true) {
  //     Serial.println(speed);
  //     break;
  //   }
  // } 
  
  mute(true);
  
  // int i2 = 0;
  // int time2 = (1300.0*speedMulti[speed])/(divisor*1.5);
  delay((1300.0*speedMulti[speed])/(divisor*1.5));
  // while (i2 < time2) {
  //   if(200 > time2-i2){
  //     delay(time2-i2);
  //     i2 += time2-i2;
  //   }else {
  //     i2 += 200;
  //     delay(200);
  //   }
  //   if(isChange == true) {
  //     Serial.println(speed);
  //     break;
  //   }
  // } 
}

int calFreq(String key){
  double freqs[] = {16.35,17.32,18.35,19.45,20.60,21.83,23.12,24.50,25.96,27.50,29.14,30.87};
  String keys[] = {"C","C#","D","Eb","E","F","F#","G","G#","A","Bb","B"};
  int index = 0;

  for(int i = 0; i < sizeof( keys ) / sizeof( String ); i++) {
    if(keys[i] == key.substring(0,key.length()-1)) index = i;
  }

  return (int)(1000000/(2*(freqs[index]*pow(2,key.substring(key.length()-1,key.length()).toInt()))));
}
