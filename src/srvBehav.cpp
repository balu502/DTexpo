#include <QThread>
#include <w32api.h>
#define WINVER WindowsXP
#define ES_AWAYMODE_REQUIRED 0x00000040
//#include <windows.h>
//#include <winbase.h>
#include "srvBehav.h"
#include "qexpodialog.h"

#include <QVBoxLayout>
#include <QDomDocument>
#include <QDataStream>

QString get_link_url(QString fn);

//-----------------------------------------------------------------------------
//--- Constructor
//-----------------------------------------------------------------------------
//TServer_Dt::TServer_Dt(QString dName,int nPort, QWidget *parent)
//    : QMainWindow(parent)

TServer_Dt::TServer_Dt(QWidget *parent) : QMainWindow(parent)
{
// Stay for example. On windows platfor don't work becouse change LIB environment variable but windows
// work with PATCH variable
// QCoreApplication::addLibraryPath(QString("C:/DtXml"));
// QCoreApplication::addLibraryPath(QString("C:/Program1/QtSDK/Desktop/Qt/4.7.4/mingw/plugins"));
// qDebug()<<QCoreApplication::libraryPaths();
// qputenv("PATH", "C:\\DtXml\\"); // change PATH in issue proceses!!!
  QString compilationTime = QString("%1 %2").arg(__DATE__).arg(__TIME__);
 // qDebug()<<compilationTime;

//  PORT = nPort;
//  NAME = dName;
  HWIN=(HWND)this->winId(); // set window ID
//  bHWIN=QString(" WinID=%1 ").arg((int)HWIN,0,16).toAscii();
  bHWIN=QString(" WinID=%1 ").arg((int)HWIN,0,16).toLatin1();
  device=0;
  //sHWIN=QString("%1").arg(HWIN);
  logSys=new LogObject("system"); logSys->setLogEnable(false,true); // enable system log into file only
  logSys->logMessage(tr(bHWIN+"Start Dt-server. Ver. "+APP_VERSION)+" "+compilationTime);
//  logSys->logMessage(bHWIN+"Device name "+NAME+QString(" Port=%1").arg(PORT));
//  setTrayIconActions();
//  showTrayIcon();

  //Qt::WindowFlags flags = Qt::Window |  Qt::CustomizeWindowHint| Qt::WindowSystemMenuHint |
  //                        Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint;
//  Qt::WindowFlags flags = Qt::Window
//          | Qt::WindowSystemMenuHint
//          | Qt::WindowCloseButtonHint
//          | Qt::WindowStaysOnTopHint;
//  setWindowFlags(flags);


// read settings
  QString exeDir = qApp->applicationDirPath();
  setingsFileName= exeDir+"/setup.ini" ;
  IniFileRead();

// create pages with log and settings
  pageSrvD= new TPageLog;
  pageNWD=  new TPageLog;
  pageDTD=  new TPageLog;
  pageSetup=new TPageSetup;

// create tab widget
  tabWidget=new QTabWidget;
  tabWidget->addTab(pageSrvD, tr("Show system diagnostics"));
//  tabWidget->addTab(pageNWD, tr("Show network diagnostics"));
  tabWidget->addTab(pageDTD, tr("Show DT-XX diagnostics"));
//  tabWidget->addTab(pageSetup, tr("Setup "));


  QWidget       *mw      = new QWidget;
  QWidget       *ew      = new QWidget;
  QVBoxLayout   *vLayout = new QVBoxLayout;
  QHBoxLayout   *eLayout = new QHBoxLayout;
  QPushButton   *oBtn    = new QPushButton("Optic");
  QPushButton   *cBtn    = new QPushButton("Read");
                 wBtn    = new QPushButton("Write");
                 cmbxExs = new QComboBox;

  wBtn->setEnabled(false);
  cBtn->setEnabled(false);
  cBtn->setEnabled(false);
  cBtn->setToolTip(tr("Read Expo from Device"));

  eLayout->addWidget( cmbxExs,  70 );
  eLayout->addWidget( wBtn,     10 );
  eLayout->addWidget( cBtn,     10 );
  eLayout->addWidget( oBtn,     10 );

  ew->setLayout( eLayout );

  vLayout->addWidget( ew );
  vLayout->addWidget( tabWidget );

  mw->setLayout( vLayout );
  setCentralWidget( mw );

  tabWidget->setCurrentIndex(0);

  setupUI();

  QString efn = exeDir +"/expo.txt";
  QString sfn = exeDir +"/service.txt";

  if ( QFile( sfn ).exists() )
      m_httpServiceUrl = get_link_url( sfn );

  if (m_httpServiceUrl.isEmpty())
      m_httpServiceUrl = "http://192.168.0.167:8080/dtxp/ticket.file";
//    m_httpServiceUrl = "http://service.dna-technology.ru/dtxp/ticket.file";
//    m_httpServiceUrl = "http://127.0.0.1/dtxp/ticket.file";
//    m_httpServiceUrl = "http://172.16.0.1:8080/dtxp/ticket.file";


  connect(wBtn, SIGNAL(clicked(bool)), this, SLOT(inputNewExpo()));
  connect(cBtn, SIGNAL(clicked(bool)), this, SLOT(showCurrentExpo()));
  connect(oBtn, SIGNAL(clicked(bool)), this, SLOT(readCurrentSpectral()));
  connect( &m_WebCtrl, SIGNAL(finished(QNetworkReply*))
                        , this, SLOT(httpTicketDownloaded(QNetworkReply*)));

  if( !(authOk = QFile( efn ).exists()) )
    httpTicketLoad();

  device = new TDtBehav(HWIN,log_Size,log_Count);

  if( device->readGlobalErr() ){ //connect to network error. Program abort
    logSys->logMessage(bHWIN+device->readGlobalErrText());
    QTimer::singleShot(1000,this,SLOT(close()));
    return;
  }

  setLogEnable();
  cmbxExs->addItem( "Connecting..." );

//  connect(device,SIGNAL(commError()),this,SLOT(quiteProg()));
// signals for Log pages
  connect(device->logSrv,SIGNAL(logScrMessage(QString)),pageSrvD,SLOT(addToLog(QString)));
  connect(device->logNw,SIGNAL(logScrMessage(QString)),pageNWD,SLOT(addToLog(QString)));
  connect(device->logDev,SIGNAL(logScrMessage(QString)),pageDTD,SLOT(addToLog(QString)));
  connect(pageNWD,SIGNAL(pauseChange(bool)),pageSetup,SLOT(setCbLogNWS(bool)));

  connect(device, SIGNAL(onDeviceInfoAvailable(bool)), cBtn, SLOT(setEnabled(bool)));
  connect(device, SIGNAL(onDeviceInfoAvailable(bool)), oBtn, SLOT(setEnabled(bool)));
  connect(device, SIGNAL(onDeviceInfoAvailable(bool)), this, SLOT(onDeviceConnectionChanged(bool)));
  connect(device, SIGNAL(onDeviceListAvailable()), this, SLOT(onDeviceListChanged()));

  connect(pageDTD,SIGNAL(pauseChange(bool)),pageSetup,SLOT(setCbLogDTS(bool)));
// signal on change data in setup page
  connect(pageSetup,SIGNAL(sigChangeUI()),this,SLOT(getUI())); //change combo box on page settings
  //device->start(QThread::NormalPriority);
  setMinimumWidth(500);
  resize(500,330);
  move(50, 50);
  SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED);
  device->logSrv->logMessage(tr("Start Dt-server. Ver. ")+APP_VERSION+" "+compilationTime);
  device->start(QThread::NormalPriority);

}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------

