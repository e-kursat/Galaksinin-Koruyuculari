#include "mainwindow.h"

#include <QApplication>
#include <QtGui/QSurface>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4,3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();
    return a.exec();
}
