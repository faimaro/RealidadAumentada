#ifndef NAVEGADOR_H
#define NAVEGADOR_H

#include <QWidget>

#include <QPixmap>
#include <QImage>

namespace Ui {
class Navegador;
}

class Navegador : public QWidget
{
    Q_OBJECT
    
public:
    explicit Navegador(QWidget *parent = 0);
    ~Navegador();

    QImage getImagenNavegador(int x, int y, int ancho, int alto);
    void cargarSitio(QString sitio);

private:
    Ui::Navegador *ui;

    QPixmap originalPixmap;

signals:
    void signal_sitioCargado();

private slots:
    void slot_sitioCargado();
};

#endif // NAVEGADOR_H
