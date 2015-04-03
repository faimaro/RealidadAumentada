#ifndef MODEL3DS_H
#define MODEL3DS_H

#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include <QGLWidget>


class CModel3DS
{
    public:
        CModel3DS(std::string filename);
        virtual void CreateVBO();
        virtual ~CModel3DS();

        void GetFaces();

        Lib3dsFile * m_model;
        unsigned int m_TotalFaces;
        GLuint m_VertexVBO, m_NormalVBO, m_TexCoordVBO;

        QString getNombreArchivo3dsConPath()  {  return QString(nombreArchivo.c_str());  }
        QString getNombreArchivoSinExtension()  {
            QString nombreConExtension = QString(nombreArchivo.c_str());

            return nombreConExtension.remove(nombreConExtension.size()-3, 3);
        }

        QString getNombreArchivoId()  {
            QString nombreSinExtensionNiCarpeta = QString(nombreArchivo.c_str());
            nombreSinExtensionNiCarpeta = nombreSinExtensionNiCarpeta.remove(nombreSinExtensionNiCarpeta.lastIndexOf("."), nombreSinExtensionNiCarpeta.size());
            nombreSinExtensionNiCarpeta = nombreSinExtensionNiCarpeta.remove(0, nombreSinExtensionNiCarpeta.lastIndexOf("/")+1);

            return nombreSinExtensionNiCarpeta;
        }

        int getIdTexturaModel()  {  return this->idTexturaModel;  }
        void setIdTexturaModel(int idTexturaModel)  {  this->idTexturaModel = idTexturaModel;  }

private:
        std::string nombreArchivo;
        QString nombreArchivoId;  // Esta variable guarda el nombre del archivo sin la extensio ni el directorio
        int idTexturaModel;
};

#endif // MODEL3DS_H
