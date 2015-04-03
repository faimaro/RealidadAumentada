#include "ventana.h"

Ventana::Ventana() : procesando( true )
{
    palette.setColor( QPalette::Background, QColor( 255, 255, 255 ) ) ;
    palette.setColor( QPalette::WindowText, QColor( 0, 0, 0 ) );

    lTitulo = new QLabel( "Delimitación de boca mediante técnicas de Realidad Aumentada" );
    lTitulo->setFont( QFont( "Courier New", 13 ) );

    imgPascal = new Imagen( "../Icons/Ubp.jpg" );

    blockDetener = new Block( "Procesar.png", "Detener", 100 );
    blockMas = new Block( "Mas.png", "Lineas", 100 );
    blockMenos = new Block( "Menos.png", "Lineas", 100 );
    blockFullScreen = new Block( "FullScreen.png", "FullScreen", 100 );

    escena = new Escena;

    layout = new QGridLayout;
    layout->setAlignment( Qt::AlignTop );

    layout->addWidget( lTitulo,         0, 0, 1, 7, Qt::AlignCenter );
    layout->addWidget( imgPascal,       0, 7, 1, 1, Qt::AlignCenter );
    layout->addWidget( blockDetener,    1, 0, 1, 1 );
    layout->addWidget( blockMas,        2, 0, 1, 1 );
    layout->addWidget( blockMenos,      3, 0, 1, 1 );
    layout->addWidget( blockFullScreen, 4, 0, 1, 1 );
    layout->addWidget( escena,          1, 1, 4, 7 );

    this->setPalette( palette );
    this->setLayout( layout );

    connect( blockDetener, SIGNAL( clicked() ), this, SLOT( slot_detener() ) );
    connect( blockDetener, SIGNAL( clicked() ), escena, SLOT( slot_detenerProcesamiento() ) );
    connect( blockMas, SIGNAL( clicked() ), escena, SLOT( slot_aumentarAnchoLineas() ) );
    connect( blockMenos, SIGNAL( clicked() ), escena, SLOT( slot_disminuirAnchoLineas() ) );
    connect( blockFullScreen, SIGNAL( clicked() ), this, SLOT( slot_fullScreen() ) );
}

void Ventana::slot_detener()
{
    if( !procesando )
    {
        blockDetener->setText( "Detener" );
    }
    else
    {
        blockDetener->setText( "Procesar" );
    }

    procesando = !procesando;
}

void Ventana::slot_fullScreen()
{
    if( !this->isFullScreen() )
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}
