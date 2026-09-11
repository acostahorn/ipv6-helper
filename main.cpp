#include <QApplication>
#include "ipv6Helper.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   QFile styleFile(":/style.qss");
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
        styleFile.close();
        qDebug() << "Stylesheet loaded successfully!";
    } else {
        qWarning() << "Failed to open style.qss from resource path ':/style.qss'!";
    }

    IPv6Helper i;
    i.show();

    return a.exec();
}
