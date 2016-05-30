#include "findreplacedlg.h"

#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QLabel>

FindReplaceDlg::FindReplaceDlg(QWidget *parent) : QDialog(parent)
{

    ledFind    = new QLineEdit();
    ledReplace = new QLineEdit();

    btnFind       = new QPushButton(tr("Find next"));
    btnReplace    = new QPushButton(tr("Replace selection"));
    btnReplaceAll = new QPushButton(tr("Replace all"));

    cbxCaseSensitive = new QCheckBox(tr("Case sensitive"));
    //cbxWholeWords    = new QCheckBox(tr("Whole words"));

    connect( btnFind, SIGNAL(clicked()), this, SLOT(onBtnFind()) );
    connect( btnReplace, SIGNAL(clicked()), this, SLOT(onBtnReplace()) );
    connect( btnReplaceAll, SIGNAL(clicked()), this, SLOT(onBtnReplaceAll()) );
    // connect( cbxCaseSensitive, SIGNAL(clicked()), this, SLOT(onCheckbox()) );

    QGridLayout *grLayout = new QGridLayout();
    grLayout->addWidget( new QLabel(tr("Find")), 0,0,1,1 );
    grLayout->addWidget( new QLabel(tr("Replace")), 1,0,1,1 );

    grLayout->addWidget( ledFind, 0,1,1,1 );
    grLayout->addWidget( ledReplace, 1,1,1,1 );

    grLayout->addWidget( cbxCaseSensitive, 2,0,1,2);
    //grLayout->addWidget( cbxWholeWords, 3,0,1,1);

    grLayout->addWidget( btnFind,       0,2,1,1);
    grLayout->addWidget( btnReplace,    1,2,1,1);
    grLayout->addWidget( btnReplaceAll, 2,2,1,1);

    this->setLayout( grLayout );
}

void FindReplaceDlg::onBtnReplace() {
    int param = 0;
    param |= ( cbxCaseSensitive->isChecked() ? FindReplaceDlg::caseSens : 0 );
    //param |= ( cbxWholeWords->isChecked() ? FindReplaceDlg::wholeWords : 0 );
    emit replace( ledFind->text(), ledReplace->text(), param );
}
void FindReplaceDlg::onBtnReplaceAll() {
    int param = 0;
    param |= ( cbxCaseSensitive->isChecked() ? FindReplaceDlg::caseSens : 0 );
    //param |= ( cbxWholeWords->isChecked() ? FindReplaceDlg::wholeWords : 0 );
    emit replaceAll( ledFind->text(), ledReplace->text(), param );
}
void FindReplaceDlg::onBtnFind() {
    int param = 0;
    param |= ( cbxCaseSensitive->isChecked() ? FindReplaceDlg::caseSens : 0 );
    //param |= ( cbxWholeWords->isChecked() ? FindReplaceDlg::wholeWords : 0 );
    emit findNext( ledFind->text(), param );
}
//void FindReplaceDlg::onCheckbox() {

//}

