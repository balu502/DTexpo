#ifndef DIGRES_H
#define DIGRES_H
#define REPCOM 3
#define MAX_OPTCHAN 8
//#define COUNT_CH 5
#define COUNT_CH 6
#define BYTES_IN_SECTOR 512            // counts of bytes in sector

#define COUNT_SIMPLE_MEASURE 2         // number of meas set 2!!!
#define LEFT_OFFSET 66          // offset for 96,384
#define LEFT_OFFSET_DT48 7      // offset for 48,192
#define W_IMAGE 752             // width image
#define H_IMAGE 292             // heigth image
#define COEF_IMAGE 1.93         //
#define W_IMAGE_COEF 390        // width image (width/coef: 752/1.93)
#define W_REALIMAGE 825         //
#define H_REALIMAGE 312         //
#define TOP_OFFSET 5
#define LEN 256*1023*2


#define BYTES_SHIFT_DATA  128            // shift from begin in a sector for analyse data in TMS470 vc
typedef struct Digitize_Result_96
{
   unsigned int sec_type;                //0-3
   unsigned int sec_sequence_number;     //4-7

   unsigned short int fn;                //8-9 number of meas
   unsigned short int type_meas;         //10-11 type of meas
   unsigned short int block_number;      //12-13 number of block in temp prog
   unsigned short int cycle_rep;         //14-15 number cycles in temp prog
   unsigned short int optical_channel;   //16-17 0 - Fam, 1 - Hex, 2 - Rox, 3 - Cy,
   unsigned short int exp_number;        //18-19 exposition 1 or 2
   unsigned short int blk_exp0;          //20-21 blk_level0 measured black level for exp0
   unsigned short int ovf_exp0;          //22-23 ovf0 overflow in exp0 measurement flag
   unsigned short int blk_exp1;          //24-25 blk_level1 measured black level for exp1
   unsigned short int ovf_exp1;          //26-27 overflow in exp1 measurement flag


   unsigned short int value_exp_0;        //28-29 value of 1 exposition
   unsigned short int value_exp_1;        //30-31 value of 2 exposition

//   char reserve[96];
   char reserve1[8];                      //32-39
   unsigned short int expo[2];            //40-43
   char reserve2[84];                     //44-127
   //values of exposition for ver 2.06 shift 32 for v3.03 110 optical measurement parameters (copy from par blk) 12 bytes dstorage.h
   unsigned short int datapoint_0[192];   //128-511 digitize data for first and second exposition

}DigitizeResult_96;

union Digitize_Result{
      Digitize_Result_96 DigitizeResult_96;
      char byte_buf[sizeof(Digitize_Result_96)];
};
typedef struct DigShortIntS
{
   unsigned short int sIntData;
}DigShortInt ;

union DigShortIntUn
{
   DigShortIntS DigShortInt;
   char byte_buf[sizeof(DigShortIntS)];
};

//_____ Структура параметров прибора ДТ-964

typedef struct Optics_Channel_96        // Структура хранения параметров канала оптики (12 байт)
{
  char filter;					         // 0-1 номер светофильтра
  char nexp;       				       // 1-2 количество экспозиций(0-2)
  unsigned char light;			     // 2-3 яркость прожектора
  char led;						           // 3-4 номер прожектора
  short int video_gain;			     // 4-6 усиление
  short int blk_level;			     // 6-8 уровень чёрного (нач. смещение)
  unsigned short int exp[2];	   // 8-12 2 экспозиции
}optics_ch[MAX_OPTCHAN];         // 12 bytes struct size

#define START_SECTOR_PARAMETERS 0x400 // номер сектора начала параметров  (1-96/48, 2-192, 4-384) TMS470
#define BYTES_SHIFT_RX  104            // shift from begin in a sector for RX
#define BYTES_SHIFT_RY  106            // shift from begin in a sector for RY
#define BYTES_SHIFT_XY  128            // shift from begin in a sector for XY coordinates of tubes (mask)
typedef struct Save_Par_96
{
  int sec_type;                               // 0-4
  int sec_sequence_number;                    // 4-8
  Optics_Channel_96  optics_ch[MAX_OPTCHAN];  // 8-104
  short int Rx;                               // 104-106
  short int Ry;                               // 106-108
  char d_xy[16];		                          // 108-124 смещения для 8-ми каналов по "X" и "Y"
  char reserve[4];                            // 124-128
  short int X_Y[96*2];                        // 128-512 лункм
}SavePar_96;

union Save_Par{
      Save_Par_96 SavePar_96;
      char byte_buf[sizeof(Save_Par_96)];
};

typedef struct Protocol_Sec0_96 {
  char version[24];
  char date[30];
  char num_protocol[10];
  char Operator[18];
  char comments[80];
  char program[350];
} ProtocolSec0_96;
union Protocol_Sec0{
      Protocol_Sec0_96 ProtocolSec0_96;
      char byte_buf[sizeof(Protocol_Sec0_96)];
};

//  Spectral coefficient correction
//-------------------------------------
struct Fluor_SpectralInfo				// 30 byte
{
  char fluor_name[12];
  short int id_group;
  short int coeff[MAX_OPTCHAN];		// коэфф. по 8-ми каналам
};
struct SpectralCoeff_Compensation   	// 512 byte
{
  char signature[30];               		// сигнатура ???
  short int count;							// кол-во красителей
  Fluor_SpectralInfo Info_Spectral[16];
};
union unionSpectralCoeff
{
  SpectralCoeff_Compensation SpectralCOEFF;
  unsigned char byte_buf[512];
};

//------------------------------------
struct OpticalCoeff_Compensation
{
  char signature[30];
  char fluor_name[12];
  short int coeff[192];
  char reserve[86];
};
union unionOpticalCoeff
{
  OpticalCoeff_Compensation OpticalCOEFF;
  unsigned char byte_buf[512];
};

//--------------------------------------
struct Fluor_UnequalInfo     	// 18 byte
{
  char fluor_name[12];
  short int id_group;
  short int coeff[2];
};
struct UnequalCoeff_Compensation   	// 512 byte
{
  char signature[30];               		// сигнатура ???
  short int count;							// кол-во красителей
  Fluor_UnequalInfo Info_Unequal[16];
  char reserve[192];
};
union unionUnequalCoeff
{
  UnequalCoeff_Compensation UnequalCOEFF;
  unsigned char byte_buf[512];
};
#endif
