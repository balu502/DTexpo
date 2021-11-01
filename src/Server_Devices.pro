
#-------------------------------------------------
#
# Project created by QtCreator 2016-02-04T15:59:10
#
#-------------------------------------------------

QT       += core gui
QT       += network xml

VERSION = 1.00.12
COMPATIBLE_VERSION = 7.30
DEFINES += MIN_VERSION=\\\"$$COMPATIBLE_VERSION\\\"

#version 1.00.00 begin
#version 1.00.01 change error processing
#1.00.05 new state machine processing + add Linguist
#1.00.07 change error processing model
#1.00.09 small error was removed
# press cover add, barcode read procedure in close cover procedure add.
#1.00.10 video online read function add
#1.00.11 add measure size of tube
#1.00.11 10.12.19 RSAV DRAV call change. Stay only for 96 and 384 type of device in
#USBCy_RW() was
#          if(answer.count() >= 3 && answer[2] == '>') { answer.remove(0,3); sts = true; if(answer[0]=='?') {sts=false; res=-4; break;}}
#          now
#          if(answer.count() >= 3 && answer[2] == '>') { answer.remove(0,3); sts = true; if(answer[0]=='?') {sts=false; res=-4; break;}}
#          else {
#             if(answer[0]=='?') {sts=false; res=-4; break;} else  sts = true;
#          }
# 23.01.20 was error read from device with chanel<Device chanel
# dtBehav.cpp
# add
#        active_ch=getActCh;  // 23.01.20
# 22.06.20 Program name in Russian was wrong on display
# change dtBehav.cpp
# was strncpy(protBuf.ProtocolSec0_96.num_protocol,map_Run.value(run_name).toAscii(),10);
# change on strncpy(protBuf.ProtocolSec0_96.num_protocol,map_Run.value(run_name).toLocal8Bit(),10);
# 24.09.2020
# New log processing. New release after error when write to log file.
# In logobject.cpp remove property QIODevice::WriteOnly in open function.
# In logMessage methode place while(log->isOpen())QApplication::processEvents(); and canWrite variable for
# right archive data

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include (config.pro)

TARGET = Server_Dev
TEMPLATE = app


LIBS += -lsetupapi \
        -lwinmm


SOURCES += main.cpp\
          CypressUsb.cpp \
          srvBehav.cpp \
          dtBehav.cpp \
          pages.cpp \
          logobject.cpp \
          CANChannels.cpp \
          progerror.cpp \
    qexpodialog.cpp

HEADERS +=CypressUsb.h \
          srvBehav.h \
          dtBehav.h \
          pages.h \
          logobject.h \
          progerror.h \
          can_ids.h \
          code_errors.h \
          CANChannels.h \
          digres.h \
          request_dev.h \
    qexpodialog.h
#          ../request_dev.h

RESOURCES += \
    graph.qrc

#DESTDIR = ../$${CURRENT_BUILD}

TRANSLATIONS +=Server_Dev_ru.ts



















