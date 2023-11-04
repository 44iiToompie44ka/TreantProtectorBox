#include "GyverEncoder.h"
#include "interface.h"

#define CLK 3                                              // Пины энкодера
#define DT 2
#define SW 1
#define BINARY_ALGORITHM

//#define QUANTITY_SCREENS 5                               // количество экранов
//#define QUANTITY_LINES 7                                 // количество сторок

Encoder enc1(CLK, DT, SW);                                 //Объект энкодера
Menu menu_one;                                             //Объект меню

uint16_t pokazometr = 5,  brigh = 20, flagind;
uint8_t ledPin1 = 11, ledPin2 = 10;
bool flag = 1;

void isr() {     // Энкодер отработка в прерывании
  enc1.tick();
}

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  lcd.clear();
  lcd.init();
  lcd.backlight();
  attachInterrupt(0, isr, CHANGE);   //Прерывания на пинах энкодера
  attachInterrupt(1, isr, CHANGE);
  enc1.setType(TYPE2);               // Тип энкодера

  /*Формирование и заполнение непосредственно самого меню. Задаем названия экранов,
    пунктов меню и переменную для отображения в конце строки (если нужна предаем по адресу (&))
    Пример: menu_one.SetNames(uint8_t Номер экрана, uint8_t Номер строки, "Название пункта меню", & прееменная для отображения в конце строки (тип uint16_t));
  */
  menu_one.SetNames(0, 0, "First Screen");
  menu_one.SetNames(0, 1, "1_line Val", &pokazometr);
  menu_one.SetNames(0, 2, "2_line");
  menu_one.SetNames(0, 3, "3_line");

  menu_one.SetNames(1, 0, "Second Screen");
  menu_one.SetNames(1, 1, "1_line");
  menu_one.SetNames(1, 2, "2_line LED SW");
  menu_one.SetNames(1, 3, "3_line");
  menu_one.SetNames(1, 4, "4_line");

  menu_one.SetNames(2, 0, "Third Screen");
  menu_one.SetNames(2, 1, "1_line Bright", &brigh);

  menu_one.SetNames(3, 0, "Fourth screen");
  menu_one.SetNames(3, 1, "1_line LCD Light", &flagind);
  menu_one.SetNames(3, 2, "2_line");

  /*
    Привязка пользовательских функций к пунктам меню, максимум две
    Пример для привязки первой функции: menu_one.SetFunc1 (uint8_t Номер экрана, uint8_t Номер строки, название функции (void));
  */
  menu_one.SetFunc1 (0, 1, pok_pluss);  //Подключаем первую прикрепленную функцию (Нулевой экран, первая строка)
  menu_one.SetFunc2 (0, 1, pok_minuss); //Подключаем вторую прикрепленную функцию (Нулевой экран, вторая строка)

  menu_one.SetFunc1 (1, 2, LedOn);
  menu_one.SetFunc2 (1, 2, LedOff);

  menu_one.SetFunc1 (2, 1, brigh_up);
  menu_one.SetFunc2 (2, 1, brigh_down);

  menu_one.SetFunc1 (3, 1, LcdLightOn);
  menu_one.SetFunc2 (3, 1, LcdLightOff);


  menu_one.MakeMenu( 2, 2);             //Обновляем информацию на экране

}


void loop() {
  enc1.tick();
  if (enc1.isRight()) {                // Событие перемещения по списку строк вниз (следующая строка)
    menu_one.MakeMenu( 1, 2);
  };
  if (enc1.isLeft()) {                 // Событие перемещения по списку строк вверх (предыдущая строка)
    menu_one.MakeMenu( 0, 2);
  };
  if (enc1.isClick()) {                // Событие переключения следующего экрана меню
    menu_one.MakeMenu( 2, 1);
  }
  /* if ( ) { // Событие переключения предыдущего экрана меню    //Расскомментиовать и подстватить условие если нужно
     menu_one.MakeMenu( 2, 0);
    }*/

  /*Вызов прикрепленных  к строкам меню функций (правый нажатый поворот энкодера первая функция, левый - вторая)*/

  if (enc1.isRightH()) {               //Событие для прикрепленной функции 1 (ПРАВЫЙ нажатый поворот энкодера)
    menu_one.RunFunction1();
  };

  if (enc1.isLeftH()) {                //Событие для прикрепленной функции 2 (ЛЕВЫЙ нажатый поворот энкодера)
    menu_one.RunFunction2();
  };
}

//Примеры простейших внешних функций:

void LedOn () {                       //Включаем и отключаем синий светодиод
  digitalWrite(ledPin2, HIGH);
}
void LedOff () {
  digitalWrite(ledPin2, LOW);
}

void brigh_up () {                    //Меняем яркость красного светодиода
  analogWrite(ledPin1, brigh);
  brigh += 10;
  brigh = constrain (brigh,10, 240);
}
void brigh_down () {
  analogWrite(ledPin1, brigh);
  brigh -= 10;
  brigh = constrain (brigh, 10, 240);
}

void pok_pluss() {                   //Просто увеличиваем и уменньшаем значение переменной
  pokazometr++;
}
void pok_minuss() {
  pokazometr--;
}

void LcdLightOn() {                  //Включаем и отключаем подсветку экрана      
  flag = 1; 
  lcd.setBacklight(flag);
  flagind = flag;
}
void LcdLightOff() {
  flag = 0;
  lcd.setBacklight(flag);
  flagind = flag;
}
