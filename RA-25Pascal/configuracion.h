#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <QString>
#include <QFile>
#include <QDebug>

// Esta clase es principalmente para saber en que directorio buscar texturas, modelos y cual es la carpeta de trabajo
class Configuracion  {

public:
    static QString carpetaDeTrabajo;
    static QString tipoCamara;

    static void obtenerConfiguracion();
};


#endif // CONFIGURACION_H
