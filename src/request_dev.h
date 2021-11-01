#ifndef REQUEST_DEV_H
#define REQUEST_DEV_H

//........................INFO_DATA.....................................................
#define INFO_DATA           "InfoData"
#define INFO_status         "status_dev"
#define INFO_rdev           "rdev"
#define INFO_Temperature    "Temperature_dev"
#define INFO_Levels         "Levels_dev"
#define INFO_fn             "fn"
#define INFO_time           "time"
#define INFO_isMeasuring    "isMeasuring"
#define INFO_pressRunButton "pressRunButton"
#define INFO_fmode          "fmode"

//.........................INFO_DEVICE....................................................
#define INFO_DEVICE         "InfoDevice"
#define INFODEV_version     "version"
#define INFODEV_serName     "serial_Name"
#define INFODEV_devMask     "fluor_Mask"
#define INFODEV_thermoBlock "type_ThermoBlock"
#define INFODEV_parameters  "dev_Parameters"
#define INFODEV_SpectralCoeff   "spectral_coeff"
#define INFODEV_OpticalCoeff    "optical_coeff"
#define INFODEV_UnequalCoeff    "unequal_coeff"

//.........................INFO_PROTOCOL..................................................
#define INFO_PROTOCOL       "InfoProtocol"
#define RUN_REQUEST         "RUN"
#define run_name            "run_name"
#define run_programm        "run_programm"
#define run_activechannel   "run_activechannel"
#define run_IDprotocol      "run_IDprotocol"
#define run_operator        "run_operator"

//........................OPENBLOCK_REQUEST................................................
#define OPENBLOCK_REQUEST    "OPEN_BLOCK"
#define CLOSEBLOCK_REQUEST   "CLOSE_BLOCK"
#define barcode_name         "barcode_name"

//........................STOP_REQUEST.....................................................
#define STOP_REQUEST        "STOP"
#define PAUSE_REQUEST       "PAUSE"
#define CONTINUE_REQUEST    "CONTINUE"

//........................MEASURE_REQUEST..................................................
#define MEASURE_REQUEST     "MEASURE"
#define MEASURE_Data        "MEASURE_Data"

//........................EXECCMD_REQUEST..................................................
#define EXECCMD_REQUEST     "EXECCMD"
#define EXECCMD_CMD         "cmd_string"
#define EXECCMD_UC          "uc_name"
#define EXECCMD_ANSWER      "answer"
#define EXECCMD_SENDER      "sender"

//........................PRERUN_REQUEST...................................................
#define PRERUN_REQUEST      "PRERUN"

//........................GETPIC_REQUEST...................................................
#define GETPIC_REQUEST      "GETPIC"
#define GETPIC_CHANNEL      "channel"
#define GETPIC_EXP          "exposition"
#define GETPIC_CTRL         "control"    //0 - get videodata 1 - get digitize data 2 - get all data
#define GETPIC_DATA         "GETPIC_Data"
#define GETPIC_VIDEO        "GETPIC_Video"

//........................SAVEPAR_REQUES.....................................................
#define SAVEPAR_REQUEST     "SAVEPAR"
#define SAVEPAR_CTRL        "control"  // 0 - save geometry, 1 - save exposition
#define SAVEPAR_DATA        "SAVEPAR_Data"

//..........................CONNECT_REQUEST..................................................
#define CONNECT_REQUEST     "ISCONNECTED"
#define CONNECT_STATUS      "status" // can be READY or BUSY

//..........................SECTORS REQUEST..................................................
#define SECTORREAD_REQUEST  "sector_read"
#define SECTORWRITE_REQUEST "sector_write"
#define SECTOR_DATA         "sector_data"
#define SECTOR_CMD          "sector_cmd"

//............................................................................................
#define ID_PARAM            "id_param"

//............................................................................................
#define DEVICE_REQUEST       "DEV_STATUS"
#define PRESS_BTN_RUN        "press_btn_run"

//...........................HIGH of tube measure.............................................
#define HTMEAS_REQUEST       "HIGHTUBEMEAS_REQUEST"
#define HIGHTUBE_SAVE        "HIGHTUBE_SAVE"

#define DTMASTERLOG_REQUEST  "DTMASTERLOG_REQUEST"
#define DTMASTERLOG_DATA     "DTMASTERLOG_DATA"

#endif // REQUEST_DEV_H

