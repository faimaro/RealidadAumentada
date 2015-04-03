#include "model3ds.h"

#include <assert.h>
#include <QDebug>
#include <QFile>

// Load 3DS model
CModel3DS::CModel3DS(std::string filename)  {

    nombreArchivo = filename;

    nombreArchivoId = QString(filename.c_str());
    nombreArchivoId = nombreArchivoId.remove(nombreArchivoId.lastIndexOf("."), nombreArchivoId.size());
    nombreArchivoId = nombreArchivoId.remove(0, nombreArchivoId.lastIndexOf("/")+1);

    m_TotalFaces = 0;

    m_model = lib3ds_file_load(filename.c_str());

    // If loading the model failed, we throw an exception
    if(!m_model)  {
        qDebug() <<  "Unable to load " << filename.c_str();
    }
}

// Destructor
CModel3DS::~CModel3DS()
{

    if(m_model != NULL)
    {
        lib3ds_file_free(m_model);
    }

}

// Copy vertices and normals to the memory of the GPU
void CModel3DS::CreateVBO()
{

}

// Count the total number of faces this model has
void CModel3DS::GetFaces()
{
    assert(m_model != NULL);

    m_TotalFaces = 0;
    Lib3dsMesh * mesh;
    // Loop through every mesh
    for(mesh = m_model->meshes;mesh != NULL;mesh = mesh->next)
    {
        // Add the number of faces this mesh has to the total faces
        m_TotalFaces += mesh->faces;
    }


}