TServer_Dt::~TServer_Dt()
{
  qDebug()<<"Server destructor start.";
  if(device) delete device;
  delete tabWidget;
//  delete trayIcon;

  logSys->logMessage(tr(bHWIN+"Stop Dt-server."));
  if(logSys) delete logSys;
  qDebug()<<"Server destructor finished.";
}

void TServer_Dt::onDeviceConnectionChanged(bool s){

    wBtn->setEnabled(s);
    cmbxExs->clear();

    if (s) {
        cmbxExs->addItem( QString("%1 is online").arg( device->getDevName() ) );
        setWindowTitle( device->getDevName() );
    }else{
        cmbxExs->addItem( "Device disconnected" );
    }
}


void TServer_Dt::onDeviceListChanged(){

    bool ok;
    QStringList availableDevices =
                    device->availableDevices();

    if (availableDevices.empty())
        return;

    if (1 == availableDevices.count()){
        device->selectDevice( availableDevices.first() );
        return;
    }

    QString devName = QInputDialog::getItem(
                            this, this->windowTitle()
                            , "Choose device", availableDevices
                            , 0, false, &ok );

    if ( !ok ) devName = availableDevices.first();

    device->selectDevice( devName );
}


void TServer_Dt::httpTicketLoad(){

    cmbxExs->clear();
    cmbxExs->addItem( "Waiting for network..." );
    m_WebCtrl.get(
        QNetworkRequest(QUrl(m_httpServiceUrl)));
}

