#include "gui/mainwindow.h"
#include <QApplication>

void initializeSettings()
{
    QCoreApplication::setOrganizationDomain(QLatin1String("www.danish.com.ua"));
    QCoreApplication::setOrganizationName(QLatin1String("qcodec2_gui"));
    QCoreApplication::setApplicationName(QLatin1String("qcodec2_gui"));
    QCoreApplication::setApplicationVersion(QLatin1String("1.2.3.0"));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
