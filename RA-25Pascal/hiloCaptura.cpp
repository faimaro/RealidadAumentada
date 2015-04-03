#include "hiloCaptura.h"
#include "util.h"
#include "escena.h"
#include <QDebug>
#include "ventana.h"
#include "ui_ventana.h"

using namespace cv;
using namespace aruco;

#define CORRECCION_BOCA_ABIERTA 1.2
#define CORRECCION_LINEA_SUPERIOR_BOCA 1.1
#define COMP_Lab 1


float angulo(Point p1, Point p2)
{
    float adyacente = p2.x - p1.x;
    float opuesto;
    if (p2.y < p1.y) opuesto = p1.y - p2.y;
    else opuesto = p2.y - p1.y;
    float ang = atan(opuesto/adyacente);
//    qDebug() << "Angulo entre ojos en grados: " << ang*57.2957795 << "°";
    return ang;
}

Point inferiorDe(Point ojo, float ang, bool caso)
{
    int y = ojo.y + 400;
    int x;
    if(caso == 0) x = ojo.x + (tan(ang)*y);
    else x = ojo.x - (tan(ang)*y);
    return Point(x, y);
}

Point puntoNuevo(Point ojo, float ang, bool caso, int dist)
{
    int x;
    if (caso == 0) x =  ojo.x + sin(ang) * dist;
    else x =  ojo.x - sin(ang) * dist;
    int y = ojo.y + cos(ang) * dist;
    return Point(x, y);
}



HiloCaptura::HiloCaptura(QObject * parent) : activo(false), camaraActiva(0), detectorQRactivo(false),
    detectorRostroActivo(false), detectorPlanosActivo(false), markerDetectado(0), qrDetectado(0)
{
    this->ventana = (Ventana *)parent;

    if (Configuracion::tipoCamara.contains("webcam1", Qt::CaseInsensitive))  {
        cap = new VideoCapture(0);  qDebug() << "webcam1";
        camaraActiva = 0;
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }
    else if (Configuracion::tipoCamara.contains("webcam2", Qt::CaseInsensitive))  {
        cap = new VideoCapture(1);  qDebug() << "webcam2";
        camaraActiva = 1;
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }
    else if (Configuracion::tipoCamara.contains("webcam3", Qt::CaseInsensitive))  {
        cap = new VideoCapture(2);
        camaraActiva = 2;
        cap->set(CV_CAP_PROP_FRAME_WIDTH, RESOLUTION_W);
        cap->set(CV_CAP_PROP_FRAME_HEIGHT, RESOLUTION_H);
    }

    this->frame.create(Size(RESOLUTION_W, RESOLUTION_H), CV_8UC3);

    camParam = new CameraParameters;
    mDetector = new MarkerDetector;
    markers = new vector<Marker>;

//    this->leerBaseCargarPersonas();

}

HiloCaptura::~HiloCaptura()  {
    delete cap;
}


void HiloCaptura::slot_cambiarCamara()  {
    this->finalizarHilo();
    this->wait(500);  // Esperamos 500 milisegundos maximo para que finalice el hilo y podamos cambiar de camara

    do  {
        if (camaraActiva>=2)
            camaraActiva=-1;
        cap->open(++camaraActiva);
    }
    while(!cap->isOpened());

    this->start();
}

void HiloCaptura::run()  {

    this->activo = true;

    if( !cap->isOpened() )  {
        qDebug() << "La camara con VideoCapture no pudo ser iniciada!!";
        return;
    }

    try  {
        // En este archivo hay algunos buenos parametros para tocar. Sobre todo nframes
        // Hace que se detecte de mejor manera el marcador. Por defecto estaba en 5.
        // En 15 mejora mucho. En 50 es casi lo mismo que con 50.
        camParam->readFromXMLFile(Configuracion::carpetaDeTrabajo.toStdString() + "/intrinsics.yml");
    }
    catch (cv::Exception e)  {
        qDebug() << "Exception:" << e.what();
        qDebug() << "Ojo que puede ser que no encuentre el archivo intrinsics.yml";
        qDebug() << "En mi caso lo tuve que ubicar en la carpeta build-desktop fuera de Debug y Release.";
    }

    float markerSize = 0.05f;  // Con esto se indica que el marcador es de 5 cm

    while(activo)  {

//        QThread::msleep(20);

        cap->operator >>(frame);



        /////////////////////////////////////////////////////////// SEGMENTAR ROSTRO
        if (ventana->ui->cbSegmentarRostro->isChecked())  {

            vector<Rect> faces;
            faces = detectorCara.detectFacesRect(frame);

            if(faces.size() > 0)   {  // Encontramos una cara

                Rect roiFace(faces.at(0).x, faces.at(0).y, faces.at(0).width, faces.at(0).height);
                Mat frameFace(frame, roiFace);

                frameFace.copyTo(frame);



//                cv::Rect roi( cv::Point( originX, originY ), smallImage.size() );
//                smallImage.copyTo( bigImage( roi ) );

            }
        }
        /////////////////////////////////////////////////////////// SEGMENTAR ROSTRO ---------- FIN




        /////////////////////////////////////////////////////////// DETECCION DE MARCADORES
//        if ( markerDetectado-- <= 0)  {
            camParam->resize(frame.size());
            mDetector->detect(frame, *markers, *camParam, markerSize);

            ventana->escena->setMarker(markers);
            ventana->escena->setCameraParam(camParam);

            // markers tiene la cantidad de marcadores detectados.
//            if ( markers->size() > 0 )
//                markerDetectado = 4;  // Este valor es para que en 4 ciclos no entre a este if y no detecte nada
//        }
        /////////////////////////////////////////////////////////// DETECCION DE MARCADORES ------------ FIN



        ventana->escena->setImage(frame);
    }
}

void HiloCaptura::finalizarHilo()  {
    this->activo = false;
}


// Este metodo lee la base de datos y carga el vector de Personas. Esto es para poder identificarlos si aparecen
// en la escena y obtener sus datos de perfil.
void HiloCaptura::leerBaseCargarPersonas()  {
//    if ( ! adminDB.conectar("tubot"))  {
//        qDebug() << "Conexion a la base fallida";
//        return;
//    }

//    QString comando_sql = "SELECT nombre, apellido, mail, url_linkedin FROM personas";
//    QVector<QStringList> vDatos = adminDB.ejecutarComandoSelect(comando_sql);



//    for (int i=0 ; i<vDatos.size() ; i++)  {
//        qDebug() << vDatos.at(i);
//        Persona persona;
//        persona.setNombre(vDatos.at(i).at(0) + " " + vDatos.at(i).at(1));
//        persona.setMail(vDatos.at(i).at(2));
//        persona.setLinkedin(vDatos.at(i).at(3));

//        this->vPersonas.push_back(persona);
//    }

//    escena->setPersonas(vPersonas);
}
