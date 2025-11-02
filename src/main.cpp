#include <QApplication>

#include "viewer/MainWindow.h"
#include <QDir>

int main(
    int argc, 
    char* argv[]
)
{
    QApplication app(argc, argv); 

    QDir resourceDir(":/");
    qDebug() << "Arquivos no recurso raiz:";
    for (const QString& entry : resourceDir.entryList()) {
        qDebug() << entry;
    }

    app.setWindowIcon(QIcon(":/app_icon"));

    MainWindow window;
    window.show();


    return app.exec();
}