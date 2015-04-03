#include "ventana.h"

Ventana::Ventana()
{
    escena = new Escena;

    layout = new QGridLayout;
    layout->setMargin(0);
    layout->addWidget(escena, 0, 0, 1, 1);
    this->setLayout(layout);
}
