#ifndef LOGOBJECT_H
#define LOGOBJECT_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#define MAX_LOG_SIZE   100000
#define ARCHIVE_LENGTH      9

class LogObject : public QObject
{
  Q_OBJECT
public:
  explicit LogObject(QString logFileName,int logFileSize=MAX_LOG_SIZE,int arch_length=ARCHIVE_LENGTH,QObject *parent = 0);
  ~LogObject();
  void setLogEnable(bool enS,bool en){logScrEnable=enS;logEnable=en;}
signals:
  void logScrMessage(QString);
public slots:
  void logMessage(QString message);
private:
  QString FileName;
  bool logEnable,logScrEnable;
  QFile *log;
  QTextStream log_out;
  int max_log_size;
  int archive_lenght;
  bool canWrite;
  void archive(void);
};

#endif // LOGOBJECT_H
