#include <QApplication>

#include "ventana.h"
#include "configuracion.h"

#include <QDebug>
#include <QDesktopWidget>

#include "video.h"

int main( int argc, char** argv )  {
    QApplication a(argc, argv);

    Configuracion::obtenerConfiguracion();

    QDesktopWidget * desktopWidget = QApplication::desktop();
    qDebug() << "Cantidad de Screens" << desktopWidget->screenCount();

    Ventana ventana;
    ventana.showMaximized();
//    ventana.show();
//    ventana.move(500, 0);

//    Video video;
//    video.reproducir();
//    video.show();


    int quit = a.exec();

    return quit;
}
