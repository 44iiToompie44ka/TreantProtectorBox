#pragma once
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#ifndef QUANTITY_SCREENS                                               //Если не задано количество строк и экранов задаем по усолчанию 4 экрана по пять строк в каждом
#define QUANTITY_SCREENS 4
#endif
#ifndef QUANTITY_LINES
#define QUANTITY_LINES 5
#endif


class Menu {
  private:
    uint8_t const static Lines = (QUANTITY_LINES + 1);                   //Небольшой костыль, чтобы строка всегда заканчивалась нулевым символом ('\0') служит как разделитель
    String screen [QUANTITY_SCREENS][Lines];                             //Массив заголовков меню
    uint16_t focuce = 1, scr_num = 0, *indicator [QUANTITY_SCREENS][Lines]; //Переменные фокуса (активной строки), номера экрана, и массив указателей индикатора (изменяющееся значение в конце строки)
  public:
    void (*pointf1[QUANTITY_SCREENS][Lines]) ();                        //Массив указателей на первую прикрепленную функцию
    void (*pointf2[QUANTITY_SCREENS][Lines]) ();                        //Массив указателей на вторую прикрепленную функцию
    Menu() {
      for (uint8_t i = 0; i < QUANTITY_SCREENS; i++ ) {                 //Обнуляем все массивы
        for (uint8_t j = 0; j < Lines; j++ ) {
          screen [i][j] = '\0';
          indicator [i][j] = 0;
          pointf1 [i][j] = 0;
          pointf2 [i][j] = 0;
        }
      }
    }
    void SetNames (uint8_t s, uint8_t l, String n, uint16_t ind) {     //Устанавливаем номер экрана, строки, название пункта меню и переменную для отображения в конце строки
      screen [s][l] = n;
      indicator [s][l] = ind;
    }

    void SetNames (uint8_t s, uint8_t l, String n) {                   // То же только без переменной в конце строки
      screen [s][l] = n;
    }

    void SetFunc1(uint8_t s, uint8_t l, void *p) {                     //Устанавливаем первую прикрепленную функцию
      pointf1[s][l] = p;
    }
    void SetFunc2(uint8_t s, uint8_t l, void *p) {                     //Устанавливаем вторую прикрепленную функцию
      pointf2[s][l] = p;
    }
    uint8_t GetScreen() {                                               //Возврат номера экрана
      return (scr_num);
    }
    uint8_t GetString() {                                               //Возврат номера строки
      return (focuce);
    }
    void RunFunction1() {                                               //Запускаем первую прикрепленную функцию (если она есть)
      if ((pointf1[GetScreen()][GetString()]) != 0) {
        pointf1[GetScreen()][GetString()]();
        MakeMenu( 2, 2);
      }
    }
    void RunFunction2() {                                               //Запускаем вторую прикрепленную функцию (если она есть)
      if ((pointf2[GetScreen()][GetString()]) != 0) {
        pointf2[GetScreen()][GetString()]();
        MakeMenu( 2, 2);
      }
    }

    /*
      Отрисовка меню на экране, обработка внутренней логики работы меню. Функция MakeMenu принимает два параметра (f, s), первый отвечает за переключения строки находящейся в фокусе
      (активной строки)второй за перелючения текущего экрана (1 - преключение на следующий экран/строку, 0 - предыдущий экран/стоку,2 - обновление текущего экрана без изменений
      Пример: MakeMenu (1, 2); - переход к следующей строке, MakeMenu (0, 2); - переход к предыдущей строке
      MakeMenu (2, 1); - переход к следующему экрану, MakeMenu (2, 1); - переход к предыдущему экрану.
    */
    void MakeMenu (uint8_t f, uint8_t s) {                       //Принимаем и смотрим параметр f, согласно его значению преключаем или прееменную focuce (активную строку)
      if (f == 0) {
        focuce--;
      } else if (f == 1) {
        focuce++;
      }
      focuce = constrain (focuce, 1, (Lines - 1) );              //Ограничиваем ее, от греха, смотрим на переменную s и то же самое только с номером экрана, или переключаем или не трогаем
      if (s == 0) {
        scr_num--;
      } else if (s == 1) {
        scr_num++;
      }
      if (scr_num > QUANTITY_SCREENS ) {                         //проверяем не ушли ли мы за размеры массива, если ушли возвращаемся к нулевому элементу
        scr_num = 0;
      }
      if (screen [scr_num][0] == '\0' ) {                        //Нулевой символ служит сигналом о конце списка строк/экранов
        scr_num = 0;
        MakeMenu( 2, 2);                                         //Обновляем картинку на экране
      }
      if (screen [scr_num][focuce] == '\0' ) {
        focuce--;
        MakeMenu( 2, 2);
      }
      if (focuce < 1) focuce = 1;

      lcd.clear();                                               //Очищаем экран, пишем название экрана меню, в правом верхнем углу экрана порядковый номер активной строки
      lcd.setCursor(0, 0);
      lcd.print (screen [scr_num][0]);
      lcd.setCursor(19, 0);
      lcd.print (focuce);
      lcd.setCursor(0, 1);
      lcd.print (">");
      uint8_t i = 1;
      uint8_t p = 1;
      for (i = focuce; i < (focuce + 3) ; i++) {                //Выводим на экран строки по порядку пока не упремся в нулевой символ
        lcd.setCursor(1, p);
        lcd.print (screen [scr_num][i]);
        if (*indicator [scr_num][i] != 0) {
          lcd.setCursor(17, p);
          lcd.print ((uint16_t)*indicator [scr_num][i]);
        };
        if (screen [scr_num][i + 1] == '\0')
          break;
        p++;
      }
    }
};
