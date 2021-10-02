#include "SystemController.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    // Initialize application
    QApplication app(argc, argv);

    // Initialize window
    SystemController window;
    window.showMaximized();
    
    // Start application
    return app.exec();
}
