#ifndef VENTANA_H
#define VENTANA_H

#include <QWidget>
#include <QGridLayout>

#include "escena.h"

class Ventana : public QWidget
{
private:
    QGridLayout *layout;
    Escena *escena;
    Mat matImagen;

public:
    Ventana();
};

#endif // VENTANA_H
