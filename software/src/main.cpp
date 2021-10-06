#include "SystemController.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    // Initialize application
    QApplication app(argc, argv);

    // Initialize window
    SystemController window;
    window.setContentsMargins(30, 30, 30, 30);
    window.showMaximized();

    //window.setStyleSheet("SystemController {background-color: gray}");
    
    // Start application
    return app.exec();
}
