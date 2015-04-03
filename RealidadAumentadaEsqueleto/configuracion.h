#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <QString>
#include <QDebug>
#include <QFile>

class Configuracion
{
public:
    static QString carpetaDeTrabajo;
    static void obtenerCarpetaDeTrabajo();
};

#endif // CONFIGURACION_H
