#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "highlighter.h"
#include "findreplacedlg.h"

/*
 * 1) Windows 7 (x86_64)
 * 2) installer
 * 3) uninstall
 * 4) синтаксис С 89
 * 5) кодировки ASCII, utf-8, utf-16
 * 6) [HEX, BIN, OCT, DEC]
 * 7) find / replace [regexp]
 * 8) откыть (недавние документы), сохранить, сохранить как
 * 9) to upper case, to lower case, sort (case sensetive +-)
 * 10) camal case, underscore, const
 * 11) autoformat ({}, tabulation)
*/

#define RECENT_FILES_VALUE "recent_files"

class QTextEdit;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void about();
    void newFile();
    void openFile();
    void openRecentFile();
    void saveFile( );
    void saveFileAs();
    void searchReplace();
    void lowercaseSelection();
    void uppercaseSelection();
    void autoIndentSelection();
    void camelCaseSelection();
    void underscoreSelection();
    void constNameSelection();
    void sortSelection();

    void findNext( QString, int );
    void replace( QString, QString, int );
    void replaceAll(QString, QString, int );

private:
    void setupEditor();
    void setupFileMenu();
    void setupHelpMenu();
    void setupEditMenu();

    void readFile( const QString & );
    void writeFile( const QString & );

    bool isCamelCase( const QString & word );
    bool isUnderscore( const QString & word );
    bool isConstName( const QString & word );

    void setCurrentFile( const QString & );
    void updateRecentActionList();

    FindReplaceDlg *dlgFindReplace;
    QTextEdit *editor;
    Highlighter *highlighter;

    QList< QAction * >  m_recentFileActions;
    enum { maxRecentQty = 6 };

    QString             m_currentFilePath;
};

#endif // MAINWINDOW_H
