#include "qexpodialog.h"
#include "digres.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSpinBox>

QExpoDialog::QExpoDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout *dLyt    = new QVBoxLayout;
    QGridLayout *mainGrid= new QGridLayout;
    QHBoxLayout *hLyt;
    QWidget     *hw;

    for(int i=0; i<COUNT_CH; i++){
        hLyt = new QHBoxLayout;
        hw   = new QWidget;

        chsList << new QSpinBox();
        chsList.last()->setRange(50, 10000);
        chsList.last()->setValue(    1000 );
        chsList.last()->setSingleStep( 50 );

        hLyt->addWidget( new QLabel( QString("Expo[%1]").arg(i) ));
        hLyt->addWidget( chsList.last() );

        hw->setLayout( hLyt );
        dLyt->addWidget( hw );
    }

    QDialogButtonBox *bts = new QDialogButtonBox(
                  QDialogButtonBox::Ok
                | QDialogButtonBox::Cancel);

//    bts->addButton( new QPushButton("Factory vals")
//                    , QDialogButtonBox::ActionRole );

    dLyt->addWidget( hw  );
    dLyt->addWidget( bts );

    mainGrid->addLayout(dLyt,0,0);
    setLayout( mainGrid );

    bts->button(QDialogButtonBox::Cancel)->setDefault(true);

    connect(bts->button(QDialogButtonBox::Ok)
            , SIGNAL(clicked(bool)), this, SLOT(accept()));

    connect(bts->button(QDialogButtonBox::Cancel)
            , SIGNAL(clicked(bool)) , this, SLOT(reject()));
}


QString QExpoDialog::values(QString vsIni, bool *ok){

    if (QDialog::Rejected == exec()){
        if (ok) *ok = false;
        return  vsIni;
    }

    QStringList rval;

    foreach (QSpinBox *s, chsList)
        rval << QString::number( s->value() );

    if (ok) *ok = true;
    return rval.join(";");
}


void QExpoDialog::setValues(QString vsIni){

    QStringList chLines;

    if (      vsIni.contains(";"))
        chLines= vsIni.split(";", QString::SkipEmptyParts);
    else if ( vsIni.contains(","))
        chLines= vsIni.split(",", QString::SkipEmptyParts);
    else if ( vsIni.contains(" "))
        chLines= vsIni.split(" ", QString::SkipEmptyParts);

    for ( int i = 0; i < qMin(chLines.count(), chsList.count()); i++ )
        chsList[ i ]->setValue( chLines[ i ].toInt() );
}
