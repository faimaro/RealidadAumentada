/* Clase definida para el uso de Texturas, cuenta con el Mat correspondiente
 * a la imagen, el id, el puntero char a la textura y la funcion generar(); */

#ifndef TEXTURA_H
#define TEXTURA_H

#include <QGLWidget>
#include <opencv/cv.h>

using namespace cv;

class Textura
{
public:
    Textura();

    QString nombreTextura;

    Mat matTextura;
    GLuint idTextura;
    unsigned char *textura;

    void generarDesdeMat();
};

#endif // TEXTURA_H
