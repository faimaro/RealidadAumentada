#ifndef CARA_H
#define CARA_H

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>

using namespace cv;

class Cara
{
public:
    Cara() : center(-1, -1), diametro(0), width(0), height(0)  {

    }

    Point center;
    int diametro;
    int width;
    int height;
};



#endif // CARA_H
