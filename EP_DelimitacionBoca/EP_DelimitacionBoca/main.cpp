#include <QApplication>

#include "ventana.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Ventana *ventana = new Ventana;
    ventana->showFullScreen();

    return app.exec();
}
