#ifndef IMAGEN_H
#define IMAGEN_H

#include <QWidget>

#include <QFile>
#include <QImage>
#include <QPainter>

class Imagen : public QWidget
{
    Q_OBJECT

private:

    QImage image;

public:

    Imagen( QString imgUri );

protected:

    void paintEvent( QPaintEvent *event );
};

#endif // IMAGEN_H
