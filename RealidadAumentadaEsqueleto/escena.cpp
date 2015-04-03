
#include "escena.h"

Escena::Escena() : cantidadTexturasDirectorio(0), texturaClienteActual(0)
{
    videoCapture = new cv::VideoCapture(0);

    timerEscena = new QTimer;
    timerEscena->start(10);
    connect(timerEscena, SIGNAL(timeout()), SLOT(slot_actualizarEscena()));
}

void Escena::initializeGL()
{   
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    texturas[0].nombreTextura = "TexturaCamara";
    glGenTextures(1, &texturas[0].idTextura);

    // Siempre llamar a leerDirectorioTexturas desde initializeGL()
    leerDirectorioTexturas();

    // Descargamos una imagen de internet como EJEMPLO
    ClienteUrl *cliente = new ClienteUrl;
    cliente->descargar("http://k31.kn3.net/C18B16126.jpg");
    connect(cliente, SIGNAL(descargaFinalizada(QByteArray)), SLOT(slot_cargarTexturaCliente(QByteArray)));
}

void Escena::leerDirectorioTexturas()
{
    // Creamos texturas para todas las imagenes ubicadas en ../RealidadAumentada/Texturas/

    QDir directorioTexturas(Configuracion::carpetaDeTrabajo + "/Texturas");

    QStringList filtroArchivos;
    filtroArchivos << "*.jpg" << "*.png" << "*.bmp" << "*.gif";

    QStringList archivosImagen = directorioTexturas.entryList(filtroArchivos);

    cantidadTexturasDirectorio = archivosImagen.size();

    for (int i=0 ; i<archivosImagen.size() ; i++)
    {
        glGenTextures(1, &texturas[i+1].idTextura);

        texturas[i+1].nombreTextura = archivosImagen.at(i);

        QString imagenActual = Configuracion::carpetaDeTrabajo + "/Texturas/" + archivosImagen.at(i);
        const char *charImagenActual = imagenActual.toStdString().c_str();

        Mat matImagen = imread( charImagenActual, CV_LOAD_IMAGE_COLOR );
        if(matImagen.data)
        {
            texturas[i+1].matTextura = matImagen.clone();
            texturas[i+1].generarDesdeMat();
        }
    }
}

void Escena::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Escena::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, RESOLUCION_ANCHO, 0, RESOLUCION_ALTO, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);

    // Dibujamos imagen de la Camara

    glBindTexture(GL_TEXTURE_2D, texturas[0].idTextura);

    glBegin(GL_QUADS);

        glTexCoord2f(0, 0);     glVertex3f(0, RESOLUCION_ALTO, -999);
        glTexCoord2f(1, 0);     glVertex3f(RESOLUCION_ANCHO, RESOLUCION_ALTO, -999);
        glTexCoord2f(1, 1);     glVertex3f(RESOLUCION_ANCHO, 0, -999);
        glTexCoord2f(0, 1);     glVertex3f(0, 0, -999);

    glEnd();


    // Dibujamos dos imagenes de EJEMPLO
    dibujarImagen("Paisaje1.jpg", 90, 100, -500, 150, 100);
    dibujarImagen("Paisaje2.jpg", 400, 100, -500, 150, 100);

    // Dibujamos una imagen de Internet de Ejemplo
    dibujarImagen("TexturaInternet0", 245, 300, -500, 150, 100);


    glDisable(GL_TEXTURE_2D);

    glFlush();
}

void Escena::slot_cargarTexturaCliente(QByteArray datos)
{
    // Este metodo deberia estar conectado con una señal de ClienteUrl
    // Genera una textura desde un QByteArray descargado de Internet
    // Y le asigna automaticamente el nombre "TexturaInternet + NumeroDeTextura"

    glGenTextures(1, &texturas[texturaClienteActual+cantidadTexturasDirectorio+1].idTextura);

    texturas[texturaClienteActual+cantidadTexturasDirectorio+1].nombreTextura = "TexturaInternet" + QString::number(texturaClienteActual);

    if (!datos.isEmpty())
    {
        // Arreglar esta conversion de QByteArray a Mat, esta como muestra rapida (No muestra bien los colores)

        QImage imagenInternet = QImage::fromData(datos);

        Mat matImagenInternet( imagenInternet.height(),
                               imagenInternet.width(),
                               CV_8UC4,
                               const_cast<uchar*>(imagenInternet.bits()),
                               imagenInternet.bytesPerLine() );

        texturas[texturaClienteActual+cantidadTexturasDirectorio+1].matTextura = matImagenInternet.clone();
        texturas[texturaClienteActual+cantidadTexturasDirectorio+1].generarDesdeMat();
        texturaClienteActual++;
    }
}

void Escena::dibujarImagen(QString nombreTextura, float x, float y, float z, float ancho, float alto)
{
    // Modificar este metodo, solo de ejemplo para mostrar imagenes

    int idTexturaActual = -1;

    for(int i=0; i<CANTIDAD_TEXTURAS; i++)
    {
        if(nombreTextura == texturas[i].nombreTextura)
        {
            idTexturaActual = texturas[i].idTextura;
        }
    }

    if (idTexturaActual == -1)
    {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, idTexturaActual);

    glBegin(GL_QUADS);

        glTexCoord2f(0, 0);     glVertex3f(x, y+alto, z);
        glTexCoord2f(1, 0);     glVertex3f(x+ancho, y+alto, z);
        glTexCoord2f(1, 1);     glVertex3f(x+ancho, y, z);
        glTexCoord2f(0, 1);     glVertex3f(x, y, z);

    glEnd();
}

void Escena::slot_actualizarEscena()
{
    // Por cada timeout de timerEscena, genera una textura de camara nueva

    videoCapture->operator >>(texturas[0].matTextura);
    texturas[0].generarDesdeMat();
    this->repaint();
}
