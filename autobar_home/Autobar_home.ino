//////////////////////////////////////////////
//   Автобар                                                                 //
//   Nano + 4 Relay                                                          //
//   DIY-pragmatiс konakovskiy@gmail.com                                     //
//   2016-06-20                                                              //
//   Два мотора 12В 4 концевика на датчиках                                  //
//   Холла. По нажатию кнопки моторы последовательно выдвигают каретку       //
//   Повтороное нажатие - каретка задвигается. Состояние кнопки запоминать   //
///////////////////////////////////////////////////////////////////////////////

//#include <EEPROM.h>
int M1_1 = 4; //Реле 1 на мотор 1
int M1_2 = 5; //Реле 2 на мотор 1
int M2_1 = 6; //Реле 3 на мотор 2
int M2_2 = 7; //Реле 4 на мотор 2
int M1_S = A0; //Датчик стартовый на мотор 1
int M1_F = A1; //Датчик финишный на мотор 1
int M2_S = A2; //Датчик стартовый на мотор 2
int M2_F = A3; //Датчик финишный на мотор 2
int START_BTN = 3; // кнопка старта
boolean START_BTN_PREV = LOW; // начальное и предыдущее состояние кнопки старта
boolean START_BTN_STAT = HIGH; // текущее состояние кнопки старта
boolean START_READY = LOW; //флаг готовности к старту
boolean S1_S, S1_F, S2_S, S2_F;// переменные для хранения состояния датчиков

void setup() {
  pinMode (M1_1, OUTPUT);
  pinMode (M1_2, OUTPUT);
  pinMode (M2_1, OUTPUT);
  pinMode (M2_2, OUTPUT);
  pinMode (M1_S, INPUT);
  pinMode (M1_F, INPUT);
  pinMode (M2_S, INPUT);
  pinMode (M2_F, INPUT);
  pinMode (START_BTN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
 readbutton (); //опрашиваем кнопку ПУСК
 startreadycheck (); //проверка состояния
 if (START_READY)  normalstart ();
 if (!START_READY)  reset ();
 }

void sensors_stat() {
 uint8_t s1_s = analogRead(M1_S);
  if ((s1_s < 370) || (s1_s > 630)) S1_S = HIGH;
  else S1_S = LOW; 
  Serial.print(s1_s);
  Serial.print("S1_S = ");
  Serial.println(S1_S);
uint8_t s1_f = analogRead(M1_F);
  if ((s1_f < 370) || (s1_f > 630)) S1_F = HIGH;
  else S1_F = LOW; 
  Serial.print(s1_f);
  Serial.print("S1_F = ");
  Serial.println(S1_F);
uint8_t s2_s = analogRead(M2_S);
  if ((s2_s < 370) || (s2_s > 630)) S2_S = HIGH;
  else S2_S = LOW; 
  Serial.print(s2_s);
  Serial.print("S2_S = ");
  Serial.println(S2_S);
uint8_t s2_f = analogRead(M2_F);
  if ((s2_f < 370) || (s2_f > 630)) S2_F = HIGH;
  else S2_F = LOW; 
  Serial.print(s2_f);
  Serial.print("S2_F = ");
  Serial.println(S2_F);
  S1_S = 1;
  S1_F = 0;
  S2_S = 1;
  S2_F = 0;
  }

//предстартовая проверка состояния после подачи питания
void startreadycheck () { 
  sensors_stat(); //проверяем сенсоры
    if (S1_S && !S1_F && S2_S && !S2_F) START_READY = HIGH; //если нормально - старт
    else START_READY = LOW;
  } 

void reset (){ //сброс в исходное состояние
  proc_close();
  START_BTN_PREV = LOW; // начальное и предыдущее состояние кнопки старта
  START_BTN_STAT = HIGH; // текущее состояние кнопки старта
  }

void normalstart () {
  if (!START_BTN_STAT && !START_BTN_PREV){
    proc_open();
  }
  if (!START_BTN_STAT && START_BTN_PREV){
    proc_close();
  }
} 

void readbutton (){
  boolean START_BTN_TMP = digitalRead (START_BTN);
    if (START_BTN_TMP == LOW) {
      delay (50);
      START_BTN_STAT = digitalRead (START_BTN);
        if (!START_BTN_TMP && !START_BTN_STAT) {
        START_BTN_STAT = LOW;
        }
        else {
          START_BTN_STAT = HIGH; 
        } 
     }
  }

void proc_open() {  //процедура открытия
  motor_1_on_f();
  delay (1000);
  motor_1_off();
  delay (1000);
  motor_2_on_f();
  delay (1000);
  motor_2_off();
  all_motors_off (); // все ввыключить для верности
  START_BTN_STAT = HIGH; //кнопка в исходное состояние
  START_BTN_PREV = HIGH; // запоминаем открытие
  
  }

void proc_close() { //процедура закрытия
  motor_2_on_r();
  delay (1000);
  motor_2_off();
  delay (1000);
  motor_1_on_r();
  delay (1000);
  motor_1_off();
  all_motors_off (); // все ввыключить для верности
  START_BTN_STAT = HIGH; //кнопка в исходное состояние
  START_BTN_PREV = LOW; // запоминаем закрытие
  }

void all_motors_off () {  //все моторы выключены
  digitalWrite (M1_1, LOW);
  digitalWrite (M1_2, LOW);
  digitalWrite (M2_1, LOW);
  digitalWrite (M2_2, LOW);  
  }

void motor_1_off() {       //первый мотор выключен
  digitalWrite (M1_1, LOW);
  digitalWrite (M1_2, LOW);  
  }

void motor_1_on_f() {      //первый мотор включен вперед
  digitalWrite (M1_1, HIGH);
  digitalWrite (M1_2, LOW); 
  digitalWrite (M2_1, LOW); //в это время второй гаррантированно выключен
  digitalWrite (M2_2, LOW);   
  }  

void motor_1_on_r() {      //первый мотор вкючен назад
  digitalWrite (M1_1, LOW);
  digitalWrite (M1_2, HIGH);
  digitalWrite (M2_1, LOW); //в это время второй гаррантированно выключен
  digitalWrite (M2_2, LOW);     
  }  

void motor_2_off() {       //первый мотор выключен
  digitalWrite (M2_1, LOW);
  digitalWrite (M2_2, LOW); 
  }

void motor_2_on_f() {      //первый мотор включен вперед
  digitalWrite (M2_1, HIGH);
  digitalWrite (M2_2, LOW);
  digitalWrite (M1_1, LOW); //в это время первый гаррантированно выключен
  digitalWrite (M1_2, LOW);    
  }  

void motor_2_on_r() {      //первый мотор вкючен назад
  digitalWrite (M2_1, LOW);
  digitalWrite (M2_2, HIGH);
  digitalWrite (M1_1, LOW); //в это время первый гаррантированно выключен
  digitalWrite (M1_2, LOW);      
}      
