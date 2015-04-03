#include "threedwriter.h"
#include <QDebug>

/**
 * Set del coeficiente que ajusta las coordenas
 * @brief ThreeDWriter::setCof
 * @param cof
 */
ThreeDWriter::ThreeDWriter(int fsize, QString fname, float x, float y, float z, float width, float height, float cof)
{
    this->fontSize=fsize;
    this->fontName=fname;
    this->start_x=x;
    this->start_y=y;
    this->start_z=z;
    this->z=z;
    this->cof=cof;
    this->width=width;
    this->height=height;


}

/**
 * Este método dibuja una línea de texto.
 * @brief ThreeDWriter::writeLine
 * @param textLine Linea de texto a escribir
 * @param px coordenada x de donde comenzar a escribir
 * @param py coordenada y de donde comenzar a escribir
 * @param pz coordenada z de donde comenzar a escribir
 */
void ThreeDWriter::writeLine(QString textLine)
{


    QPainterPath path;
    glDisable(GL_LIGHTING);
    glColor3f (0.1f, 0.1f, 0.1f);
    QFont font(this->fontName, this->fontSize);
    path.addText(QPointF(0, 0), font, textLine);
    QList<QPolygonF> poly = path.toSubpathPolygons();    
    for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++){
        glBegin(GL_LINE_LOOP);

        for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++){

            this->x = p->rx()*this->cof+this->start_x;
            this->y = p->ry()*this->cof+this->start_y;
            this->z = this->start_z;
            glVertex3f(this->x, this->y, this->z);
            //qDebug()<<"Escribiendo en x:"<<x<<" y"<<y<< " z"<<z<<" p-rx "<<p->rx()<<" p->ry"<<p->ry();

        }
        glEnd();

    }
    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHTING);
}





void ThreeDWriter::writeText(QString text)
{

    QByteArray ba;
    int lines=0;
    ba = text.toLatin1();
    int charsPerLine= this->width/(this->fontSize*this->cof);
    int maxLines = this->height/(this->fontSize*this->cof);

    QString *toPrint = new QString();
    if(this->mirroredWriting) glRotated(180,0,1,0);
    foreach (QChar c, ba){

//        qDebug()<<"cha"<<c;
        if((toPrint->length()>=charsPerLine)||c==QChar(13)){

            this->writeLine(*toPrint);
            toPrint->clear();
            lines++;
            this->start_y+=this->fontSize*this->cof+cof;
        }

         toPrint->append(c);
         if(lines >= maxLines) return;


    }
    this->writeLine(*toPrint);








}

void ThreeDWriter::setCof(int cof)
{
    this->cof=cof;
}

/**
 * Set del valor x de la posicion donde comenzar a escribir
 * @brief ThreeDWriter::setX
 * @param x
 */
void ThreeDWriter::setX(int x)
{
    this->x=x;
}

/**
 * Set del valor Y de la posicion donde comenzar a escribir
 * @brief ThreeDWriter::setY
 * @param y
 */
void ThreeDWriter::setY(int y)
{
    this->y=y;
}
/**
 * Set del valor Z de la posicion donde comenzar a escribir
 * @brief ThreeDWriter::setZ
 * @param z
 */
void ThreeDWriter::setZ(int z)
{
    this->z=z;
}

void ThreeDWriter::setHeight(float h)
{
    this->height=h;
}

/**
 * Set del tamanio de la fuente
 * @brief ThreeDWriter::setFontSize
 * @param size
 */
void ThreeDWriter::setFontSize(int size)
{
    this->fontSize=size;
}

/**
 * Set del nombre de la fuente
 * @brief ThreeDWriter::setFontName
 * @param name
 */
void ThreeDWriter::setFontName(QString name)
{
    this->fontName=name;
}

void ThreeDWriter::setIsMirroredWriting(bool mirrored)
{
    this->mirroredWriting=mirrored;
}

/**
 * Get del coeficiente que ajusta el sistema de coordenadas
 * @brief ThreeDWriter::getCof
 * @return
 */
int ThreeDWriter::getCof()
{
    return this->cof;
}
/**
 * get de la coordenada x donde comenzara a escribir
 * @brief ThreeDWriter::getX
 * @return
 */
int ThreeDWriter::getX()
{
    return this->x;
}
/**
 * Get de la coordenada y donde comenzara a escribir
 * @brief ThreeDWriter::getY
 * @return
 */
int ThreeDWriter::getY()
{
    return this->y;
}

/**
 * Get de la coordenada z donde comenzara a escribir
 * @brief ThreeDWriter::getZ
 * @return
 */
int ThreeDWriter::getZ()
{
    return this->z;
}

float ThreeDWriter::getHeight()
{
    return this->height;
}

float ThreeDWriter::getWidth()
{
    return this->width;
}

/**
 * Get del tamanio de la fuente
 * @brief ThreeDWriter::getFontSize
 * @return
 */
int ThreeDWriter::getFontSize()
{
    return this->fontSize;
}
/**
 * Get del nombre de la fuente
 * @brief ThreeDWriter::getFontName
 * @return
 */
QString ThreeDWriter::getFontName()
{
    return this->fontName;
}


/**
 * Set del ancho de la caja de texto
 * @brief ThreeDWriter::setWidth
 * @param w
 */
void ThreeDWriter::setWidth(float w)
{
    this->width=w;
}
