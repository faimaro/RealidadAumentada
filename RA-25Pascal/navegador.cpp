#include "navegador.h"
#include "ui_navegador.h"
#include <QUrl>
#include <QDesktopWidget>

Navegador::Navegador(QWidget *parent) : QWidget(parent), ui(new Ui::Navegador)  {
    ui->setupUi(this);

    this->resize(QApplication::desktop()->width(), QApplication::desktop()->height());

    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(slot_sitioCargado()));
}

Navegador::~Navegador()  {
    delete ui;
}

// Se llama desde escena desde el slot_navegadorLoadFinished cuando ya esta descargado el sitio
QImage Navegador::getImagenNavegador(int x, int y, int ancho, int alto)  {
    originalPixmap = QPixmap(); // clear image for low memory situations
                                    // on embedded devices.
//    originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    originalPixmap = QPixmap::grabWidget(this, x, y, ancho, alto);

    return originalPixmap.toImage();
}

// Este metodo se llama desde escena cuando detecta algun marcado y codigo QR
void Navegador::cargarSitio(QString sitio)  {
    ui->webView->load(QUrl(sitio));
}

void Navegador::slot_sitioCargado()  {
    emit signal_sitioCargado();
}