typedef struct
{
    char    id_default[  8];             // "default"
    short   Expo_def  [ 16];             //
    char    reserve   [472];

} Device_ParDefault;


QStringList processReadSector(QString sdata){

    QByteArray       buf_default;
    QString          str_default;
    QStringList      retExp;
    short            ex;

    union{
        Device_ParDefault   Device_ParamDefault;
        unsigned char       byte_buf[ 512 ];
    } DEVICE_PAR_DEFAULT;


    if ( sdata.isEmpty() )
        return  retExp;

    buf_default = QByteArray::fromBase64(
                        sdata.toStdString().c_str() );

    if ( buf_default.size() != 512 )
        return  retExp;

    for( int i=0; i<512; i++ )
        DEVICE_PAR_DEFAULT.byte_buf[i]
                            = buf_default.at(i);
    str_default = QString(
        DEVICE_PAR_DEFAULT.Device_ParamDefault.id_default );

    if ( !str_default.contains("default") )
        return  retExp;


    for ( int j=0; j<COUNT_CH*2; j++ ){
        ex = DEVICE_PAR_DEFAULT.Device_ParamDefault.Expo_def[ j ];

        if (0 == j%2)
            retExp << QString("%1").arg( ex );
    }

    return  retExp;
}


void TServer_Dt::gpgDecode(QByteArray dataFile){

    authOk = false;

    QProcess gpgProc;
    QString  phrase = QInputDialog::getText(
                        this, this->windowTitle()
                            , "Enter code", QLineEdit::Password );

    gpgProc.start( "gpg", QStringList()
        << "--batch" << "--passphrase" << phrase << "--" );

    if (!gpgProc.waitForStarted()){
        cmbxExs->addItem( "Err: gpg start" );
        return;
    }

    gpgProc.write(dataFile);
    gpgProc.closeWriteChannel();

    if (!gpgProc.waitForFinished()){
        cmbxExs->addItem( "Err: gpg result" );
        return;
    }

    if (QProcess::NormalExit != gpgProc.exitCode()){
        cmbxExs->addItem( "Err: gpg exit" );
        return;
    }

    QByteArray result = gpgProc.readAll();
    cmbxExs->addItem( QString("Out: %1").arg( result.length() ) );

    //qDebug()<<"Ticket:"<<result;
    authOk = true;
}


QStringList TServer_Dt::getCurrentExpo(){

    QByteArray  buf;
    QString     text;
    QString     ctrl = QString("PRDS %1").arg(0x0418);

     buf.resize(BYTES_IN_SECTOR);
    text.resize(BYTES_IN_SECTOR);

    if (!device->readFromUSB512( ctrl, "0", (unsigned char*)buf.data())){
        cmbxExs->addItem( "Err: current read" );
        return  QStringList();
    }

    text =  buf.toBase64();
            buf.clear();

    return  processReadSector(text);
}


void TServer_Dt::showCurrentExpo(){

    QStringList currEx = getCurrentExpo();

    QMessageBox::information(this, this->windowTitle(),
        QString("Current expo: [%1].\n")
            .arg( currEx.isEmpty()
                    ? "unavailable" : currEx.join("; ") ) );
}


void TServer_Dt::readCurrentSpectral(){

    QMessageBox::information(this, this->windowTitle(),
        QString("Spectral read \n'%1' ")
            .arg(device->infoDevice( INFODEV_SpectralCoeff )));

    return;
}


