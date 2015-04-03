#ifndef UTIL_H
#define UTIL_H


//#include <cxtypes.h>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
//#include <io.h>

#include <QImage>
#include <opencv/cv.h>
#include <QVector>
#include <QByteArray>
#include <QDebug>
#include <QPoint>
#include <QRect>
#include <QVector>

#include "configuracion.h"

class Util  {
private:
    static Util* util;

public:
    Util();
    virtual ~Util();
    static Util* getUtil();
    static IplImage* qImage2IplImage(const QImage& qImage, bool BGR);
    static QImage IplImage2QImage(const IplImage *iplImage);
};


#endif // UTIL_H
