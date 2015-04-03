#include <QApplication>

#include "configuracion.h"
#include "ventana.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Configuracion::obtenerCarpetaDeTrabajo();

    Ventana *ventana = new Ventana;
    ventana->resize(640,480);
    ventana->show();

    return app.exec();
}
