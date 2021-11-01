#ifndef QEXPODIALOG_H
#define QEXPODIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QSpinBox>

class QExpoDialog : public QDialog
{
    Q_OBJECT

    QList< QSpinBox* > chsList;

public:
    explicit QExpoDialog(
            QWidget *parent = 0
            , Qt::WindowFlags f = Qt::WindowFlags());

    QString values( QString vsIni, bool* ok=0 );
    void setValues( QString vsIni );

signals:

public slots:
};

#endif // QEXPODIALOG_H
