#ifndef TEXTURACOMUN_H
#define TEXTURACOMUN_H

#include <QString>


// Esta clase mantiene la info de la textura que es un archivo en disco y tambien mantiene los valores de rotacion,
// traslacion, etc. Basicamente es la caja y todos sus datos.
class TexturaComun  {

    public:
        TexturaComun(QString nombreArchivo) : rotacionActual(0)  {
            this->nombreArchivo = nombreArchivo;

            nombreArchivoId = nombreArchivo;
            nombreArchivoId = nombreArchivoId.remove(nombreArchivoId.lastIndexOf("."), nombreArchivoId.size());
            nombreArchivoId = nombreArchivoId.remove(0, nombreArchivoId.lastIndexOf("/")+1);
        }

        QString getNombreArchivoConPath()  {  return nombreArchivo;  }
        QString getNombreArchivoSinExtension()  {
            QString nombreConExtension = nombreArchivo;

            return nombreConExtension.remove(nombreConExtension.size()-3, 3);
        }

        QString getNombreArchivoId()  {
            QString nombreSinExtensionNiCarpeta = nombreArchivo;
            nombreSinExtensionNiCarpeta = nombreSinExtensionNiCarpeta.remove(nombreSinExtensionNiCarpeta.lastIndexOf("."), nombreSinExtensionNiCarpeta.size());
            nombreSinExtensionNiCarpeta = nombreSinExtensionNiCarpeta.remove(0, nombreSinExtensionNiCarpeta.lastIndexOf("/")+1);

            return nombreSinExtensionNiCarpeta;
        }

        int getIdTexturaComun()  {  return this->idTexturaComun;  }
        void setIdTexturaComun(int idTexturaComun)  {  this->idTexturaComun = idTexturaComun;  }

        void incrementarAnguloRotacion(int angulo)  {
            rotacionActual += angulo;
        }

        int getAnguloActual()  {  return this->rotacionActual;  }

        void clearAngulo()  {  this->rotacionActual = 0;  }

private:
        QString nombreArchivo;  // Aca se almacena el nombre con extension y directorio
        QString nombreArchivoId;  // Esta variable guarda el nombre del archivo sin la extensio ni el directorio
        int idTexturaComun;

        // Como se usa en cada render el loadIdentity() entonces hay que mantener la angulo en cada momento
        int rotacionActual;
};

#endif // TEXTURACOMUN_H
