#ifndef THREEDWRITER_H
#define THREEDWRITER_H

#include <QPoint>
#include <QGLFunctions>
#include <QString>
#include <QChar>
#include <QList>

/**
 * Esta clase permite escribir texto en un plano 3D al estilo "Star Wars".
 * @brief The ThreeDWriter class
 */
class ThreeDWriter
{
public:
    ThreeDWriter(int fsize, QString fname, float x, float y, float z, float width, float height, float cof);
    void writeText(QString text);
    void setCof(int cof);
    void setX(int x);
    void setY(int y);
    void setZ(int z);
    void setHeight( float h);
    void setWidth( float w);
    void setFontSize(int size);
    void setFontName(QString name);
    void setIsMirroredWriting(bool mirrored);
    int getCof();
    int getX();
    int getY();
    int getZ();
    float getHeight();
    float getWidth();
    int getFontSize();
    bool isMirroredWriting();
    QString getFontName();



private:
    int fontSize;
    QString fontName;
    bool mirroredWriting;
    float x;
    float y;
    float z;
    float width;
    float height;
    float start_x;
    float start_y;
    float start_z;
    float cof;
    void writeLine(QString textLine);


};

#endif // THREEDWRITER_H
