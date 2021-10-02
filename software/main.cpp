#include "SystemController.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SystemController w;
    w.show();
    return a.exec();
}
