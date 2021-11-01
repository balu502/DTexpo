#include "pages.h"

//-----------------------------------------------------------------------------
//--- Constructor
//--- Pages system  network  device
//-----------------------------------------------------------------------------
TPageLog::TPageLog(QWidget *parent) : QWidget(parent)
{
  log=new QTextBrowser ;
  QVBoxLayout *mainLayout = new QVBoxLayout;

  log->clear();
  mainLayout->addWidget(log);
  log->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(log,SIGNAL(customContextMenuRequested(const QPoint&)), this,SLOT(showContextMenu(const QPoint&)));
  setLayout(mainLayout);
  countLines=0;
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TPageLog::~TPageLog()
{
  qDebug()<<"Log page destructor";
  if(log) delete log;
}

//-----------------------------------------------------------------------------
//--- Make user define menu on pages system,  network,  device
//-----------------------------------------------------------------------------
void TPageLog::showContextMenu(const QPoint& pt)
{
  QMenu * menu = log->createStandardContextMenu();

/* Example how create sub menu in menu
  QMenu * tags;
  tags=menu->addMenu(tr("&User menu"));
  QAction * taginstance = new QAction(tr("Clear"), this);
  connect(taginstance, SIGNAL(triggered()), log, SLOT(clear()));
  tags->addAction(taginstance);
*/
// add to menu actions
  QAction * clear = new QAction(tr("Clear"), this);
  connect(clear, SIGNAL(triggered()), log, SLOT(clear()));
  menu->addAction(clear);
  QAction * pause = new QAction(tr("Pause"), this);
  connect(pause, SIGNAL(triggered()), this, SLOT(clickPause()));
  menu->addAction(pause);
  QAction * run = new QAction(tr("Run"), this);
  connect(run, SIGNAL(triggered()), this, SLOT(clickRun()));
  menu->addAction(run);

  menu->exec(log->viewport()->mapToGlobal(pt));
  delete clear;
  delete pause;
  delete run;
  delete menu;
}

//-----------------------------------------------------------------------------
//--- Emit signal when press user menu pause
//-----------------------------------------------------------------------------
void TPageLog::clickPause()
{
  emit pauseChange(false);
}

//-----------------------------------------------------------------------------
//--- Emit signal when press user menu run
//-----------------------------------------------------------------------------
void TPageLog::clickRun()
{
  emit pauseChange(true);
}

//-----------------------------------------------------------------------------
//--- Constructor
//--- Pages Settings
//-----------------------------------------------------------------------------
TPageSetup::TPageSetup(QWidget *parent) : QWidget(parent)
{
  QGridLayout *mainLayout = new QGridLayout;

  cbLogSysS=new QCheckBox(tr("System log enable on screen"),this);
  cbLogSys=new QCheckBox(tr("System log enable into file"),this);
  cbLogNWS=new QCheckBox(tr("Network log enable on screen"),this);
  cbLogNW=new QCheckBox(tr("Network log enable into file"),this);
  cbLogDTS=new QCheckBox(tr("Device DT-XX log enable on screen"),this);
  cbLogDT=new QCheckBox(tr("Device DT-XX log enable into file"),this);

  connect(cbLogSysS,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));
  connect(cbLogSysS,SIGNAL(stateChanged(int)),this,SIGNAL(sigChangeUI()));
 // connect(cbLogSys,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));

  connect(cbLogNWS,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));
  connect(cbLogNWS,SIGNAL(stateChanged(int)),this,SIGNAL(sigChangeUI()));
  connect(cbLogNW,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));

  connect(cbLogDTS,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));
  connect(cbLogDTS,SIGNAL(stateChanged(int)),this,SIGNAL(sigChangeUI()));
  connect(cbLogDT,SIGNAL(clicked()),this,SIGNAL(sigChangeUI()));
  QString compilationTime = QString("SW %1 %2 %3").arg(APP_VERSION).arg(__DATE__).arg(__TIME__);
  labelSW =new QLabel(compilationTime);

  mainLayout->setSpacing(20);
  mainLayout->addWidget(cbLogSysS,0,0);
  mainLayout->addWidget(cbLogSys, 1,0);
  mainLayout->addWidget(cbLogNWS, 2,0);
  mainLayout->addWidget(cbLogNW,  3,0);
  mainLayout->addWidget(cbLogDTS, 4,0);
  mainLayout->addWidget(cbLogDT,  5,0);
  mainLayout->addWidget(labelSW,  6,0,Qt::AlignBottom);

  setLayout(mainLayout);
}

//-----------------------------------------------------------------------------
//--- Destructor
//-----------------------------------------------------------------------------
TPageSetup::~TPageSetup()
{
  qDebug()<<"Settings page destructor";
}

