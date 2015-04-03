#ifndef ESCENA_H
#define ESCENA_H

#include <QGLWidget>
#include <QGLFunctions>
#include <QImage>
#include <QKeyEvent>
#include <QTimer>
#include <QPoint>
#include <QVector>
#include <QRect>
#include <opencv/cv.h>
#include "aruco/aruco.h"
#include "aruco/cvdrawingutils.h"
#include "glut.h"

#include "model3ds.h"
#include "texturaComun.h"

#include "cara.h"

#include  <QSerialPort>

#include "configuracion.h"

#include "navegador.h"
#include "persona.h"
#include "faceDetector.h"

#include "adminDB.h"

#include "clienteURL.h"

#include "video.h"

//#define RESOLUTION_W 1280
//#define RESOLUTION_H 1024

#define RESOLUTION_W 640
#define RESOLUTION_H 480


class Ventana;


using namespace aruco;

// Heredar tambien de QGLFunctions es para poder utilizar algunas funciones que estan disponibles en la extensiones
// de OpenGL. Algunos funciones como glGenBuffers. Para habilitarlas hay que llamar al
// metodo initializeGLFunctions() de QGLFunctions, y se recomienda hacerlo en initializeGL().
class Escena : public QGLWidget, protected QGLFunctions  {
    Q_OBJECT

public:
    Escena(QWidget * parent = 0);
    virtual ~Escena();
    void setImage(Mat & imageCamara);
    void setMarker(std::vector<Marker> *markers);
    void iniciarTemporizador();
    void detenerTemporizador();
    void setCameraParam(CameraParameters* camParam);

    void mostrarRealidad(bool realidadActiva)  {  this->realidadActiva = realidadActiva;  }
    void mostrarEscenaOpengl(bool escenaActiva)  {  this->escenaActiva = escenaActiva;  }

    void setPersonas(QVector<Persona> vPersonas)  {  this->vPersonas = vPersonas;  }

    enum SizePapel { A4, A5, A6, SizeMarcador };

private:
    Ventana * ventana;

    AdminDB adminDB;

    ClienteURL clienteURL;

    Video *video;
    bool videoPlayed;

    QImage imageCamara;        // QImage para camara
    QImage imageTexturaComun;
    QImage imageModel;
    QImage imageGoogleStreetView;
    QImage imageVideo;
    void prepararImagenCamara(QRect recorte = QRect());
    void prepararImagenesTexturasComunes();
    void prepararModelos();
    void prepararImagenModel();
    void prepararImagenNavegador(QImage imagenNavegador);
    void prepararImagenVideo(QVideoFrame imagenVideo);

    cv::Mat frame;     // IplImage para camara
    unsigned char* textureCamara;
    void createTextureFromCurrentImg();

    GLuint idTextura[50];  // Limitamos por ahora un maximo de 50 texturas

    GLuint idTexturaWidget[10];  // Se usa para las texturas de widgets para llevarlos a 3D. Limitamos a 10 texturas
    bool widgetTexturizado;
    bool widgetTexturizadoPedido;

    GLuint texturaParaIplImage;

    unsigned char* texturaCamara;
    unsigned char* texturaComun;
    unsigned char* texturaModel;
    unsigned char* texturaGoogleStreetView;
    unsigned char* texturaImageVideo;
    bool alMenosUnaVistaStreetView;


    bool imagenNueva;
    bool camaraActiva;
    QTimer * timer;
    std::vector<aruco::Marker> *markers;
    CameraParameters* camParam;
    GLfloat LightAmbient[4];
    GLfloat LightDiffuse[4];
    GLfloat LightPosition[4];
    int paraIntercambiar;

    // 0.025f es el size del marcador /// isPapel es para que dibuje un papel con la textura en lugar de la caja
    void dibujarCaja(QString nombreTexturaComun, float sizeLadoCaja = 0.025f, bool isPapel = false);
    void dibujarPapelParaEscribir(SizePapel sizePapel, int anguloRotacion = 0);
    void dibujarModel(QString nombreArchivoModelo, int porcentajeEscala=100);
    void dibujarVistaGoogleStreetView(float sizeLadoCaja = 0.025f);
    void dibujarVideo(float sizeLadoCaja = 0.025f);

    void leerDirectorioCrearCModel3DS();
    void leerDirectorioTexturas();

    float redimensionConTeclas;

    QSerialPort serial;
    void abrirPuerto(QString numeroCom);
    void actualizaServoSiCorresponde(int xActualMarcador, int yActualMarcador, int franjaCentral);
    int valorServoHorizontal, valorServoVertical;

    bool seguirMarcadorConServo;

    bool realidadActiva;
    bool escenaActiva;

    Navegador navegador;

    QVector<Persona> vPersonas;

    FaceDetector detectorCara;


protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mouseDoubleClickEvent(QMouseEvent *)  {
        this->detenerTemporizador();
        this->close();
    }

    void keyPressEvent(QKeyEvent *e)  {
        switch(e->key())  {
        case Qt::Key_Escape:
            if (!realidadActiva)  {
                this->detenerTemporizador();
                this->close();
            }
            break;
        case Qt::Key_Up:
            this->redimensionConTeclas+=0.00025f;
            break;
        case Qt::Key_Down:
            this->redimensionConTeclas-=0.00025f;
            break;

        case Qt::Key_P:

            break;

        default:
            break;
        }
    }

    QVector<CModel3DS *> vModels;
    QVector<TexturaComun *> vTexturas;

private slots:
    void slot_actualizarEscena();
    void slot_pedirRecurso();
    void slot_cargarTexturaGoogleStreetView();

public slots:
    void slot_fotocopiar();
    void slot_navegadorLoadFinished();

};

#endif // ESCENA_H
