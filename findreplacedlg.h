#ifndef FINDREPLACEDLG_H
#define FINDREPLACEDLG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QCheckBox;

class FindReplaceDlg : public QDialog
{
    Q_OBJECT

    QLineEdit *ledFind;
    QLineEdit *ledReplace;

    QPushButton *btnFind;
    QPushButton *btnReplace;
    QPushButton *btnReplaceAll;

    QCheckBox   *cbxCaseSensitive;
    //QCheckBox   *cbxWholeWords;

public:
    explicit FindReplaceDlg(QWidget *parent = 0);

    enum {
        caseSens = 1,
        wholeWords = 2
    };

signals:
    void findNext( QString, int );
    void replace( QString, QString, int );
    void replaceAll( QString, QString, int );

public slots:
    void onBtnFind();
    void onBtnReplace();
    void onBtnReplaceAll();

    //void onCbxCaseSensitive();
    //void onCbxWholeWords();
    //void onCheckbox();
};

#endif // FINDREPLACEDLG_H