void TServer_Dt::inputNewExpo(){

    if (!authOk){
        cmbxExs->clear();
        cmbxExs->addItem( QString("Err: auth") );

        httpTicketLoad();
        return;
    }

    QStringList chLines;
    QStringList currEx = getCurrentExpo();

    qDebug() << "Current expositions:\n"
             << currEx.join("; ");

    QExpoDialog *ed = new QExpoDialog(this);

    bool inOk;


    ed->setValues(currEx.join(";"));
    QString exp = ed->values("", &inOk);

    if ( !inOk ) return;

    chLines = exp.split(";", QString::SkipEmptyParts);

    cmbxExs->addItem(
            QString("Chs: %1").arg( exp ) );

    if (QMessageBox::No
    ==  QMessageBox::question(this, this->windowTitle(),
            tr("%1Continue writing expo [%2] into device?")
                .arg( QString("Current expo: [%1].\n")
                        .arg(currEx.isEmpty()?"unavailable":currEx.join("; ") ) )
                .arg( chLines.join("; ") ),
            QMessageBox::Yes|QMessageBox::No, QMessageBox::No ))
        return;

    setDeviceExpo( chLines );
}


void TServer_Dt::setDeviceExpo( QStringList strExp ){

    QString ctrl = QString("PWRS %1").arg(0x0418);

    union{  Device_ParDefault   Device_ParamDefault;
            unsigned char       byte_buf[ 512 ];
    }   DEVICE_PAR_DEFAULT;

    for( int i=0; i<512; i++ )
        DEVICE_PAR_DEFAULT.byte_buf[i] = 0;

    strcpy( DEVICE_PAR_DEFAULT.Device_ParamDefault.id_default, "default" );
//    strcpy( DEVICE_PAR_DEFAULT.Device_ParamDefault.id_default, "undeflt" );

    short   ex;
    for(int i=0; i < COUNT_CH; i++){
        ex = ( i >= strExp.length() ) ? 0 : strExp[i].trimmed().toUShort();
        DEVICE_PAR_DEFAULT.Device_ParamDefault.Expo_def[2*i+0] = ex;
        DEVICE_PAR_DEFAULT.Device_ParamDefault.Expo_def[2*i+1] = ex/5;
    }

    if (!device->writeIntoUSB512( ctrl, "0", DEVICE_PAR_DEFAULT.byte_buf )){
        cmbxExs->addItem( "Err: data write" );
        return;
    }

    cmbxExs->addItem( "Write: ok" );
    showCurrentExpo();
}


void TServer_Dt::httpTicketDownloaded(QNetworkReply *pReply){

    QByteArray dataRep = pReply->readAll();
    gpgDecode( dataRep );
}


//-----------------------------------------------------------------------------
//--- Read ini settings
//-----------------------------------------------------------------------------
void TServer_Dt::IniFileRead(void)
{
  QSettings setings(setingsFileName,QSettings::IniFormat) ;
  logSrvScr=setings.value("logSystemScr",true).toBool();
  logSrv=setings.value("logSystem",true).toBool();
  logNWScr=setings.value("logNWScr",true).toBool();
  logNW=setings.value("logNW",true).toBool();
  logDTScr=setings.value("logDTScr",true).toBool();
  logDT=setings.value("logDT",true).toBool();
  logDTM=setings.value("logDTMASTER",false).toBool();
  log_Size=setings.value("logFileSize",100000).toInt();
  log_Count=setings.value("logFileCount",10).toInt();
  loc=setings.value("Locale","EN").toString();
}

//-----------------------------------------------------------------------------
//--- Write ini settings
//-----------------------------------------------------------------------------
void TServer_Dt::IniFileWrite(void)
{
   QSettings setings(setingsFileName,QSettings::IniFormat) ;

   setings.setValue("logSystemScr",logSrvScr);
   setings.setValue("logSystem",logSrv);
   setings.setValue("logNWScr",logNWScr);
   setings.setValue("logNW",logNW);
   setings.setValue("logDTScr",logDTScr);
   setings.setValue("logDT",logDT);
}

//-----------------------------------------------------------------------------
//--- Sets enable/disable log information in log object
//-----------------------------------------------------------------------------
void TServer_Dt::setLogEnable(void)
{
  device->logNw->setLogEnable(logNWScr,logNW);
  device->logSrv->setLogEnable(logSrvScr,logSrv);
  device->logDev->setLogEnable(logDTScr,logDT);
  device->logDTMaster->setLogEnable(false,logDTM);
}

