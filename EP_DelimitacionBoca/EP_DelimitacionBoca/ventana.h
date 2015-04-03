#ifndef VENTANA_H
#define VENTANA_H

#include <QWidget>
#include <QPalette>
#include <QGridLayout>

#include <QLabel>
#include "imagen.h"
#include "block.h"
#include "escena.h"

class Ventana : public QWidget
{
    Q_OBJECT

private:

    QGridLayout *layout;

    QPalette palette;

    QLabel *lTitulo;
    Imagen *imgPascal;

    Block *blockDetener; bool procesando;
    Block *blockMas;
    Block *blockMenos;
    Block *blockFullScreen;

    Escena *escena;

public:

    Ventana();

private slots:

    void slot_detener();
    void slot_fullScreen();
};

#endif // VENTANA_H
