
#define CRITICALVALUE  100

#define PIN_0 2
#define PIN_g 3
#define PIN_c 4
#define PIN_h 5
#define PIN_d 6
#define PIN_e 7
#define PIN_b 8
#define PIN_1 9
#define PIN_2 10
#define PIN_f 11
#define PIN_a 12
#define PIN_3 13

#define POS_NUM 4
#define SEG_NUM 8

#define ALLDARK 10

#define PHOTOCELLPIN  2
#define BUTTONPIN  1

const byte pos_pins[POS_NUM] = {PIN_0, PIN_1, PIN_2, PIN_3};
const byte seg_pins[SEG_NUM] = {PIN_a, PIN_b, PIN_c, PIN_d, PIN_e, PIN_f, PIN_g, PIN_h};


#define t true
#define f false
const boolean data[11][SEG_NUM] = {
  {t, t, t, t, t, t, f, f}, // 0
  {f, t, t, f, f, f, f, f}, // 1
  {t, t, f, t, t, f, t, f}, // 2
  {t, t, t, t, f, f, t, f}, // 3
  {f, t, t, f, f, t, t, f}, // 4
  {t, f, t, t, f, t, t, f}, // 5
  {t, f, t, t, t, t, t, f}, // 6
  {t, t, t, f, f, f, f, f}, // 7
  {t, t, t, t, t, t, t, f}, // 8
  {t, t, t, t, f, t, t, f}, // 9
  {f, f, f, f, f, f, f, f}, // all false
};

int* num;

void pf(const char *fmt, ... ){
    char tmp[128]; // max is 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(tmp, 128, fmt, args);
    va_end (args);
    Serial.print(tmp);
}

void setDigit(int pos, int n){
  if(pos < 0 || 3 < pos){
    pf("error pos=%d\n", pos);
    return;
  }
  
  for(int p = 0; p < POS_NUM; p++){
    if(p == pos)
      digitalWrite(pos_pins[p], LOW);
    else
      digitalWrite(pos_pins[p], HIGH);
  }
    
  if(0 <= n && n <= 10){
    for(int i = 0; i < SEG_NUM; i++){
      digitalWrite(seg_pins[i], data[n][i] == t ? HIGH : LOW);
    }
  }
  else{
    for(int i = 0; i < SEG_NUM; i++){
      digitalWrite(seg_pins[i], LOW);
    }
    digitalWrite(PIN_h, HIGH);
    pf("error pos=%d, n=%d\n", pos, n);
  }
}

void setNumber(int number)
{
  int n0, n1, n2, n3;
  n3 = number / 1000;
  number %= 1000;
  n2 = number / 100;
  number %= 100;
  n1 = number / 10;
  n0 = number % 10;
  
  setDigit(0, n0); delay(5);
  setDigit(1, n1); delay(5);
  setDigit(2, n2); delay(5);
  setDigit(3, n3); delay(5);
}

void setDarkZeroNumber(int number)
{
  int n0, n1, n2, n3;
  n3 = number / 1000;
  number %= 1000;
  n2 = number / 100;
  number %= 100;
  n1 = number / 10;
  n0 = number % 10;
  
  setDigit(0, n0==0?10:n0); delay(5);
  setDigit(1, n1); delay(5);
  setDigit(2, n2); delay(5);
  setDigit(3, n3==0?10:n3); delay(5);
}

unsigned long time_previous;
void setup() {
  Serial.begin(115200);
  pinMode(BUTTONPIN, INPUT);
  
  for(int i = 0; i < POS_NUM; i++){
    pinMode(pos_pins[i], OUTPUT);
    digitalWrite(pos_pins[i], HIGH);
  }
  for(int i = 0; i < SEG_NUM; i++){
    pinMode(seg_pins[i], OUTPUT);
    digitalWrite(seg_pins[i], LOW);
  }
  
  time_previous = millis();
}

int number = 0;
boolean flagPickNum=false;
int maskLightTimes=0;
void fourStartLotto(){
  if(flagPickNum){
    setNumber(number);
  }else{
    int photocellVal = analogRead(PHOTOCELLPIN);
    randomSeed(photocellVal);  
 
    if(photocellVal<CRITICALVALUE){
        Serial.println(photocellVal);   
        ++maskLightTimes; 
        if(maskLightTimes==50){
          flagPickNum=true;
          number=random(0000, 9999);
          return;
        }
        setNumber(random(1111, 9999));
        
    }
    else{
        if(maskLightTimes<=0) 
           setNumber(0);
        else
          setNumber(random(1111, 9999));
    }
    
  } 
}

#define BINGONUMCOUNT 80
#define RESULTCOUNT   10
int arrayBingoNum[BINGONUMCOUNT];
int arrayResult[RESULTCOUNT];
int displayIndex=0;

void bingoLotto(){
  
  if(flagPickNum){
    unsigned long time_now = millis();
    if(time_now - time_previous > 1000){
      time_previous += 1000;
      number=arrayResult[displayIndex%RESULTCOUNT];
      
      ++displayIndex;
      
    }
    setDarkZeroNumber(number*10);
   
    //delay(500);
    
  }else{
    int photocellVal = analogRead(PHOTOCELLPIN);
    
    if(photocellVal<CRITICALVALUE){
        Serial.println(photocellVal);   
        ++maskLightTimes; 
        if(maskLightTimes==30){
          flagPickNum=true;
          
          for(int i=0;i<BINGONUMCOUNT;i++){
            arrayBingoNum[i]=i+1;
          }
          
          for(int i=0;i<RESULTCOUNT;i++){
            arrayResult[i]=0;
          }
          
          for(int times=0;times<RESULTCOUNT;times++){
            randomSeed(random());
            int index=random(0,BINGONUMCOUNT-times);
            arrayResult[times]=arrayBingoNum[index];
            
            for(int pickIdx=index;pickIdx<BINGONUMCOUNT-1;pickIdx++){
              arrayBingoNum[pickIdx]=arrayBingoNum[pickIdx+1];
            }
            arrayBingoNum[BINGONUMCOUNT-1]=0;
          }
          for(int i=0;i<RESULTCOUNT;i++){
            pf("number %d =%d\n", i+1,arrayResult[i]);
          }
          
          return;
        }
        setNumber(random(1111, 9999));
        
    }
    else{
        if(maskLightTimes<=0) 
           setNumber(0);
        else
          setNumber(random(1111, 9999));
    }
    
  } 
    
}

void loop() {
  
  //Serial.println(10);   
  fourStartLotto();
   //bingoLotto();
}