//-----------------------------------------------------------------------------
//--- Put on user interface data from settings
//-----------------------------------------------------------------------------
void TServer_Dt::setupUI(void)
{
  pageSetup->setCbLogSysS(logSrvScr);
  pageSetup->setCbLogSys(logSrv);
  pageSetup->setCbLogNWS(logNWScr);
  pageSetup->setCbLogNW(logNW);
  pageSetup->setCbLogDTS(logDTScr);
  pageSetup->setCbLogDT(logDT);
}

//-----------------------------------------------------------------------------
//--- Private slot on change combo box in setup page
//-----------------------------------------------------------------------------
void TServer_Dt::getUI(void)
{
  logSrvScr=pageSetup->getCbLogSysS();
  logSrv=pageSetup->getCbLogSys();
  logNWScr=pageSetup->getCbLogNWS();
  logNW=pageSetup->getCbLogNW();
  logDTScr=pageSetup->getCbLogDTS();
  logDT=pageSetup->getCbLogDT();

  setLogEnable(); //update log
  IniFileWrite();
}

/*
//-----------------------------------------------------------------------------
//--- Private slot
//--- Show icon from resurces in tray
//-----------------------------------------------------------------------------
void TServer_Dt::showTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QIcon trayImage = QIcon(":/image/usb_32.png");
    trayIcon->setIcon(trayImage);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("DT-XX server");
    setWindowIcon(trayImage);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,     SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon ->show();
}

//-----------------------------------------------------------------------------
//--- Private slot
//--- Set actions in pull up menu
//-----------------------------------------------------------------------------
void TServer_Dt::setTrayIconActions()
{
  //... Setting actions ...
  minimizeAction = new QAction(tr("Minimize"), this);
  restoreAction = new QAction(tr("Restore"), this);
  quitAction = new QAction(tr("Exit"), this);

  //... Connecting actions to slots ...
  connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hideProg()));
  connect(restoreAction, SIGNAL(triggered()), this, SLOT(showProg()));
  connect(quitAction, SIGNAL(triggered()), this, SLOT(quiteProg()));


  //... Setting system tray's icon menu ...
  trayIconMenu = new QMenu(this);

  trayIconMenu->addAction (restoreAction);
  trayIconMenu->addAction (minimizeAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction (quitAction);
}
*/

/*
//-----------------------------------------------------------------------------
//--- Private slot
//--- tray processing function. Reaction on press mouse in the icon region
//-----------------------------------------------------------------------------
void TServer_Dt::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
  case QSystemTrayIcon::Trigger:
  case QSystemTrayIcon::DoubleClick:
    if(!isVisible()) this->showNormal(); else hide();
    break;
  case QSystemTrayIcon::MiddleClick:
    break;
  default:
    break;
  }
}
*/

// actions on tray menu pressed  private slots
//-----------------------------------------------------------------------------
//--- Private slot
//--- restore window
//-----------------------------------------------------------------------------
void TServer_Dt::showProg(void)
{
  showNormal();
}
//-----------------------------------------------------------------------------
//--- Private slot
//--- hide window
//-----------------------------------------------------------------------------
void TServer_Dt::hideProg(void)
{
  hide();
}

//-----------------------------------------------------------------------------
//--- Private slot
//--- quite from program
//-----------------------------------------------------------------------------
void TServer_Dt::quiteProg(void)
{
  qApp->quit();
}

/*
//-----------------------------------------------------------------------------
//--- Reimplement of the close window event
//-----------------------------------------------------------------------------
void TServer_Dt::closeEvent(QCloseEvent *event)
{
  if(trayIcon->isVisible()) {
    hide();//quiteProg();
    event->ignore();
  }

}
*/

QString get_link_url(QString fn){

    QString r;
    QFile   data(fn);

    if (!data.open(QFile::ReadOnly))
        return r;

    QTextStream out(&data);
    QString line;

    do {
        if ((line = out.readLine()).toUpper().startsWith("URL"))
            return  line.split("=")[1].trimmed();

    } while( !line.isNull() );

    return r;
}
