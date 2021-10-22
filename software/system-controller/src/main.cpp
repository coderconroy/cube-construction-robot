#include "SystemController.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    // Initialize application
    QApplication app(argc, argv);

    // Initialize OpenGL context
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Initialize window
    SystemController window;
    window.setContentsMargins(30, 30, 30, 30);
    window.showMaximized();

    //window.setStyleSheet("SystemController {background-color: gray}");
    
    // Start application
    return app.exec();
}
