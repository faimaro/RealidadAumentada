#include "configuracion.h"

QString Configuracion::carpetaDeTrabajo = ".";
QString Configuracion::tipoCamara = "webcam1";

// Este metodo se llama en main.cpp para que levante del archivo config.txt la carpeta de trabajo.
// Si este archivo no existe entonces la carpeta sera ./
void Configuracion::obtenerConfiguracion()  {
    QFile file("../RA-25Pascal/config.txt");

    // Vemos aqui que se lee linea por linea y se descarta el /r/n que puede estar al final
    // Primer linea: Carpeta de trabajo
    // Segunda linea: Camara a utilizar (opciones: kinect - webcam1 - webcam2 - webcam3
    if (file.exists())  {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))  {

            QByteArray lineCarpetaTrabajo = file.readLine();
            carpetaDeTrabajo = lineCarpetaTrabajo;

            carpetaDeTrabajo = carpetaDeTrabajo.remove("\n");
            carpetaDeTrabajo = carpetaDeTrabajo.remove("\r");

            qDebug() << "Directorio de trabajo = " << carpetaDeTrabajo;


            QByteArray lineTipoCamara = file.readLine();
            tipoCamara = lineTipoCamara;

            tipoCamara = tipoCamara.remove("\n");
            tipoCamara = tipoCamara.remove("\r");

            qDebug() << "tipo de camara = " << tipoCamara;

        }
    }
}

