#ifndef VENTANA_H
#define VENTANA_H

#include <QWidget>
#include <QDesktopWidget>

class HiloCaptura;
class Escena;

namespace Ui {
    class Ventana;
}

class Ventana : public QWidget
{
    Q_OBJECT

public:
    explicit Ventana(QWidget *parent = 0);
    ~Ventana();

    Ui::Ventana *ui;
    HiloCaptura* hiloCaptura;
    Escena * escena;

private:    


protected:
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);

private slots:
    void slot_encenderCamara();
    void slot_apagarCamara();
    void slot_llevarHaciaProyector();

    void slot_SendMsg_recibido(QString,int);

};

#endif // VENTANA_H
