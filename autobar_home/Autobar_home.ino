///////////////////////////////////////////////////////////////////////////////
//   Автобар                                                                 //
//   Nano + 4 Relay                                                          //
//   DIY-pragmatiс konakovskiy@gmail.com                                     //
//   2016-06-20                                                              //
//   Два мотора 12В 4 концевика на Герконах.                                 //
//   По нажатию кнопки моторы последовательно выдвигают каретку              //
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
boolean OPEN_READY = LOW; //флаг готовности к открытию
boolean CLOSE_READY = LOW; //флаг готовности к закрытию

boolean S1_S, S1_F, S2_S, S2_F;// переменные для хранения состояния датчиков

void setup() {
  pinMode (M1_1, OUTPUT);
  pinMode (M1_2, OUTPUT);
  pinMode (M2_1, OUTPUT);
  pinMode (M2_2, OUTPUT);
  pinMode (M1_S, INPUT_PULLUP);
  pinMode (M1_F, INPUT_PULLUP);
  pinMode (M2_S, INPUT_PULLUP);
  pinMode (M2_F, INPUT_PULLUP);
  pinMode (START_BTN, INPUT_PULLUP);
  reset ();
}

void loop() {
 readbutton (); //опрашиваем кнопку ПУСК
 startreadycheck (); //проверка состояния
 normalstart ();
 }

//предстартовая проверка состояния после подачи питания
void startreadycheck () { 
  sensors_stat(); //проверяем сенсоры
    if (S1_S && !S1_F && S2_S && !S2_F) OPEN_READY = HIGH; //если нормально - старт
    else OPEN_READY = LOW;
  } 
void startclosecheck (){
  sensors_stat(); //проверяем сенсоры
    if (!S1_S && S1_F && !S2_S && S2_F) CLOSE_READY = HIGH; //если нормально - старт
    else CLOSE_READY = LOW;
  }

void sensors_stat() {
  uint8_t s1_s = digitalRead(M1_S);
    if (s1_s) S1_S = LOW;
    else S1_S = HIGH; 
  uint8_t s1_f = digitalRead(M1_F);
    if (s1_f) S1_F = LOW;
    else S1_F = HIGH; 
 uint8_t s2_s = digitalRead(M2_S);
    if (s2_s) S2_S = LOW;
    else S2_S = HIGH; 
 uint8_t s2_f = digitalRead(M2_F);
    if (s2_f) S2_F = LOW;
    else S2_F = HIGH; 
}

void reset (){ //сброс в исходное состояние
  proc_close();
  START_BTN_PREV = LOW; // начальное и предыдущее состояние кнопки старта
  START_BTN_STAT = HIGH; // текущее состояние кнопки старта
  }

void normalstart () {
  if (!START_BTN_STAT && !START_BTN_PREV){
    startreadycheck ();
      if (OPEN_READY) proc_open();
      if (!OPEN_READY) proc_close();
  }
  if (!START_BTN_STAT && START_BTN_PREV){
    proc_close();
  }
} 

void readbutton (){
  boolean START_BTN_TMP = digitalRead (START_BTN);
    if (START_BTN_TMP == LOW) {
      delay (30);
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
  motor_1_f();
  delay (300);
  sensors_stat();
  if (S1_F) motor_2_f();
  else {
    motor_1_f(); //иначе опять мотор 1 гоним вперед до конца
    motor_2_f();  // и потом опять включаем 2 вперед
  }
  START_BTN_STAT = HIGH; //кнопка в исходное состояние
  START_BTN_PREV = HIGH; // запоминаем открытие
  }

void motor_1_f(){
  motor_1_on_f();  //мотор 1 на открытие вкл
  do sensors_stat();
  while (!S1_F); //пока не дойдет до финишного
  all_motors_off (); // все выключить для верности
}
void motor_2_f(){
  motor_2_on_f();  //мотор 2 на открытие вкл
  do sensors_stat(); //опрашиваем концевики
  while (!S2_F);   //пока не дойдет до финишного
  all_motors_off(); // все выключить
  }

void proc_close() { //процедура закрытия
  motor_2_r(); //мотор 2 назад
  delay (300);
  sensors_stat();
  if (S2_S) motor_1_r(); //если мотор 2 дошел до конца, мотор 1 назад
  else {
    motor_2_r(); //иначе опять мотор 2 гоним до конца
    motor_1_r();  // и потом опять включаем 1 назад
  }
  START_BTN_STAT = HIGH; //кнопка в исходное состояние
  START_BTN_PREV = LOW; // запоминаем закрытие
  }

void motor_2_r(){
  motor_2_on_r();  //мотор 2 на закрытие вкл
  do sensors_stat(); //опрашиваем концевики
  while (!S2_S);
  all_motors_off();
  }
void motor_1_r(){
  motor_1_on_r();  //мотор 1 на закрытие вкл
  do sensors_stat();
  while (!S1_S);
  all_motors_off (); // все выключить для верности
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
