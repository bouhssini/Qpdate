#ifndef DIALOG_H
#define DIALOG_H
#ifdef WIN32
#include <windows.h>
#endif
#include <QApplication>
#include <QTextCodec>
#include <QProcessEnvironment>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>

#include <QDialog>
#include <QTranslator>
#include <QSettings>
#include <QKeyEvent>
#include <QtNetwork>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QDesktopServices>
#include <QFile>

#ifdef Q_OS_WIN32
#define OSTEST "Win32"
#elif Q_OS_MAC
#define OSTEST "MacOS"
#elif Q_OS_LINUX
#define OSTEST "Linux"
#endif

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
   void closeEvent(QCloseEvent *);
    
private:
    Ui::Dialog *ui;
    QTranslator  translator;
    QSettings settings;
    QString NameEXE;
    QString PathWeb;
    QString PathWeb2;
    QString VersionWeb;
    QString skey;
    QString FilenameEXE;
    QTcpSocket *socket;
    bool RecorData;
    bool Response_Ok;
    bool Response_Resume;
    QByteArray Stream;
    int TotalStream;
    int CurrStream;
    QFile *exe;

private:
    void ChangeLanguages(int index);
    void OpenURL(QString web);
    void test();
    void Fintest();
    void Cleantest();


private slots:
    void keyPressEvent(QKeyEvent *e);
    void whereConnect();
    void readsck();
    void errorsck(QAbstractSocket::SocketError);
    void closed();

};

#endif // DIALOG_H
