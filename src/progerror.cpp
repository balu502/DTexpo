#include "progerror.h"

//----------------------------------------------------------------------------------------
// Error processing on start program.
//----------------------------------------------------------------------------------------
TProgErrors::TProgErrors(QObject *parent) : QObject(parent)
{
  prog_errors=NONE;
}
TProgErrors::~TProgErrors()
{
}
QString TProgErrors::readProgTextError(void)
{
  switch(prog_errors)
  {
    case NONE: return tr("none");
    case NETWORK_ERROR: return tr("TCP/IP not work");
    case USB_CREATE_ERROR: return tr("USB error. FX2 device don't create.");
    case USB_CONNECT_ERROR: return tr("USB conection to device is absent.");
    case USB_DEVNAME_ERROR: return tr("Name of device don't corresponds of request.");
    default:
      return("");

  }
}

//----------------------------------------------------------------------------------------
// Error processing on working time of device
//----------------------------------------------------------------------------------------
TDevErrors::TDevErrors(QObject *parent) : QObject(parent)
{
  cntOptErr=cntAmpErr=cntMotErr=cntTftErr=cntBulkErr=cntNWErr=0;
  map=0;
  devErrorsText.clear();
  bitError.reset();
}
TDevErrors::~TDevErrors()
{
}
void TDevErrors::setDevError(DevErrors err)
{
  bitError.set((int)err);

  switch(err)
  {
    case USB_CONNECTION_ERROR:  break;
    case OPEN_OPT_ERROR: cntOptErr++ ; map|=0x1000; break;
    case OPEN_AMP_ERROR: cntAmpErr++;  map|=0x2000;break;
    case OPEN_MOT_ERROR: cntMotErr++;  map|=0x4000;break;
    case OPEN_TFT_ERROR: cntTftErr++;  map|=0x8000;break;
    case RW_OPT_ERROR:   cntOptErr++;  map|=0x1000; break;
    case RW_AMP_ERROR:   cntAmpErr++;  map|=0x2000;break;
    case RW_MOT_ERROR:   cntMotErr++;  map|=0x4000;break;
    case RW_TFT_ERROR:   cntTftErr++;  map|=0x8000;break;
    case TOUT_OPT_ERROR: cntOptErr++;  map|=0x1000; break;
    case TOUT_AMP_ERROR: cntAmpErr++;  map|=0x2000;break;
    case TOUT_MOT_ERROR: cntMotErr++;  map|=0x4000;break;
    case TOUT_TFT_ERROR: cntTftErr++;  map|=0x8000;break;
    case INCORRECT_CAN_DEVICE: ; break;
    case RW_USBBULK_ERROR: cntBulkErr++; map|=0x0800; break ;
    case GETFRAME_ERROR: cntNWErr++; break;
    default:;
  }
}

//-----------------------------------------------------------------------------
//---Return simple error for server answer
//-----------------------------------------------------------------------------
short int TDevErrors::analyseError(void)
{
  short int ret=::CODEERR::NONE;
  if(bitError.any()){
    for(int i=0;i<bitError.size();i++){
      if(bitError.test(i))
        switch(i)
        {
          case USB_CONNECTION_ERROR: ret=CODEERR::USB_ERROR;  break;
          case OPEN_OPT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case OPEN_AMP_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case OPEN_MOT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case OPEN_TFT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case RW_OPT_ERROR:    ret=CODEERR::CAN_ERROR; break;
          case RW_AMP_ERROR:    ret=CODEERR::CAN_ERROR; break;
          case RW_MOT_ERROR:    ret=CODEERR::CAN_ERROR; break;
          case RW_TFT_ERROR:    ret=CODEERR::CAN_ERROR; break;
          case TOUT_OPT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case TOUT_AMP_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case TOUT_MOT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case TOUT_TFT_ERROR:  ret=CODEERR::CAN_ERROR; break;
          case INCORRECT_CAN_DEVICE:  ret=::CODEERR::CAN_ERROR; break;
          case RW_USBBULK_ERROR: ret=CODEERR::USBBULK_ERROR; break ;
          case CMD_USBBULK_ERROR: ret=CODEERR::DEVHW_ERROR; break;
          case GETFRAME_ERROR: ret=CODEERR::DATAFRAME_ERROR; break;
          case OPENINRUN_ERROR: ret=CODEERR::OPENINRUN ; break;
          case MOTORALARM_ERROR: ret=CODEERR::MOTORALARM ; break;
          case STARTWITHOPENCOVER_ERROR: ret=::CODEERR::STARTONOPEN ; break;
          case PRERUNFAULT_ERROR:  ret=CODEERR::PRERUNFAULT_ERROR ; break; //set flag 9 bit for full fault
          case PRERUNSEMIFAULT_ERROR:  ret=CODEERR::PRERUNSEMIFAULT_ERROR ; break;
          case MOTORDRVLED_ERROR: ret=CODEERR::CAN_ERROR; break;
          case MEASHIGHTUBE_ERROR: ret=CODEERR::MEASHT_ERROR; break;
          default: if(ret==CODEERR::NONE) ret=CODEERR::UNKNOWN_ERROR;
        }
    }
  }
  ret|=(map&0xf800);
  return ret;
}

