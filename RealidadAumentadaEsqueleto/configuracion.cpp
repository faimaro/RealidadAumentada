#include "configuracion.h"

QString Configuracion::carpetaDeTrabajo = ".";

// Este metodo se llama en main.cpp para que levante del archivo config.txt la carpeta de trabajo.
// Si este archivo no existe entonces la carpeta sera ./

void Configuracion::obtenerCarpetaDeTrabajo()
{
    QFile file("../config.txt");

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            carpetaDeTrabajo = line;

            carpetaDeTrabajo = carpetaDeTrabajo.remove("\n");
            carpetaDeTrabajo = carpetaDeTrabajo.remove("\r");

            qDebug() << "Directorio de trabajo = " << carpetaDeTrabajo;
        }
    }
}
