#include "dialog.h"
#include "ui_dialog.h"

void Dialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape) QDialog::keyPressEvent(e);
}
void Dialog::ChangeLanguages(int index)
{
    if (index == 0){
        QApplication::setLayoutDirection(Qt::RightToLeft);
        #ifdef Q_OS_WIN32
        SetWindowLong((HWND)this->winId(), GWL_EXSTYLE, GetWindowLong((HWND)this->winId(), GWL_EXSTYLE) | WS_EX_LAYOUTRTL);
        SetWindowLong((HWND)ui->widget->winId(), GWL_EXSTYLE, GetWindowLong((HWND)ui->widget->winId(), GWL_EXSTYLE) & (~WS_EX_LAYOUTRTL));
        #endif
    }else{
        QApplication::setLayoutDirection(Qt::LeftToRight);
        #ifdef Q_OS_WIN32 //Q_OS_MAC  Q_OS_LINUX
        SetWindowLong((HWND)this->winId(), GWL_EXSTYLE, GetWindowLong((HWND)this->winId(), GWL_EXSTYLE) & (~WS_EX_LAYOUTRTL));
        #endif
    }

    if (index == 0){
        translator.load(":/Resources/Languages/arabic");
    }else if(index == 1){
        translator.load(":/Resources/Languages/french");
    }else{
        translator.load(":/Resources/Languages/english");
    }
    QApplication::installTranslator(&translator);
    ui->retranslateUi(this);
    this->setLayoutDirection(QApplication::layoutDirection());
    ui->widget->setLayoutDirection(QApplication::layoutDirection());

}
void Dialog::closeEvent(QCloseEvent *)
{
    closed();
    qApp->quit();
}
Dialog::Dialog(QWidget *parent) : QDialog(parent),ui(new Ui::Dialog),
    settings("QMaahad","Settings"),RecorData(false),
    Response_Ok(false),TotalStream(0),
    CurrStream(0),Response_Resume(false)
{
    QString s(OSTEST);
    ui->setupUi(this);
    int n = QApplication::arguments().count();

    if (n < 5) {qApp->quit();return;}
    skey = qApp->arguments().at(1);
    if (skey.mid(0,1) != "/") {qApp->quit();return;}

    NameEXE    = qApp->arguments().at(2);
    VersionWeb = qApp->arguments().at(3);
    PathWeb    = qApp->arguments().at(4);
    if (n > 5)
        PathWeb2 = qApp->arguments().at(5);
    else{
        PathWeb2 = "";
        if (skey == "/k") {qApp->quit();return;}
    }

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags);
    int index = settings.value("Languages", 0).toInt();
    ChangeLanguages(index);


    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/Resources/frm/pp.png")));
    this->setPalette(palette);

    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(connected()),this,SLOT(whereConnect()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readsck()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(closed()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorsck(QAbstractSocket::SocketError)));

    QApplication::processEvents();
    if (skey == "/k")
        OpenURL(PathWeb2);
    else
        OpenURL(PathWeb);

}
Dialog::~Dialog()
{
    delete ui;
}
void Dialog::Cleantest()
{
    if (skey == "/c"){
        QDir dir(QDir::tempPath());
        dir.rmdir(VersionWeb);
    }else if (skey == "/k"){
        QDir dir(QApplication::applicationDirPath());
        dir.rmdir(VersionWeb);
    }

}
void Dialog::test()
{
    QString pat,tmp;

    pat = QApplication::applicationDirPath();
    tmp = QDir::tempPath();
    if (skey == "/k"){
        QDir dir(pat);
        dir.mkdir(VersionWeb);
        FilenameEXE = PathWeb2.split("/").at(PathWeb2.split("/").count()-1);
        exe = new QFile(pat+"/"+VersionWeb+"/"+FilenameEXE);
    }else if(skey == "/c"){
        QDir dir(tmp);
        dir.mkdir(VersionWeb);
        FilenameEXE = PathWeb.split("/").at(PathWeb.split("/").count()-1);
        exe = new QFile(tmp+"/"+VersionWeb+"/"+FilenameEXE);
    }

    if (exe->exists())
        CurrStream = exe->size();
    else
        CurrStream = 0;

}
void Dialog::Fintest()
{

    if (exe->isOpen())
    {
        exe->close();

        if (TotalStream == CurrStream && TotalStream > 0){
            if (skey == "/k"){
                if (QFile(QApplication::applicationDirPath()+"/"+NameEXE).exists())
                {
                    QFile::remove(QApplication::applicationDirPath()+"/"+NameEXE);
                }
                exe->rename(QApplication::applicationDirPath()+"/"+NameEXE);
                Cleantest();
                QProcess *prs = new QProcess();
                exe->setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeUser |
                                    QFile::ReadUser | QFile::WriteUser |
                                    QFile::ReadGroup | QFile::WriteGroup |
                                    QFile::ReadOther);
/* -------------------------------------------Uncompress------------------------------------------ */
                QFile file(QApplication::applicationDirPath()+"/"+NameEXE);
                file.open(QIODevice::ReadOnly);
                QByteArray by = file.readAll();
                file.close();
                QByteArray by2 = qUncompress(by); //qCompress(by,9);
                by.clear();
                if (by2.size() > 0)
                {
                    QFile file2(QApplication::applicationDirPath()+"/"+NameEXE+".0");
                    file2.open(QIODevice::WriteOnly);
                    file2.write(by2);
                    file2.close();
                    file2.setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeUser |
                                         QFile::ReadUser | QFile::WriteUser |
                                         QFile::ReadGroup | QFile::WriteGroup |
                                         QFile::ReadOther);
                    QFile::remove(QApplication::applicationDirPath()+"/"+NameEXE);
                    file2.rename(QApplication::applicationDirPath()+"/"+NameEXE);

                }
/* ---------------------------------------------------------------------------------------------- */
#ifdef Q_OS_WIN32
                QDesktopServices::openUrl(QUrl("file:///"+QApplication::applicationDirPath()+"/../maahad.exe"));
#else
                prs->start(QApplication::applicationDirPath()+"/"+NameEXE);
#endif

            }else if(skey == "/c"){
                if (QFile(QDir::tempPath()+"/"+FilenameEXE).exists())
                {
                    QFile::remove(QDir::tempPath()+"/"+FilenameEXE);
                }
                exe->rename(QDir::tempPath()+"/"+FilenameEXE);
                Cleantest();
                exe->setPermissions(QFile::ExeGroup | QFile::ExeOther | QFile::ExeUser |
                                    QFile::ReadUser | QFile::WriteUser |
                                    QFile::ReadGroup | QFile::WriteGroup |
                                    QFile::ReadOther);
                QDesktopServices::openUrl(QUrl("file:///"+QDir::tempPath()+"/"+FilenameEXE));
            }
        }
    }
    qApp->quit();
}
/*----------------------------Socket-----------------------------*/
void Dialog::whereConnect()
{

}
void Dialog::readsck()
{
    QByteArray HeadStream,StLen,tst;
    QByteArray by = socket->readAll();

    Stream.append(by);

aa:
    if (RecorData)
    {
        //Save Stream
        CurrStream += Stream.size();
        ui->label->setText(QString::number(CurrStream/1024)+" KB");
        float ss = ((float)CurrStream/(float)TotalStream);
        ss = ss * 100;
        ui->progressBar->setValue((int)ss);
        exe->write(Stream);
        Stream.clear();
        if (TotalStream == CurrStream && TotalStream > 0){
            closed();
        }
        return;
    }
    if (Stream.size() < 15 && !Response_Ok )
        if(!Response_Resume) return;

    if (Response_Ok){
        int ps = Stream.indexOf("\r\n\r\n");
        if (ps != -1){
            HeadStream = Stream.mid(0,ps);

            //ui->plainTextEdit->setPlainText(QString(HeadStream));
            Stream = Stream.mid(ps+4);
            RecorData = true;

            int ps1 = HeadStream.indexOf("Content-Length:");
            int ps2 = HeadStream.indexOf("\r\n",ps1);
            StLen = HeadStream.mid(ps1,ps2-ps1);
            TotalStream = StLen.mid(16).toInt() ;
            ui->label_2->setText(QString::number(TotalStream/1024)+" KB");
            goto aa;
        }
    }else if(Response_Resume){
        int ps = Stream.indexOf("\r\n\r\n");
        if (ps != -1){
            HeadStream = Stream.mid(0,ps);

            //ui->plainTextEdit->setPlainText(QString(HeadStream));
            Stream = Stream.mid(ps+4);
            RecorData = true;

            int ps1 = HeadStream.indexOf("Content-Range:");
            int ps2 = HeadStream.indexOf("\r\n",ps1);
            StLen = HeadStream.mid(ps1,ps2-ps1);
            tst = StLen.mid(15,5);
            if (tst == "bytes") StLen = StLen.mid(21);
            ps1 = StLen.indexOf("-");
            CurrStream = StLen.mid(0,ps1).toInt();
            ps2 = StLen.indexOf("/");
            TotalStream = StLen.mid(ps2+1).toInt() ;
            ui->label_2->setText(QString::number(TotalStream/1024)+" KB");
            goto aa;
        }
    }else{
        if (Stream.mid(9,6) == "200 OK"){
            Response_Ok = true;
            exe->open(QIODevice::WriteOnly);
        }else if (Stream.mid(9,11) == "206 Partial"){
            Response_Resume = true;
            exe->open(QIODevice::Append);
        }else if (Stream.mid(9,4) == "416 "){
            //Response_Resume = true;
            exe->open(QIODevice::Append);
            TotalStream = CurrStream;
        }else if (Stream.mid(9,13) == "404 Not Found"){
            QMessageBox msgBox;
             msgBox.setWindowTitle(tr("Hakiba al 3lmiya"));
             msgBox.setText(tr("404 Not Found"));
             msgBox.exec();
             closed();
             Cleantest();
             QApplication::processEvents();
             qApp->quit();
        }else{
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Hakiba al 3lmiya"));
            msgBox.setText(tr("Error On Site"));
            msgBox.exec();
            closed();
            Cleantest();
            QApplication::processEvents();
            qApp->quit();
        }
    }
}
void Dialog::closed()
{
    switch (socket->state()) {
    case QAbstractSocket::UnconnectedState:
        qDebug("Status: Unconnected");

        break;
    case QAbstractSocket::HostLookupState:
        qDebug("Status: Searching for host");
        break;
    case QAbstractSocket::ConnectingState:
        qDebug("Status: Connecting to host");
        break;
    case QAbstractSocket::ConnectedState:
        qDebug("Status: ConnectedState");
        break;
    case QAbstractSocket::BoundState:
        qDebug("Status: BoundState");
        break;
    case QAbstractSocket::ClosingState:
        qDebug("Status: ClosingState");
        break;
    case QAbstractSocket::ListeningState:
        qDebug("Status: ListeningState");
        break;
    default:
        break;
    }
    if (socket->state() == QAbstractSocket::ConnectedState )
    {
        socket->close();
        Fintest();
    }

}
void Dialog::errorsck(QAbstractSocket::SocketError)
{
    closed();
}
/*----------------------------Socket-----------------------------*/
void Dialog::OpenURL(QString web)
{
    QString strGet,Host;
    int n = 0, portno = 80;

    test();

    if (web.mid(0,7).toLower()=="http://" || web.mid(0,4).toLower()=="www."){
        portno=80;
    }else{
        return;
    }
    n = web.indexOf("//");

    Host = web.mid(n+2);
    n = Host.indexOf("/");
    if (n > -1) strGet = Host.mid(n);
    Host = Host.mid(0,Host.size() - strGet.size());

    socket->connectToHost(Host,portno,QIODevice::ReadWrite);

    while(socket->state() == QAbstractSocket::HostLookupState || socket->state() == QAbstractSocket::ConnectingState)
    {
        QApplication::processEvents();
    }

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        //ui->pushButton_2->setEnabled(true);
        QByteArray source(strGet.toStdString().c_str());
        QByteArray host(Host.toStdString().c_str());
        QByteArray range(QString::number(CurrStream).toStdString().c_str());


        if(CurrStream > 0)
            range = "Range: bytes="+range+"-\n";
        else
            range ="";

        QByteArray St = "GET "+source+" HTTP/1.0\n"
                        +range+
                        "User-Agent: Wget/1.12 (linux-gnu)\n"
                        "Accept: */*\n"
                        "Host: "+host+"\n"
                        "Connection: Keep-Alive\n\n";
        socket->write(St);
    }else if (socket->state() == QAbstractSocket::UnconnectedState){
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Hakiba al 3lmiya"));
        msgBox.setText(tr("No Connection"));
        msgBox.exec();
        closed();
        Cleantest();
        QApplication::processEvents();
        qApp->quit();
    }
}

