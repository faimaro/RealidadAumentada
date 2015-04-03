#include "ventana.h"
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include "hiloCaptura.h"
#include "escena.h"

#include "ui_ventana.h"


Ventana::Ventana(QWidget *parent) : QWidget(parent), ui(new Ui::Ventana)  {
    ui->setupUi(this);

    escena = new Escena(this);
    hiloCaptura = new HiloCaptura(this);

    ui->layoutEscena->addWidget(escena);
//    ui->layoutEscena->addWidget(escena, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
    escena->setFocus();  // Para que el Escape dentro de Escena sea capturada y poder cerrar la aplicacion

    connect(ui->pbCambiarCamara, SIGNAL(clicked()), hiloCaptura, SLOT(slot_cambiarCamara()));

    connect(ui->pbVisitarDomicilio, SIGNAL(clicked()), escena, SLOT(slot_pedirRecurso()));
    connect(ui->leVisitarDomicilio, SIGNAL(returnPressed()), escena, SLOT(slot_pedirRecurso()));
    connect(ui->leRotacion, SIGNAL(returnPressed()), escena, SLOT(slot_pedirRecurso()));

    this->slot_encenderCamara();
}

Ventana::~Ventana()  {
    delete ui;
    delete hiloCaptura;
}


void Ventana::slot_encenderCamara()  {
    escena->iniciarTemporizador();
    hiloCaptura->start();
    escena->setFocus();
}

void Ventana::slot_apagarCamara()  {
    escena->detenerTemporizador();
    hiloCaptura->finalizarHilo();
    escena->setFocus();
}


/**
  * Metodo que se ejecuta antes de cerrar la ventana (por ejemplo, cuando pulsamos X)
  */
void Ventana::closeEvent(QCloseEvent *)  {
    this->slot_apagarCamara();
    hiloCaptura->wait();  // Esperamos que el hilo finalice nates de cerrar la aplicacion.
    qDebug() << "closeEvent";
}

void Ventana::keyPressEvent(QKeyEvent *keyEvent)  {
    int key = keyEvent->key();

    switch(key)  {
    case Qt::Key_Escape:
        this->slot_apagarCamara();
        hiloCaptura->wait();  // Esperamos que el hilo finalice nates de cerrar la aplicacion.
        this->close();
        break;
    default: ;
    }
}

void Ventana::slot_llevarHaciaProyector()  {

    qDebug() << "Remover widget Escena";
    this->hide();
    ui->gridLayout->removeWidget(escena);  // Solo esta linea no funciona como se pretende
    escena->setParent(NULL);  // Si funciona.
    // Quizas porque cuando uno manda un widget a un layout entonces setea como padre al widget que lo contiene
//    escena->setFixedSize(this->width(), this->height());

    escena->mostrarRealidad(false);  /////////// Esta tenemos que descommentar
//    escena->mostrarRealidad(true);  /////// Esta tenemos que eliminar


    // Esto es para que si hay una segunda pantalla conectada, entonces que maximice escena en la segunda pantalla.
    QRect screenres = QApplication::desktop()->screenGeometry(1/*screenNumber*/);
    escena->move(QPoint(screenres.x(), screenres.y()));
    escena->resize(screenres.width(), screenres.height());


//    escena->setFixedSize(ancho, alto);
    escena->showFullScreen();
}

// Aqui es donde este servidor es controlado remotamente por el cliente. Los servicios son:
// - Llevar y traer desde el proyector: ("Proyector", 1) y ("Proyector", 0)
void Ventana::slot_SendMsg_recibido(QString cadenaRecibida, int valor)  {

    if (cadenaRecibida.contains("Proyector", Qt::CaseInsensitive))  {
        if (valor == 0)  {  // 0 - Sinluz - Para que apague el proyector

            escena->mostrarEscenaOpengl(false);

            this->slot_llevarHaciaProyector();  // ya incluye escena->mostrarRealidad(false);


        }
        if (valor == 1)  {  // 1 - Para que elimine el fondo real

            escena->mostrarEscenaOpengl(true);
            escena->mostrarRealidad(false);

            this->slot_llevarHaciaProyector();  // ya incluye escena->mostrarRealidad(false);
        }
        if (valor == 2)  {  // 2 = Normal - Para que se vea todo

            escena->mostrarEscenaOpengl(true);
            escena->mostrarRealidad(true);
            escena->setParent(this);

            ui->layoutEscena->addWidget(escena);
//            escena->setFocus();  // Para que el Escape dentro de Escena sea capturada y poder cerrar la aplicacion

            this->showMaximized();

        }
    }

    if (cadenaRecibida.contains("DetectorRostro", Qt::CaseInsensitive))
        (valor == 0) ? hiloCaptura->detectarRostro(false) : hiloCaptura->detectarRostro(true);

    if (cadenaRecibida.contains("DetectorQR", Qt::CaseInsensitive))
        (valor == 0) ? hiloCaptura->detectarQR(false) : hiloCaptura->detectarQR(true);

    if (cadenaRecibida.contains("DetectorPlanos", Qt::CaseInsensitive))
        (valor == 0) ? hiloCaptura->detectarPlanos(false) : hiloCaptura->detectarPlanos(true);

}

