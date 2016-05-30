#include "mainwindow.h"

#include <QtGui>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , dlgFindReplace( 0 )
    , highlighter( 0 )
{
    setupFileMenu();
    setupEditMenu();
    setupHelpMenu();

    setupEditor();

    setCentralWidget( editor );
    setWindowTitle( tr("Syntax highlighter") );

    resize( 800, 600 );
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupEditor() {
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont( font );
    editor->setLineWrapMode( QTextEdit::NoWrap );

    highlighter = new Highlighter( editor->document() );

    QSettings settings;
    QStringList recentFilesList = settings.value(RECENT_FILES_VALUE).toStringList();
    if( !recentFilesList.empty() )
    {
        readFile( recentFilesList.first() );
    }
    //QFile file( "O:\\prog\\src\\QtPrj\\TextEditor\\mainwindow.h" );
    //if( file.open( QFile::ReadOnly | QFile::Text ) )
    //    editor->setPlainText( file.readAll() );
}

void MainWindow::about() {
    QMessageBox::about( this, tr( "About text editor" ),
                        tr("<p>The <b>Text Editor</b></p>") );
}

void MainWindow::newFile(){
    editor->clear();

    setCurrentFile("");
}

void MainWindow::writeFile( const QString &path ) {
    QFile file( path );
    if( !file.open( QFile::WriteOnly | QFile::Text ) ) {
        QMessageBox::warning( this, tr("Text editor"), tr("Cannot write file %1:\n%2").arg(path).arg(file.errorString()) );
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << editor->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile( path );
}

void MainWindow::saveFileAs() {

    QString filePath = QFileDialog::getSaveFileName(this );
    if( filePath.isEmpty() ) return;
    writeFile( filePath );
}

void MainWindow::saveFile() {

    if( m_currentFilePath.isNull() )
        saveFileAs();
    else
        writeFile( m_currentFilePath );
}

void MainWindow::readFile( const QString &path ) {
    QFile file( path );

    if( !file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::warning( this, tr("Editor"), tr("This file could not be found:\n%!").arg(path) );
        return;
    }

    editor->setPlainText( file.readAll() );

    setCurrentFile( path );
}

void MainWindow::openRecentFile() {
    QAction * action = qobject_cast< QAction * > (sender());
    if( action ) {
        readFile(action->data().toString());
    }
}

void MainWindow::openFile() {

    QString fileName = QFileDialog::getOpenFileName( this, tr("Open File"), "", "");

    if( !fileName.isEmpty() )
        readFile( fileName );
}

void MainWindow::setCurrentFile( const QString &path ) {
    m_currentFilePath = path;
    setWindowFilePath( m_currentFilePath );

    QSettings settings;

    QStringList recentFilePaths = settings.value(RECENT_FILES_VALUE).toStringList();

    recentFilePaths.removeAll( m_currentFilePath );
    recentFilePaths.prepend(m_currentFilePath);
    while( recentFilePaths.size() >= maxRecentQty )
        recentFilePaths.removeLast();

    settings.setValue(RECENT_FILES_VALUE, recentFilePaths);

    updateRecentActionList();
}

void MainWindow::updateRecentActionList() {

    QSettings settings;
    QStringList recentFilePaths = settings.value(RECENT_FILES_VALUE).toStringList();

    int maxSz = qMin( static_cast<int>(maxRecentQty), recentFilePaths.size() );

    for( int i = 0; i < maxSz; ++i ) {
        QString fileName = QFileInfo( recentFilePaths.at(i) ).fileName();
        m_recentFileActions.at(i)->setText(fileName);
        m_recentFileActions.at(i)->setData(recentFilePaths.at(i));
        m_recentFileActions.at(i)->setVisible(true);
    }

    for( int i = maxSz; i < maxRecentQty; ++i ) {
        m_recentFileActions.at(i)->setVisible(false);
    }
}

void MainWindow::searchReplace() {

    if( dlgFindReplace == 0 ) {
        dlgFindReplace = new FindReplaceDlg(this);
        connect( dlgFindReplace, SIGNAL( findNext( QString, int ) ),             this, SLOT( findNext( QString, int ) ) );
        connect( dlgFindReplace, SIGNAL( replace( QString, QString, int ) ),     this, SLOT( replace( QString, QString, int ) ) );
        connect( dlgFindReplace, SIGNAL( replaceAll( QString, QString, int ) ),  this, SLOT( replaceAll( QString, QString, int ) ) );
    }

    dlgFindReplace->show();
    dlgFindReplace->raise();
    dlgFindReplace->activateWindow();
}

// changes selected text to "repl" if it is exactly matches regular exression provided in "next"
void MainWindow::replace( QString next, QString repl, int param ) {
    QTextCursor cur = editor->textCursor();
    if( !cur.hasSelection() ) return;

    //int pos = ( cur.position() < cur.anchor() ? cur.position() : cur.anchor() );

    QRegExp re( next, ( param & FindReplaceDlg::caseSens ? Qt::CaseSensitive : Qt::CaseInsensitive ) );
    if( re.exactMatch( cur.selectedText() ) ) {
        cur.beginEditBlock();
        //cur.setPosition( pos );
        //cur.setPosition( pos + re.matchedLength(), QTextCursor::KeepAnchor );
        cur.insertText( repl );
        cur.endEditBlock();

        editor->setTextCursor( cur );
    }
}

void MainWindow::replaceAll(QString next, QString repl , int param ) {

    QTextCursor cur = editor->textCursor();

    QString str = editor->toPlainText();
    int pos = 0, found = 0;

    QRegExp re( next, ( param & FindReplaceDlg::caseSens ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

    cur.beginEditBlock();
    while( ( found = re.indexIn( str, pos ) ) > 0 ) {
         cur.setPosition( found );
         cur.setPosition( found + re.matchedLength(), QTextCursor::KeepAnchor );
         cur.insertText( repl );

        str.replace( found, re.matchedLength(), repl );
        qDebug( "found: %d, pos: %d\n", found, pos );
        pos = found + repl.length();
    }

    qDebug( "found: %d, pos: %d\n", found, pos );
    cur.endEditBlock();

    //editor->setPlainText( str );

    editor->setTextCursor( cur );
}

void MainWindow::findNext( QString next, int param ) {

    QTextCursor cur = editor->textCursor();
    int pos = cur.position();

    QString str = editor->toPlainText();

    QRegExp re( next, ( param & FindReplaceDlg::caseSens ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

    int found = re.indexIn( str, pos );
    qDebug( "re result: %d\npattern: %s\nmatched len: %d\n", found, next.toAscii().data(), re.matchedLength() );
    if( found >= 0 ) {
        cur.setPosition( found );
        cur.setPosition( found + re.matchedLength(), QTextCursor::KeepAnchor );
        editor->setTextCursor( cur );
    }
    else {
        QMessageBox::information( this, tr("Text editor"), tr("Nothing was found. Repeat from start") );
    }
}

void MainWindow::sortSelection() {
    //QMessageBox::information( this, tr("Text editor"), tr("To be done...") );

    QTextCursor cur = editor->textCursor();
    if( !cur.hasSelection() ) return;

    qDebug( "Anchor: %d\nPos:   %d\n", cur.anchor(), cur.position() );

    //if( cur.anchor() < cur.position() ) {
    //    //cur.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
    //    cur.movePosition( QTextCursor::StartOfLine );
    //}
    //else {
    //    //cur.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor );
    //    cur.movePosition( QTextCursor::EndOfLine );
    //}
    //qDebug( "after:\nAnchor: %d\nPos:   %d\n", cur.anchor(), cur.position() );

    QString str = cur.selection().toPlainText();

    qDebug( "Selected text: %s\n", str.toAscii().data() );
    QStringList lines = str.split( QRegExp( "[\\r\\n]{1,2}" ) );
    //QStringList lines = str.split( "\r\n" );
    qDebug( "Lines qty: %d\n", lines.length() );
    lines.sort();

    str = lines.join( QString::fromUtf8( "\n") );
    qDebug( "Str: %s\n", str.toAscii().data() );

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( str );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::lowercaseSelection() {
    QTextCursor cur = editor->textCursor();
    if( !cur.hasSelection() ) return;

    QString str = cur.selectedText().toLower();

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( str );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::uppercaseSelection() {
    QTextCursor cur = editor->textCursor();
    if( !cur.hasSelection() ) return;

    QString str = cur.selectedText().toUpper();

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( str );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::autoIndentSelection() {

    QTextCursor cur = editor->textCursor();
    if( !cur.hasSelection() ) return;
    QString str = cur.selection().toPlainText();

    qDebug( "Selected text: %s\n", str.toAscii().data() );
    QStringList lines = str.split( QRegExp( "[\\r\\n]{1,2}" ) );
    //QStringList lines = str.split( "\r\n" );
    qDebug( "Lines qty: %d\n", lines.length() );

    QStringList::iterator linIt = lines.begin();

    int prevBracketLevel = 0, // level of indentation
        curBracketLevel = 0;

    bool openFirst = false, // is '}' found first?
        elseFirst = false;

    // cycle through lines in selection
    for( ; linIt != lines.end(); ++linIt ) {
        QString curLine = linIt->trimmed();
        //curLine.append("\r\n");
        // cycle through characters in line
        openFirst = false;
        elseFirst = false;
        foreach( const QChar & ch, curLine ) {
            //bool isWhitespace = true;
            // trim
            if( ch == '{' ) {
                curBracketLevel++;
                qDebug( "bracketLvl: %d, char: %c\n", curBracketLevel, ch.toAscii() );
                elseFirst = !openFirst;
            }
            else if( ch == '}' ) {
                if( curBracketLevel > 0 ) curBracketLevel--;
                qDebug( "bracketLvl: %d, char: %c\n", curBracketLevel, ch.toAscii() );
                openFirst = !elseFirst;
            }
            //else if( ch == ' ' || ch == '\t' ) {
            //    isWhitespace = true;
            //}
            else if( ch == '\n' ) {
                continue; // must be the last char in line
            }
        }
        int indent = 0; // effective level of indentation
        //if( curBracketLevel <= prevBracketLevel ) { // use curBracketLevel
        //    indent = openFirst? curBracketLevel-1 : curBracketLevel;
        //}
        //else { // use prevBracketLevel
            indent = openFirst? prevBracketLevel-1 : prevBracketLevel;
        //}

        // indenting current line
        for( int i=0; i<indent; ++i ) {
            //linIt->prepend( '\t' );
            curLine.prepend( "    " );
        }
        // switching bracket levels;
        prevBracketLevel = curBracketLevel;

        *linIt = curLine;
    }

    str = lines.join( QString::fromUtf8( "\n") );
    qDebug( "Str: %s\n", str.toAscii().data() );

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( str );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

bool MainWindow::isCamelCase( const QString & word ) {
    foreach( const QChar & ch, word ) {
        if( ch.isLower() || ch.isUpper() )
            continue;
        else return false;
    }
    return true;
}

bool MainWindow::isUnderscore( const QString & word ) {
    foreach( const QChar & ch, word ) {
        if( ch.isLower() || ch == '_' )
            continue;
        else return false;
    }
    return true;
}

bool MainWindow::isConstName( const QString & word ) {
    foreach( const QChar & ch, word ) {
        if( ch.isUpper() || ch == '_' )
            continue;
        else return false;
    }
    return true;
}

void MainWindow::camelCaseSelection() {

    QTextCursor cur = editor->textCursor();
    cur.select( QTextCursor::WordUnderCursor );

    QString word = cur.selectedText();

    if( isConstName( word ) ) {

        int undScore = 0,
            curPos = 0;
        bool isFirst = true;
        while( ( undScore = word.indexOf( '_', curPos ) ) != -1 ) {
            for( int i=curPos;i<undScore;++i ) {
                word[i] = word[i].toLower();
            }
            if( !isFirst ) {
                word[curPos] = word[curPos].toUpper();
            }

            word.remove(undScore, 1);

            curPos = undScore;
            isFirst = false;
        }

        if( !isFirst )
            word[curPos] = word[curPos].toUpper();
        else
            word[curPos] = word[curPos].toLower();
        // lowercase rest of the word
        for( int i = curPos+1; i < word.length(); ++i ) {
            word[i] = word[i].toLower();
        }
    }
    else if( isUnderscore( word ) ) {

        int undScore = 0,
            curPos = 0;
        bool isFirst = true;
        while( ( undScore = word.indexOf( '_', curPos ) ) != -1 ) {
            //for( int i=curPos;i<undScore;++i ) {
            //    word[i] = word[i].toLower();
            //}
            if( !isFirst ) {
                word[curPos] = word[curPos].toUpper();
            }

            word.remove(undScore, 1);

            curPos = undScore;
            isFirst = false;
        }

        if( !isFirst )
            word[curPos] = word[curPos].toUpper();
        else
            word[curPos] = word[curPos].toLower();
        // lowercase rest of the word
        //for( int i = curPos+1; i < word.length(); ++i ) {
        //    word[i] = word[i].toLower();
        //}
    }
    else return;

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( word );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::underscoreSelection() {

    QTextCursor cur = editor->textCursor();
    cur.select( QTextCursor::WordUnderCursor );

    QString word = cur.selectedText();

    if( isConstName( word ) ) {

    }
    else if( isUnderscore( word ) ) {

    }
    else return;

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( word );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::constNameSelection() {

    QTextCursor cur = editor->textCursor();
    cur.select( QTextCursor::WordUnderCursor );

    QString word = cur.selectedText();

    if( isConstName( word ) ) {

    }
    else if( isUnderscore( word ) ) {

    }
    else return;

    cur.beginEditBlock();
    //cur.removeSelectedText();
    cur.insertText( word );
    cur.endEditBlock();

    editor->setTextCursor(cur);
}

void MainWindow::setupFileMenu() {
    QMenu *fileMenu = new QMenu( tr( "&File" ), this );
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction( tr("&Open"), this, SLOT(openFile()),
                        QKeySequence( tr("Ctrl+O", "File|Open") ) );

    QAction *actTemp = 0;
    QMenu *recentFilesMenu = fileMenu->addMenu(tr("Open recent"));
    for( int i = 0; i < maxRecentQty; ++i ) {
        actTemp = new QAction( this );
        actTemp->setVisible( false );
        QObject::connect( actTemp, SIGNAL(triggered(bool)), this, SLOT(openRecentFile()));
        m_recentFileActions.append(actTemp);

        recentFilesMenu->addAction(m_recentFileActions.at(i));
    }

    fileMenu->addAction( tr("&New"), this, SLOT(newFile()),
                        QKeySequence( tr("Ctrl+N", "File|New") ) );
    fileMenu->addAction( tr("&Save"), this, SLOT(saveFile()),
                         QKeySequence( tr("Ctrl+S", "File|Save") ) );
    fileMenu->addAction( tr("Save as..."), this, SLOT(saveFileAs()) );

    fileMenu->addAction( tr("E&xit"), qApp, SLOT(quit()),
                        QKeySequence( tr("Ctrl+Q", "File|Exit") ) );
}

void MainWindow::setupHelpMenu() {
    QMenu *helpMenu = new QMenu( tr("&Help"), this );
    menuBar()->addMenu( helpMenu );

    helpMenu->addAction( tr("&About"), this, SLOT(about()));
    helpMenu->addAction( tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::setupEditMenu() {
    QMenu *editMenu = new QMenu( tr("&Edit"), this );
    menuBar()->addMenu( editMenu );

    editMenu->addAction( tr("Search and replace"), this, SLOT(searchReplace()) );
    editMenu->addSeparator();
    editMenu->addAction( tr("Sort"), this, SLOT(sortSelection()) );
    editMenu->addAction( tr("Lowercase"), this,  SLOT(lowercaseSelection()) );
    editMenu->addAction( tr("Uppercase"), this,  SLOT(uppercaseSelection()) );
    editMenu->addSeparator();
    editMenu->addAction( tr("Autoindent"), this, SLOT(autoIndentSelection()) );
    editMenu->addSeparator();
    editMenu->addAction( tr("To Camel case"), this, SLOT(camelCaseSelection()) );
    editMenu->addAction( tr("To Underscore"), this, SLOT(underscoreSelection()) );
    editMenu->addAction( tr("To Const name"), this, SLOT(constNameSelection()) );
//  9) to upper case, to lower case, sort (case sensetive +-)
//  10) camel case, underscore, const
//  11) autoformat ({}, tabulation)
}