QStringList TDevErrors::readDevTextErrorList(void)
{
  devErrorsText.clear();
  if(bitError.any()){
    for(int i=0;i<bitError.size();i++)
      if(bitError.test(i)) {
        switch(i)
        {
          case USB_CONNECTION_ERROR: devErrorsText.append(tr("USB Connection error.")); break;
          case OPEN_OPT_ERROR: devErrorsText.append(tr("Open optical CAN channel error.")); break;
          case OPEN_AMP_ERROR: devErrorsText.append(tr("Open temperatur CAN channel error.")); break;
          case OPEN_MOT_ERROR: devErrorsText.append(tr("Open motor CAN channel error.")); break;
          case OPEN_TFT_ERROR: devErrorsText.append(tr("Open TFT CAN channel error. ")); break;
          case RW_OPT_ERROR:   devErrorsText.append(tr("Optical conrtoller R/W error.")); break;
          case RW_AMP_ERROR:   devErrorsText.append(tr("Temperature conrtoller R/W error.")); break;
          case RW_MOT_ERROR:   devErrorsText.append(tr("Motor conrtoller R/W error.")); break;
          case RW_TFT_ERROR:   devErrorsText.append(tr("TFT conrtoller R/W error.")); break;
          case TOUT_OPT_ERROR:   devErrorsText.append(tr("Optical conrtoller timeout error.")); break;
          case TOUT_AMP_ERROR:   devErrorsText.append(tr("Temperature conrtoller timeout error.")); break;
          case TOUT_MOT_ERROR:   devErrorsText.append(tr("Motor conrtoller timeout error.")); break;
          case TOUT_TFT_ERROR:   devErrorsText.append(tr("TFT conrtoller timeout error.")); break;
          case INCORRECT_CAN_DEVICE: devErrorsText.append(tr("Set incorrect can device.")); break;
          case RW_USBBULK_ERROR: devErrorsText.append(tr("Usb bulk R/W error.")); break;
          case CMD_USBBULK_ERROR: devErrorsText.append(tr("Device hardware internal error.")); break;
          case GETFRAME_ERROR: devErrorsText.append(tr("Get data from network don't correspond of requset.")); break;
          case OPENINRUN_ERROR: devErrorsText.append(tr("Can't open cover in RUN time.")); break;
          case MOTORALARM_ERROR: devErrorsText.append(tr("Motor device alarm.")); break;
          case STARTWITHOPENCOVER_ERROR: devErrorsText.append(tr("Close cover before start RUN.")); break;
          case PRERUNFAULT_ERROR: devErrorsText.append(tr("Found unremovable error before RUN.")); break;
          case PRERUNSEMIFAULT_ERROR: devErrorsText.append(tr("Found removable error before RUN.")); break;
          case PRERUNMEMTEST_ERROR: devErrorsText.append(tr("Memory test error befor RUN.")); break;
          case PRERUNANALYSE_ERROR:devErrorsText.append(tr("Tset get valid measure data error befor RUN."));  break;
          case MOTORDRVLED_ERROR: devErrorsText.append(tr("Found error in setup of LED filters.")); break;
          case MEASHIGHTUBE_ERROR: devErrorsText.append(tr("Found error in meas high of tube procedure.")); break;
          default:devErrorsText.append(tr("Unknown Error."));
        }
        devErrorsText.last().append(tr(" Error code ")+QString("%1").arg(i));
      }
  }
  return devErrorsText;
}
