#ifndef CODE_ERRORS_H
#define CODE_ERRORS_H

namespace CODEERR{
//#define    C_NONE              0
//#define    C_INITIALISE_ERROR  1
//#define    C_USB_ERROR         2
//#define    C_OPEN_CAN_ERROR    3
//#define    C_RW_CAN_ERROR      4
//#define    C_USBBULK_ERROR     5
//#define    C_NOTREADY          6

// Error 16 bits word
// bits set 15-TFT, 14-Motor,13-Temp,12-Optical,11-USB if 1 then error
enum TCodeErrors
{
  NONE = 0,              // 0
  INITIALISE_ERROR,      // 1  ошибка инициализации (возможно в командной строке неверно указан номер прибора)
  USB_ERROR,             // 2  ошибка USB
  CAN_ERROR,             // 3  ошибка на CAN канале
  USBBULK_ERROR,         // 4  ошибка записи чтения bulk USB
  NOTREADY,              // 5  прибор не готов к работе, требуется подождать завершнния инициализации
  DEVHW_ERROR,           // 6  ошибка работы с аппаратурой внутри прибора (необходимо выключение прибора)
  OPENINRUN,             // 7  попытка открытия термоблока в момент выполнения температурной программы
  MOTORALARM,            // 8  ошибка привода
  STARTONOPEN,           // 9  попытка выполнения температурной программы при открытом термоблоке
  PRERUNFAULT_ERROR,     // 10 ошибка при попытке выполнить подготовку к запуску программы
  PRERUNSEMIFAULT_ERROR, // 11 была ошибка при попытке выполнить подготовку к запуску, но она была устранена
  DATAFRAME_ERROR,       // 12 ошибка в принятии блока данных, число полученных данных не соответствует ожидаемому
  LEDSSETUP_ERROR,       // 13 ошибка позиционирования колеса с фильтрами, колесо не провернулось
  MEASHT_ERROR,          // 14 ошибка измерения высоты пробирки
  UNKNOWN_ERROR          // 15
};

}

#endif // CODE_ERRORS_H
