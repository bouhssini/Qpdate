#include "dialog.h"
#include <QTextCodec>
// /k QMaahad.exe 1.0.52 http://ks391271.kimsufi.com/web/setup_maahade_1.0.52.exe

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    int id1 = QFontDatabase::addApplicationFont(":/Resources/font/tahoma.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/Resources/font/tahomabd.ttf");
    int id3 = QFontDatabase::addApplicationFont(":/Resources/font/DroidKufi-Bold.ttf");
    int id4 = QFontDatabase::addApplicationFont(":/Resources/font/DroidKufi-Regular.ttf");

    QString family1 = QFontDatabase::applicationFontFamilies(id1).at(0);
    QString family2 = QFontDatabase::applicationFontFamilies(id2).at(0);
    QString family3 = QFontDatabase::applicationFontFamilies(id3).at(0);
    QString family4 = QFontDatabase::applicationFontFamilies(id4).at(0);

    QFont tahoma(family1);
    QFont tahomabd(family2);
    QFont DroidKufiB(family3);
    QFont DroidKufiR(family4);

#ifdef Q_OS_MAC
    tahoma.setPointSize(12);
    tahomabd.setPointSize(12);
    DroidKufiB.setPointSize(12);
    DroidKufiR.setPointSize(12);
#else
    tahoma.setPointSize(8);
    tahomabd.setPointSize(8);
    DroidKufiB.setPointSize(8);
    DroidKufiR.setPointSize(8);
#endif

    a.setFont(DroidKufiR);
    a.setFont(DroidKufiB);

    a.setLayoutDirection(Qt::RightToLeft);
    a.setWindowIcon(QIcon(":/Resources/Internet.png"));

    #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    #endif

    int n = QApplication::arguments().count();

    if (n < 5) {a.quit();return 0;}

    if (a.arguments().at(1).mid(0,1) != "/") {a.quit();return 0;}

    Dialog w;
    w.show();
    
    return a.exec();
}
