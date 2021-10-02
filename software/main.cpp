#include "SystemController.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SystemController window;
    window.show();
    return app.exec();
}
