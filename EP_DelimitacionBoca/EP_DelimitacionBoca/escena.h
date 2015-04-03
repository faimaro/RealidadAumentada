#ifndef ESCENA_H
#define ESCENA_H

#define RESOLUCION_ANCHO 640
#define RESOLUCION_ALTO 480

#define CANTIDAD_TEXTURAS 10

// Macros para los contenedores de ojos (EN PORCENTAJE)
#define MARGEN_LATERAL_CONTENEDOR_OJOS 10 // Proyecto Emi Original = 0
#define MARGEN_VERTICAL_CONTENEDOR_OJOS 10 // Proyecto Emi Original = 20
#define ANCHO_CONTENEDOR_OJO 40 // Proyecto Emi Original = 50
#define ALTO_CONTENEDOR_OJO 50 // Proyecto Emi Original = 40

// Widget
#include <QGLWidget>
#include <QTimer>

// Imagen y Procesamiento
#include <opencv/highgui.h>
#include "textura.h"
#include <cmath>

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

    // Metodo para procesar el Mat
    void procesar( Mat &frame );
    int linesWidth;

    // Metodos y atributos para reconocimiento
    CascadeClassifier clasificadorCara;
    CascadeClassifier clasificadorOjoIzquierdo;
    CascadeClassifier clasificadorOjoDerecho;

    void procesar( QString archivoImagen );
    Rect rectanguloMasGrande( std::vector<Rect> rectangulos );
    float anguloEntre( Point a, Point b );
    int cant;

public:

    Escena();

protected:

    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();

private slots:

    void slot_actualizarEscena();

    // Slots para acciones externas
    void slot_aumentarAnchoLineas();
    void slot_disminuirAnchoLineas();
    void slot_detenerProcesamiento();
};

#endif // ESCENA_H
