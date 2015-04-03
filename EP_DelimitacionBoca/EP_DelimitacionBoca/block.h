#ifndef BLOCK_H
#define BLOCK_H

// Widget
#include <QWidget>
#include <QGridLayout>

// Visual
#include <QPainter>
#include <QImage>
#include <QLabel>

// Events
#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>

class Block : public QWidget
{
    Q_OBJECT

private:

    QGridLayout *layout;
    QLabel      *label;
    QPalette    *palette;
    QImage       image;

public:

    Block( QString iconName, QString text, int size, QWidget *parent = 0 );
    void setText( QString text );

protected:

    bool event              ( QEvent       *event );
    void paintEvent         ( QPaintEvent  *event );
    void mousePressEvent    ( QMouseEvent  *event );
    void mouseReleaseEvent  ( QMouseEvent  *event );
    void resizeEvent        ( QResizeEvent *event );

signals:

    void clicked();
};

#endif // BLOCK_H
