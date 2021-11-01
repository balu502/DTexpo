#ifndef TPAGES_H
#define TPAGES_H

#include <QtGui>
#include <QtWidgets>

class TPageLog : public QWidget
{
  Q_OBJECT
public:
  TPageLog(QWidget *parent = 0);
  ~TPageLog();

private:
  QTextBrowser *log;
  int countLines;
public slots:
  void addToLog(QString s){ if(countLines++>1000) {log->clear();countLines=0;}log->append(s); }
  void showContextMenu(const QPoint& pt);
  void clickPause(void);
  void clickRun();
signals:
  void pauseChange(bool);
};

class TPageSetup :public QWidget
{
  Q_OBJECT
public:
  TPageSetup(QWidget *parent = 0);
  ~TPageSetup();
public slots:
  void setCbLogSysS(bool state){ cbLogSysS->setChecked(state); }
  void setCbLogSys(bool state){ cbLogSys->setChecked(state); }
  void setCbLogNWS(bool state){ cbLogNWS->setChecked(state); }
  void setCbLogNW(bool state){ cbLogNW->setChecked(state); }
  void setCbLogDTS(bool state){ cbLogDTS->setChecked(state); }
  void setCbLogDT(bool state){ cbLogDT->setChecked(state); }
  bool getCbLogSysS(void) { return cbLogSysS->isChecked();}
  bool getCbLogSys(void) { return cbLogSys->isChecked();}
  bool getCbLogNWS(void) { return cbLogNWS->isChecked();}
  bool getCbLogNW(void) { return cbLogNW->isChecked();}
  bool getCbLogDTS(void) { return cbLogDTS->isChecked();}
  bool getCbLogDT(void) { return cbLogDT->isChecked();}

signals:
  void sigChangeUI(void );

protected:
  QCheckBox *cbLogSysS,*cbLogSys,*cbLogNWS,*cbLogNW,*cbLogDTS,*cbLogDT;
  QLabel *labelSW;
};
#endif // TPAGES_H
