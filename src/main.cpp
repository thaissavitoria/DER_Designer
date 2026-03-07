#include <QApplication>

#include "view/MainWindow.h"
#include <QDir>

int main(
    int argc, 
    char* argv[]
)
{
    QApplication app(argc, argv); 

    QDir resourceDir(":/");
    app.setWindowIcon(QIcon(":/app_icon"));

    MainWindow window;
    window.show();

    return app.exec();
}