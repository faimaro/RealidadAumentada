#include "imagen.h"

Imagen::Imagen( QString imgUri )
{
    if ( QFile( imgUri ).exists() )
    {
        image.load( imgUri );
    }
    if( !image.isNull() ) this->setFixedSize( image.size() );
}

void Imagen::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );

    if( !image.isNull() )
    {
        painter.drawImage( this->width() / 2 - image.width() / 2,
                           this->height() / 2 - image.height() / 2,
                           image );
    }
}
