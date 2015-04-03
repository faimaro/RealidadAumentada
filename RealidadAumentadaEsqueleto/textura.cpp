#include "textura.h"

Textura::Textura() : nombreTextura (""), idTextura(-1)
{

}

void Textura::generarDesdeMat()
{
    glBindTexture( GL_TEXTURE_2D, idTextura );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, matTextura.cols, matTextura.rows,0, GL_BGR, GL_UNSIGNED_BYTE, matTextura.data);
}
