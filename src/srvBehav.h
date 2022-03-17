#ifndef SRVBEHAV_H
#define SRVBEHAV_H

#include <QtGui>

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QAction>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QLabel>
#include <QDebug>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "pages.h"
#include "logobject.h"
#include "dtBehav.h"

class TServer_Dt : public QMainWindow
{
    Q_OBJECT

public:
  TServer_Dt(QWidget *parent = 0);
  ~TServer_Dt();

  void IniFileRead(void);
  void IniFileWrite(void);
  void setupUI(void);
  void setLogEnable(void);

protected:
//  void closeEvent(QCloseEvent *event); //Q_DECL_OVERRIDE;

private slots:
  void httpTicketLoad();
  void httpTicketDownloaded(QNetworkReply* pReply);

  void gpgDecode(       QByteArray );
  void inputNewExpo();
  void setDeviceExpo(  QStringList );
  void showCurrentExpo();
  void readCurrentSpectral();

  void onDeviceConnectionChanged(bool);
  void onDeviceListChanged();

  QStringList getCurrentExpo();

//  void iconActivated(QSystemTrayIcon::ActivationReason reason);
//  void setTrayIconActions();
//  void showTrayIcon();

  void getUI(void);
  void hideProg(void);
  void showProg(void);
  void quiteProg(void);

// private variables
private:
  QNetworkAccessManager m_WebCtrl;
  QString m_httpServiceUrl;
  bool  authOk;

  QComboBox *cmbxExs;
  QPushButton  *wBtn;

  QMessageBox message;
  //QTimer dev_timer;
  TDtBehav *device;
  LogObject *logSys;

//  QSystemTrayIcon *trayIcon;
//  QMenu   *trayIconMenu;
//  QAction *minimizeAction;
//  QAction *restoreAction;
//  QAction *quitAction;

// var for check box seting in pages UI
  bool logSrvScr,logSrv,logNWScr,logNW,logDTScr,logDT,logDTM;

  QString setingsFileName;
  int log_Size,log_Count; // size of log file and count zips archives get from settings
  QString loc; // localisation RU ENG Auto

//  int PORT;     // connection port
//  QString NAME; // request device name
  HWND HWIN;    //header of the window
  QByteArray bHWIN;
// main window
  QTabWidget *tabWidget;

// pages
  TPageLog *pageSrvD;
  TPageLog  *pageNWD;
  TPageLog  *pageDTD;
  TPageSetup   *pageSetup;

};

#endif // srvBehav_H
