#ifndef ESCENA_H
#define ESCENA_H

#define RESOLUCION_ANCHO 640
#define RESOLUCION_ALTO 480

#define CANTIDAD_TEXTURAS 10

// Para el Widget
#include <QGLWidget>
#include <QTimer>

#include <opencv/highgui.h>
#include "textura.h"

// Para leer el directorio de Texturas
#include <configuracion.h>
#include <QStringList>
#include <QImage>
#include <QDir>

// Para descargar imagenes de Internet
#include "clienteurl.h"

using namespace cv;

class Escena : public QGLWidget
{
    Q_OBJECT

private:
    // Puntero a VideoCapture para captar las imagenes de la Camara
    VideoCapture *videoCapture;

    // Timer para actualizar la escena con paintGL()
    QTimer *timerEscena;

    // Este arreglo de texturas incluye la textura para la camara (texturas[0])
    Textura texturas[CANTIDAD_TEXTURAS];

    // Metodo para leer las texturas de la carpeta Texturas al iniciar la Escena
    void leerDirectorioTexturas();

    // Metodos para dibujar
    void dibujarImagen(QString nombreTextura, float x, float y, float z, float ancho, float alto);

    // Valor que nos indica la cantidad de texturas de la carpeta, para ennumerar las de internet
    int cantidadTexturasDirectorio;

    // Valor que nos indica que textura de internet estamos utilizando
    int texturaClienteActual;

public:
    Escena();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private slots:
    void slot_actualizarEscena();
    void slot_cargarTexturaCliente(QByteArray datos);
};

#endif // ESCENA_H
