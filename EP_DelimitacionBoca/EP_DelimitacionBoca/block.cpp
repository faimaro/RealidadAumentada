#include "block.h"

Block::Block( QString iconName, QString text, int size, QWidget *parent ) : QWidget( parent ),
                                                                            layout( new QGridLayout ),
                                                                            label( new QLabel( text ) ),
                                                                            palette( new QPalette )
{
    this->setMinimumWidth( size );

    layout->addWidget( this->label, 0, 0, 1, 1, Qt::AlignBottom );
    this->setLayout( layout );

    palette->setColor( QPalette::Background, QColor( 100, 0, 0 ) );
    palette->setColor( QPalette::WindowText, QColor( 255, 255, 255 ) );
    this->setAutoFillBackground( true );
    this->setPalette( *palette );

    image.load( "../Icons/" + iconName );
    this->repaint();
}

void Block::setText(QString text)
{
    label->setText( text );
}

bool Block::event( QEvent *event )
{
    if( event->type() == QEvent::Enter )
    {
        palette->setColor( QPalette::Background, QColor( 180, 0, 0 ) );
        this->setPalette( *palette );
    }

    if( event->type() == QEvent::Leave )
    {
        palette->setColor( QPalette::Background, QColor( 100, 0, 0 ) );
        this->setPalette( *palette );
    }

    return QWidget::event( event );
}

void Block::mousePressEvent( QMouseEvent *event )
{
    palette->setColor( QPalette::Background, QColor( 230, 0, 0 ) );
    this->setPalette( *palette );

    emit clicked();
}

void Block::mouseReleaseEvent( QMouseEvent *event )
{
    palette->setColor( QPalette::Background, QColor( 180, 0, 0 ) );
    this->setPalette( *palette );
}

void Block::resizeEvent(QResizeEvent *event)
{
    this->setFixedHeight( this->width() );
}

void Block::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );

    if( !image.isNull() )
    {
        painter.drawImage( this->width() / 2 - image.width() / 2,
                           this->height() / 2 - image.height() / 2,
                           image );
    }
}
