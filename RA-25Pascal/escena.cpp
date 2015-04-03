#include "escena.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "util.h"
#include <QMouseEvent>
#include <vector>
#include <QDir>
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include "threedwriter.h"
#include <QProcess>
#include "ventana.h"

#include "ui_ventana.h"

#include "smtp.h"

Escena::Escena(QWidget * parent) : QGLWidget(parent), imagenNueva(false), camaraActiva(false),
    redimensionConTeclas(0.04f), realidadActiva(true), widgetTexturizado(false), widgetTexturizadoPedido(false),
    escenaActiva(true), alMenosUnaVistaStreetView(false), videoPlayed(false)
{

    this->ventana = (Ventana*)parent;

//    this->setFixedSize(320, 240);
    this->resize(RESOLUTION_W * 1.4f, RESOLUTION_H * 1.4f);

    // Este timer esta porque no se puede llamar a actualizacion de escena desde el hilo.
    timer = new QTimer(this);
    timer->setInterval(10);

    connect(timer, SIGNAL(timeout()), this, SLOT(slot_actualizarEscena()));

    // Estas lineas son para despues evitar el uso de puntero nulo.
    this->markers = NULL;
    this->camParam = NULL;


    // Esto es para la caja con textura e iluminacion
    LightAmbient[0]= 0.5f;
    LightAmbient[1]= 0.5f;
    LightAmbient[2]= 0.5f;
    LightAmbient[3]= 1.0f;

    LightDiffuse[0]= 1.0f;
    LightDiffuse[1]= 1.0f;
    LightDiffuse[2]= 1.0f;
    LightDiffuse[3]= 1.0f;

    LightPosition[0]= 0.0f;
    LightPosition[1]= 0.0f;
    LightPosition[2]= 2.0f;
    LightPosition[3]= 1.0f;

    this->paraIntercambiar = 0;

    this->leerDirectorioCrearCModel3DS();
    this->leerDirectorioTexturas();

    this->abrirPuerto("3");

    seguirMarcadorConServo = true;

    valorServoVertical = 0;

    connect(&navegador, SIGNAL(signal_sitioCargado()), this, SLOT(slot_navegadorLoadFinished()));


    bool isConectado = adminDB.conectar(Configuracion::carpetaDeTrabajo + "/db/base.accdb");
    if (isConectado)
        qDebug() << "Se conecto al archivo access";


    connect(&clienteURL, SIGNAL(signal_recursoDescargado()), this, SLOT(slot_cargarTexturaGoogleStreetView()));

//    video = new Video("25UBP.mpg");
    video = new Video("prueba.mp4");

}

// Lee la carpeta texturas y obtiene el listado de archivos y tambien prepara las texturas en OpenGL
void Escena::leerDirectorioTexturas()  {
    // Crear dir no nos cambia el directorio actual de trabajo
    QDir dir(Configuracion::carpetaDeTrabajo + "/texturas");

    QStringList filtroArchivos;
    filtroArchivos << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
    QStringList archivosImagen = dir.entryList(filtroArchivos);

    for (int i=0 ; i<archivosImagen.size() ; i++)  {
        TexturaComun *texturaComun = new TexturaComun(Configuracion::carpetaDeTrabajo + "/texturas/" + archivosImagen.at(i));

        vTexturas.push_back(texturaComun);
    }
}

// Lee la carpeta modelos y obtiene el listado de archivos 3ds y tambien crea los objetos CModel3DS y los agrega al QVector
void Escena::leerDirectorioCrearCModel3DS()  {

    // Crear dir no nos cambia el directorio actual de trabajo
    QDir dir(Configuracion::carpetaDeTrabajo + "/modelos");

    QStringList filtroArchivos;
    filtroArchivos << "*.3ds";
    QStringList archivos3ds = dir.entryList(filtroArchivos);

    for (int i=0 ; i<archivos3ds.size() ; i++)  {
        CModel3DS * model = new CModel3DS(Configuracion::carpetaDeTrabajo.toStdString() + "/modelos/" + archivos3ds.at(i).toStdString());
        vModels.push_back(model);
    }
}

void Escena::iniciarTemporizador()  {
    timer->start();
    this->camaraActiva = true;
}

void Escena::detenerTemporizador()  {
    timer->stop();
    this->camaraActiva = false;
}

void Escena::slot_pedirRecurso()  {
    QString str = "http://maps.googleapis.com/maps/api/streetview?size=400x400&sensor=false&location=";
    clienteURL.pedirRecurso(str + ventana->ui->leVisitarDomicilio->text()
                            + "&heading=" + ventana->ui->leRotacion->text());
}

void Escena::slot_cargarTexturaGoogleStreetView()  {
    imageGoogleStreetView = QGLWidget::convertToGLFormat( clienteURL.getImage() );
    texturaGoogleStreetView = imageGoogleStreetView.bits();

    qDebug() << "imageGoogleStreetView" << imageGoogleStreetView.width() << imageGoogleStreetView.height();

    glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + vModels.size() + 1]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageGoogleStreetView.width(), imageGoogleStreetView.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texturaGoogleStreetView);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    alMenosUnaVistaStreetView = true;
}


Escena::~Escena()  {
    for (signed int i=0 ; i<vModels.size() ; i++)  {
        glDeleteBuffers(1, &vModels.at(i)->m_VertexVBO);
        glDeleteBuffers(1, &vModels.at(i)->m_NormalVBO);
        glDeleteBuffers(1, &vModels.at(i)->m_TexCoordVBO);
    }
}

/**
  * Este metodo es llamado desde el hilo. Debe tener total independencia con respecto al momento en que se
  * actualiza la escena, es por ello que tenemos un timer para actualizar.
  */
void Escena::setImage(cv::Mat & frame)  {

    if(!imagenNueva)  {

        this->frame = frame;

//        Mat roi(iplImageCamara, Rect(125, 0, 380, 285));
//        cv::resize(roi, roi, Size(640, 480));
//        iplImageCamara = roi;

        this->imagenNueva = true;

        if (ventana->ui->cbSeguirRostro->isChecked())  {

            vector<Rect> faces;
            faces = detectorCara.detectFacesRect(this->frame);
            detectorCara.drawMultipleRect(faces, this->frame);

            if(faces.size() > 0)   {  // Encontramos una cara
                Rect rectCara = faces.at(0);
                QPoint centroCara(rectCara.x + rectCara.width/2, rectCara.y + rectCara.height/2);

                actualizaServoSiCorresponde(centroCara.x(), centroCara.y(), 50);
                qDebug() << "rostro detectado" << centroCara;
            }
        }
    }
}

/**
  * Le pasamos los marcadores detectados para que dibuje lo que quiera.
  */
void Escena::setMarker(std::vector<Marker> *markers)  {
    this->markers = markers;
}

void Escena::setCameraParam(CameraParameters* camParam)  {
    this->camParam = camParam;
}

void Escena::initializeGL()  {
    // A esto lo pide la clase QGLFunctions
    this->initializeGLFunctions();

//    glClearColor(1, 1, 1, 0);
    glClearColor(0, 0, 0, 0);

    glShadeModel(GL_SMOOTH); // Type of shading for the polygons
    glEnable(GL_DEPTH_TEST);

    int cantidadTexturas3ds = vModels.size();
    int cantidadTexturasComunes = vTexturas.size();
    int cantidadTexturasGoogleStreetView = 2;
    int cantidadCredenciales = 1;

    glGenTextures(1 + cantidadTexturasComunes + cantidadTexturas3ds
                  + cantidadTexturasGoogleStreetView + cantidadCredenciales, idTextura);
    // idTextura[0] - Imagen de la camara
    // idTextura[1] - Primer textura comun
    // ...
    // idTextura[cantidadTexturas] - Ultima textura comun
    // idTextura[cantidadTexturas + 1] - Primer textura de model
    // ...
    // idTextura[cantidadTexturas + cantidadModelos] - Ultima textura de modelos
    // idTextura[cantidadTexturas + cantidadModelos + cantidadTexturasGoogleStreetView] - Textura de Google Street View

    this->prepararImagenesTexturasComunes();

    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
    glEnable(GL_LIGHT1);

    // Aqui llamamos a este metodo para que prepare y tenga cargados en memoria todos los modelos 3ds.
    this->prepararModelos();

    glGenTextures(10, idTexturaWidget);
    // Generamos identificadores para 10 texturas. Posiblemente no se usen pero se dejan generados

}

void Escena::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
}

void Escena::paintGL()  {
//    glutWireTorus(0.01,1.5,1,75);
    // We clear both the color and the depth buffer so to draw the next frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if ( ! escenaActiva)  // No muestra nada en la escena.
        return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, RESOLUTION_W, 0, RESOLUTION_H, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (camaraActiva)  {

        // Esto es para que la imagen de la realidad no se vea y podamos proyectarlo con un proyector, claro
        if (realidadActiva)  {

            glBindTexture(GL_TEXTURE_2D, idTextura[0]);

            glEnable(GL_TEXTURE_2D);
            glColor3f(1, 1, 1);
            glBegin(GL_QUADS);
                glTexCoord2f(0, 1);     glVertex3f(0, 0, -999);
                glTexCoord2f(0, 0);     glVertex3f(0, RESOLUTION_H, -999);
                glTexCoord2f(1, 0);     glVertex3f(RESOLUTION_W, RESOLUTION_H, -999);
                glTexCoord2f(1, 1);     glVertex3f(RESOLUTION_W, 0, -999);
            glEnd();

            // Si la textura se deshabilitara al final de este metodo. El color que seteemos se visualiza mal.
            glDisable(GL_TEXTURE_2D);
        }


        if (this->camParam != NULL && this->markers != NULL)  {


            // Dentro de este for podemos usar la perspectiva glOrtho
            for(unsigned int m=0 ; m<this->markers->size() ; m++)  {

                // Dentro de este switch podemos dibujar sobre la escena segun:
                // glOrtho(0, RESOLUTION_W, 0, RESOLUTION_H, 1, 1000);
                switch(this->markers->at(m).id)  {

                glLoadIdentity();

                case 102:

                    // Entra a este if cuando fue pedido a internet (hay que esperar para que se descargue)
                    if ( ! this->widgetTexturizadoPedido)  {

                        // El recorte de linkedin tiene ancho=590 y alto=195

                        if (vPersonas.size()>0)
                            navegador.cargarSitio(vPersonas.at(0).getLinkedin());
//                        navegador.cargarSitio("http://ar.linkedin.com/in/cesarosimani");
//                        navegador.cargarSitio("http://mx.linkedin.com/in/drleonardogarrido");
//                        navegador.cargarSitio("http://mx.linkedin.com/pub/juan-carlos-aguilera/92/287/651");
                        this->widgetTexturizadoPedido = true;
                    }

                    // Entra a este if cuando fue pedido y ademas ya se haya descargado
                    if (this->widgetTexturizado)  {
                        int anchoLinkedin = 590/2, altoLinkedin = 195/2;

                        glBindTexture(GL_TEXTURE_2D, idTexturaWidget[0]);

                        glEnable(GL_TEXTURE_2D);
                        glColor3f(1, 1, 1);
                        glBegin(GL_QUADS);
                            glTexCoord2f(0, 0);     glVertex3f(0, 0, -998);
                            glTexCoord2f(0, 1);     glVertex3f(0, altoLinkedin, -998);
                            glTexCoord2f(1, 1);     glVertex3f(anchoLinkedin, altoLinkedin, -998);
                            glTexCoord2f(1, 0);     glVertex3f(anchoLinkedin, 0, -998);
                        glEnd();

                        // Si la textura se deshabilitara al final de este metodo. El color que seteemos se visualiza mal.
                        glDisable(GL_TEXTURE_2D);
                    }
                    break;

                default:;
                }
            }


            ///Set the appropriate projection matrix so that rendering is done in a enrvironment
            //like the real camera (without distorsion)
            glMatrixMode(GL_PROJECTION);
            double proj_matrix[16];

            cv::Size2i sizeEscena(RESOLUTION_W, RESOLUTION_H);
            cv::Size2i sizeVentanaOpenGL(RESOLUTION_W, RESOLUTION_H);

//            cv::Size2i sizeEscena(380, 285);
//            cv::Size2i sizeVentanaOpenGL(380, 285);

//            Mat roi(iplImageCamara, Rect(135, 5, 365, 285));
//            cv::resize(roi, roi, Size(640, 480));

            // Las siguientes 3 lineas se obtiene la matriz de perspectiva y despues se aplica
            // Es decir, el glOrtho ya no tiene vigencia a partir de aqui.
            // Deprecada y reemplzada por el m´etodo glGetProyectMatrix de la clase CameraParameters
//            MarkerDetector::glGetProjectionMatrix(*this->camParam, sizeEscena, sizeVentanaOpenGL, proj_matrix, 0.05, 10);
            this->camParam->glGetProjectionMatrix(sizeEscena, sizeVentanaOpenGL, proj_matrix, 0.05, 10);
            glLoadIdentity();
            glLoadMatrixd(proj_matrix);


            glMatrixMode(GL_MODELVIEW);

            //now, for each marker,
            double modelview_matrix[16];


            for(unsigned int m=0 ; m<this->markers->size() ; m++)  {


                // En las siguientes 3 lines obtenemos la matriz para llevar el centro del universo
                // a la posicion del marcador, cargamos la matriz identidad y cargamos esa matriz
                this->markers->at(m).glGetModelViewMatrix(modelview_matrix);
                glLoadIdentity();
                glLoadMatrixd(modelview_matrix);

//                glTranslated(-0.125, 0.01, 0);




//                glRotated(270, 1, 0, 0);  // Hago esta rotacion para que corresponda x y z como estoy acostumbrado
                // Si este rotate no se hace entonces el papel blanco queda bien. Hay que analizar esto.

                // A partir de este momento situados en el centro del marcador y con el plano como se debe.

    // markers es un vector<Marker> y Marker tiene un atributo id que es un entero que identifica a cada Marker
    // Entonces aca podemos con un switch y segun el id que este al frente de la camara dibujamos distintos objetos 3D

                qDebug() << "id del Marker" << this->markers->at(m).id;

                ThreeDWriter textoParaFolleto(100, "Arial", -0.2f+0.030f, 0.020f, -0.009f, 0.3f, 0.02f, 0.0001f);
                ThreeDWriter textoParaCredencial(80, "Arial", -0.04f, 0.045f, -0.001f, 0.3f, 0.02f, 0.0001f);

                QString comando_sql;
                QString textoParaMostrar;
                QVector<QStringList> vDatos;

                switch(this->markers->at(m).id)  {

                /////////////// LAS 4 CAJITAS JUNTAS
                case 461:
                    dibujarCaja("caja");
                    break;
                case 233:
                    dibujarCaja("cajaAcero");
                    break;
                case 684:
                    dibujarCaja("conaiisi", 0.035f);
                    break;
                case 175:
                    dibujarCaja("cajaDanger");
                    break;
                /////////////// LAS 4 CAJITAS JUNTAS /////// FIN


                ///////////////  8 MARCADORES y 1 DOBLADO
                case 791:
                    dibujarCaja("cajaTIC");
                    break;
                case 277:  // Folleto de Informatica
                    dibujarVideo(0.080f);
                    break;
                case 181:
                    dibujarCaja("cajaTIC_UBP");
                    break;
                case 717:
                    dibujarCaja("pared", 0.035f, true);
                    break;
//                case 125:
//                    dibujarCaja("cajaDanger");
//                    break;
//                case 477:  // Folleto de Informatica
////                    dibujarPapelParaEscribir(A6, 180);
//                    dibujarCaja("logo_U_B_P", 0.015f);
//                    glTranslated(-0.005f, 0, 0.015f);
//                    dibujarModel("man", 5);
//                    break;
                ///////////////  8 MARCADORES y 1 DOBLADO ////////// FIN

                case 177:
                    dibujarCaja("cajaDanger", 0.037f);
                    break;

                case 838:
                    dibujarCaja("caja", 0.037f);
                    break;

                case 982:
                    dibujarCaja("cajaDanger", 0.037f);
                    break;


                case 477:  // Caja tec
//                    glTranslated(-0.050f, 0, 0);
                    dibujarCaja("logoUBP", 0.037f);
                    break;



                case 299:
                    comando_sql = "SELECT sorteo FROM usuarios WHERE nombre = 'Cesar'";
                    vDatos = adminDB.ejecutarComandoSelect(comando_sql);

                    for (int i=0 ; i<vDatos.size() ; i++)  {
                        qDebug() << vDatos.at(i);
                        textoParaMostrar = vDatos.at(i).at(0);
                    }

                    glRotated(180, 0, 0, 5);
//                    textoParaCredencial.setIsMirroredWriting(true); //Para que se espeje el texto
                    textoParaCredencial.writeText(textoParaMostrar);
                    glRotated(180, 0, 0, 5);
                    dibujarCaja("PerfilCesar", 0.05f, true);
                    break;

                case 341:
                    comando_sql = "SELECT sorteo FROM usuarios WHERE nombre = 'House'";
                    vDatos = adminDB.ejecutarComandoSelect(comando_sql);

                    for (int i=0 ; i<vDatos.size() ; i++)  {
                        qDebug() << vDatos.at(i);
                        textoParaMostrar = vDatos.at(i).at(0);
                    }

                    glRotated(180, 0, 0, 5);
//                    textoParaCredencial.setIsMirroredWriting(true); //Para que se espeje el texto
                    textoParaCredencial.writeText(textoParaMostrar);
                    glRotated(180, 0, 0, 5);
                    dibujarCaja("PerfilHouse", 0.05f, true);
                    break;

                case 125:
                    comando_sql = "SELECT sorteo FROM usuarios WHERE nombre = 'Eva'";
                    vDatos = adminDB.ejecutarComandoSelect(comando_sql);

                    for (int i=0 ; i<vDatos.size() ; i++)  {
                        qDebug() << vDatos.at(i);
                        textoParaMostrar = vDatos.at(i).at(0);
                    }

                    glRotated(180, 0, 0, 5);
//                    textoParaCredencial.setIsMirroredWriting(true); //Para que se espeje el texto
                    textoParaCredencial.writeText(textoParaMostrar);
                    glRotated(180, 0, 0, 5);
                    dibujarCaja("PerfilEva", 0.05f, true);
                    break;

                case 64:  // Marcador que tiene que ser seguido

                    if (seguirMarcadorConServo)
                        actualizaServoSiCorresponde(this->markers->at(m).getCenter().x, this->markers->at(m).getCenter().y, 50);

                    glBegin(GL_LINES);
                        glColor3f (1, 1, 0);
                        glVertex3f( 0, 0,  0);
                        glVertex3f( 0, 100,  0);
                        glColor3f (1, 0, 1);
                        glVertex3f( 0, 0,  0);
                        glVertex3f( 100, 0,  0);
                        glColor3f (0, 1, 1);
                        glVertex3f( 0, 0,  0);
                        glVertex3f( 0, 0,  100);
                    glEnd();
                    break;





                case 882:  // Folleto de Informatica
//                    dibujarPapelParaEscribir(A6, 180);

                    dibujarCaja("logo_U_B_P", 0.035f);
                    glTranslated(-0.005f, 0, 0.035f);
                    dibujarModel("man", 10);
                    break;

                case 760:
                    glTranslated(0.2f, 0, 0);
//                    glTranslated(0.100f, -0.020f, -0.035f);
                    glRotated(-90, 0,1,0);
                    dibujarModel("Iphone", 130);
                    break;


                case 985:
                    glRotated(90, 0, 0, 5);
                    dibujarPapelParaEscribir(A5);

                    textoParaFolleto.setIsMirroredWriting(true); //Para que se espeje el texto
                    textoParaFolleto.writeText("Jornadas de puertas abiertas");

                    break;

                case 618:
                    dibujarPapelParaEscribir(A4);
                    textoParaFolleto.setIsMirroredWriting(true); //Para que se espeje el texto
                    textoParaFolleto.writeText("Plan de estudio actualizado");
                    break;

                // Para mostrar el Gogole Street View
                case 76:
                    dibujarVistaGoogleStreetView(0.05f);
                    break;

                default:
                    dibujarCaja("cajaDanger", 0.025f);

                    break;
                }
            }
        }

        glFlush();
    //    glutSwapBuffers();  // No funciono, siempre se me cierra el programa

    }
}

// Este metodo prepara cada uno de los modelos (objetos CModel3DS) que se encuentran en el QVector vModels
void Escena::prepararModelos()  {

    for (signed int i=0 ; i<vModels.size() ; i++)  {

        assert(vModels.at(i)->m_model != NULL);

        // Levanta la imagen para que quede disponible como textura
        this->prepararImagenModel();

        // Calculate the number of faces we have in total
        vModels.at(i)->GetFaces();

        // Allocate memory for our vertices and normals
        Lib3dsVector * vertices = new Lib3dsVector[vModels.at(i)->m_TotalFaces * 3];
        Lib3dsVector * normals = new Lib3dsVector[vModels.at(i)->m_TotalFaces * 3];
        Lib3dsTexel* texCoords = new Lib3dsTexel[vModels.at(i)->m_TotalFaces * 3];

        Lib3dsMesh * mesh;
        unsigned int finishedFaces = 0;

        // Loop through all the meshes
        for(mesh = vModels.at(i)->m_model->meshes ; mesh != NULL ; mesh = mesh->next)
        {
            lib3ds_mesh_calculate_normals(mesh, &normals[finishedFaces*3]);
            // Loop through every face

            for(unsigned int cur_face=0; cur_face < mesh->faces ; cur_face++)
            {
                Lib3dsFace * face = &mesh->faceL[cur_face];
                for(unsigned int i=0 ; i<3 ; i++)
                {
                    if(&mesh->texelL)  {
    //                    qDebug() << "Hay textura!";
                        memcpy(&texCoords[finishedFaces*3 + i], mesh->texelL[face->points[ i ]], sizeof(Lib3dsTexel));
                    }
                    else  {
                        qDebug() << "No hay textura!";
                    }
                    memcpy(&vertices[finishedFaces*3 + i], mesh->pointL[face->points[i]].pos, sizeof(Lib3dsVector));
                }
                finishedFaces++;
            }
        }

        // Generate a Vertex Buffer Object and store it with our vertices
        glGenBuffers(1, &vModels.at(i)->m_VertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vModels.at(i)->m_VertexVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * vModels.at(i)->m_TotalFaces, vertices, GL_STATIC_DRAW);

        // Generate another Vertex Buffer Object and store the normals in it
        glGenBuffers(1, &vModels.at(i)->m_NormalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vModels.at(i)->m_NormalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsVector) * 3 * vModels.at(i)->m_TotalFaces, normals, GL_STATIC_DRAW);

        // Generate a third VBO and store the texture coordinates in it.
        glGenBuffers(1, &vModels.at(i)->m_TexCoordVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vModels.at(i)->m_TexCoordVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Lib3dsTexel) * 3 * vModels.at(i)->m_TotalFaces, texCoords, GL_STATIC_DRAW);

        // Clean up our allocated memory
        delete vertices;
        delete normals;
        delete texCoords;

        // We no longer need lib3ds
        lib3ds_file_free(vModels.at(i)->m_model);
        vModels.at(i)->m_model = NULL;
    }
}

// Dibuja un modelo particular segun el parametro que se le pase a este metodo.
// El primer modelo es el 0 y el ultimo es el vModels.size()-1
// El numeroModelo es 1 para el primer archivo de imagen en la carpeta modelos, 2 para el segundo, etc
void Escena::dibujarModel(QString nombreArchivoModelo, int porcentajeEscala)  {

    bool nombreArchivoModeloEncontrado = false;
    int idTexturaEncontrada = -1;

    for (int i=0 ; i<vModels.size() ; i++)  {
        if (vModels.at(i)->getNombreArchivoId().contains(nombreArchivoModelo, Qt::CaseInsensitive))  {
            nombreArchivoModeloEncontrado = true;
            idTexturaEncontrada = i;  // Guardamos el idTextura
            break;
        }
    }

    // Este if es para evitar que se quiera dibujar un modelo que no existe.
    // Si se intenta hacer esto retorna y no dibuja nada
    if (!nombreArchivoModeloEncontrado)
        return;

    // Draw our model
    assert(vModels.at(idTexturaEncontrada)->m_TotalFaces != 0);

    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + idTexturaEncontrada]);

    float escala = this->redimensionConTeclas * porcentajeEscala / 100;
    glScalef(escala, escala, escala);

    // Enable vertex and normal arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Bind the vbo with the normals
    glBindBuffer(GL_ARRAY_BUFFER, vModels.at(idTexturaEncontrada)->m_NormalVBO);

    // The pointer for the normals is NULL which means that OpenGL will use the currently bound vbo
    glNormalPointer(GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vModels.at(idTexturaEncontrada)->m_TexCoordVBO);
    glTexCoordPointer(2, GL_FLOAT, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vModels.at(idTexturaEncontrada)->m_VertexVBO);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    // Render the triangles
    glDrawArrays(GL_TRIANGLES, 0, vModels.at(idTexturaEncontrada)->m_TotalFaces * 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

// Aca cargamos la textura (solo una por modelo) de cada uno de los modelos 3ds
void Escena::prepararImagenModel()  {

    for (signed int i=0 ; i<vModels.size() ; i++)  {
        QString nombreArchivo = vModels.at(i)->getNombreArchivoSinExtension() + "jpg";

        if (!imageModel.load(nombreArchivo))  {
            QMessageBox::critical(this, "Recurso no disponible en " + QDir::currentPath(), "Imagen " + nombreArchivo);
            return;
        }

        imageModel = QGLWidget::convertToGLFormat( imageModel );
        texturaModel = imageModel.bits();

        // 1 por la textura de la camara + la cantidad de texturas comunes
        glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + i]);

        // Seteamos el idTextura para que dentro del objeto CModel3DS tambien este disponible el idTextura
        // Esto nos permitir[a luego poder identificar la textura con el nombre del archivo
        vModels.at(i)->setIdTexturaModel(1 + vTexturas.size() + i);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageModel.width(), imageModel.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texturaModel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}


// Es el reemplazo de preparaImagenCamara() pero que no pasa la imagen por QImage, va directo
void Escena::createTextureFromCurrentImg()  {
    glBindTexture( GL_TEXTURE_2D, idTextura[0] );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (Configuracion::tipoCamara.contains("kinect", Qt::CaseInsensitive))  {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.size().width,
                     frame.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
    }
    else  {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.size().width,
                     frame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    }


}


void Escena::prepararImagenCamara(QRect recorte)  {
    // Entra a este if cuando se quiere recortar la imagen de la camara segun el Qrect. Como para hacer zoom
    if (!recorte.isNull())  {
        imageCamara = imageCamara.copy(recorte.x(), recorte.y(), recorte.width(), recorte.height());
    }

    imageCamara = QGLWidget::convertToGLFormat(imageCamara);
    texturaCamara = imageCamara.bits();

    glBindTexture(GL_TEXTURE_2D, idTextura[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imageCamara.width(), imageCamara.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texturaCamara);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


void Escena::prepararImagenNavegador(QImage imagenNavegador)  {
    QImage imagenNavegadorGL = QGLWidget::convertToGLFormat(imagenNavegador);

    glBindTexture(GL_TEXTURE_2D, idTexturaWidget[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagenNavegadorGL.width(), imagenNavegadorGL.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imagenNavegadorGL.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Escena::prepararImagenVideo(QVideoFrame frameActual)  {
//    QImage imagenVideoGL = QGLWidget::convertToGLFormat(imagenVideo);

//    glBindTexture(GL_TEXTURE_2D, idTexturaWidget[1]);
//    glTexImage2D(GL_TEXTURE_2D, 0, 3, imagenVideoGL.width(), imagenVideoGL.height(),
//                 0, GL_RGBA, GL_UNSIGNED_BYTE, imagenVideoGL.bits());

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + vModels.size() + 2]);  // Activamos idTextura.

    glTexImage2D(GL_TEXTURE_2D, 0, 3, frameActual.width(), frameActual.height(),
                 0, GL_BGRA, GL_UNSIGNED_BYTE, frameActual.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//    QImage img( frameActual.bits(),
//                frameActual.width(),
//                frameActual.height(),
//                frameActual.bytesPerLine(),
//                QVideoFrame::imageFormatFromPixelFormat(frameActual.pixelFormat()));
//    img.save("frameActual.png");


}


void Escena::prepararImagenesTexturasComunes()  {

    for (signed int i=0 ; i<vTexturas.size() ; i++)  {
        if (!imageTexturaComun.load(vTexturas.at(i)->getNombreArchivoConPath()))  {
            QMessageBox::critical(this, "Recurso no disponible en " + QDir::currentPath(),
                                  "Imagen " + vTexturas.at(i)->getNombreArchivoConPath() );
            return;
        }

        imageTexturaComun = QGLWidget::convertToGLFormat( imageTexturaComun );
        texturaComun = imageTexturaComun.bits();

        glBindTexture(GL_TEXTURE_2D, idTextura[1 + i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageTexturaComun.width(), imageTexturaComun.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texturaComun);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}



void Escena::slot_actualizarEscena()  {
    if (imagenNueva)  {

        if (realidadActiva)  {
//            this->prepararImagenCamara();
            this->createTextureFromCurrentImg();
        }

        imagenNueva = false;
        this->updateGL();
    }
}






// El numeroTexturaComun es 1 para el primer archio de imagen en la carpeta texturas, 2 para el segundo, etc
void Escena::dibujarCaja(QString nombreTexturaComun, float sizeLadoCaja, bool isPapel)  {

    bool nombreArchivoEncontrado = false;
    int idTexturaEncontrada = -1;

    for (int i=0 ; i<vTexturas.size() ; i++)  {
        if (vTexturas.at(i)->getNombreArchivoId().contains(nombreTexturaComun, Qt::CaseInsensitive))  {
            nombreArchivoEncontrado = true;

            idTexturaEncontrada = i;  // Guardamos el idTextura

            // Con esto todas cajas con texturas en un archivo de imagen que diga UBP, entonces sera rotado
            if (nombreTexturaComun.contains("UBP", Qt::CaseInsensitive))  {
                vTexturas.at(idTexturaEncontrada)->incrementarAnguloRotacion(4);
            }
            break;
        }
    }

    // Este if es para evitar que se quiera dibujar un modelo que no existe.
    // Si se intenta hacer esto retorna y no dibuja nada
    if (!nombreArchivoEncontrado)
        return;


    glEnable(GL_TEXTURE_2D);

    // Tener en cuenta que para textura con id 0 es la de la camara
    glBindTexture(GL_TEXTURE_2D, idTextura[1 + idTexturaEncontrada]);

    glColor3f (1, 1, 1);

    // Esta traslacion es para que la caja quede encima del marcador.
    glTranslatef(0, 0, sizeLadoCaja);

    glRotated(vTexturas.at(idTexturaEncontrada)->getAnguloActual(), 0, 0, 5);

    // Si es papel ejecuta este if y retorna del metodo
    if (isPapel)  {
        glBegin(GL_QUADS);

        glNormal3f( 0.0f, 0.0f,-1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        return;
    }

    // Dibujo de la caja con textura
    glEnable( GL_LIGHTING );  // Para la caja con textura


    glBegin(GL_QUADS);
    // Front Face
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    // Back Face
    glNormal3f( 0.0f, 0.0f,-1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    // Top Face
    glNormal3f( 0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    // Bottom Face
    glNormal3f( 0.0f,-1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    // Right face
    glNormal3f( 1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja,  sizeLadoCaja);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
    glEnd();

    glDisable( GL_LIGHTING );  // Para la caja con textura

    glDisable(GL_TEXTURE_2D);

}

// Este metodo es para dibujar sobre el marcador la imagen de Google Street View
void Escena::dibujarVistaGoogleStreetView(float sizeLadoCaja)  {

    if (!alMenosUnaVistaStreetView)
        return;

    glEnable(GL_TEXTURE_2D);

    // Tener en cuenta que para textura con id 0 es la de la camara
    glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + vModels.size() + 1]);

    glColor3f (1, 1, 1);

    // Esta traslacion es para que la caja quede encima del marcador.
    glTranslatef(0, 0, sizeLadoCaja);

//    glRotated(vTexturas.at(idTexturaEncontrada)->getAnguloActual(), 0, 0, 5);

    glBegin(GL_QUADS);

    glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Este metodo es para dibujar sobre el marcador el video que se esta reproduciendo
void Escena::dibujarVideo(float sizeLadoCaja)  {

    if (!videoPlayed)  {
        video->reproducir();
        videoPlayed = true;
    }
    else  {
        QVideoFrame imagenInstantaneaVideo = video->getImagenVideo();

        qDebug() << "imagenInstantaneaVideo" << imagenInstantaneaVideo.width() << imagenInstantaneaVideo.height();

        prepararImagenVideo(imagenInstantaneaVideo);

    }

    glEnable(GL_TEXTURE_2D);

    // Tener en cuenta que para textura con id 0 es la de la camara
    glBindTexture(GL_TEXTURE_2D, idTextura[1 + vTexturas.size() + vModels.size() + 2]);

    glColor3f (1, 1, 1);

    // Esta traslacion es para que la caja quede encima del marcador.
    glTranslatef(0, 0, sizeLadoCaja);

//    glRotated(vTexturas.at(idTexturaEncontrada)->getAnguloActual(), 0, 0, 5);

    glBegin(GL_QUADS);

    glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-sizeLadoCaja,  sizeLadoCaja, -sizeLadoCaja);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-sizeLadoCaja, -sizeLadoCaja, -sizeLadoCaja);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Escena::dibujarPapelParaEscribir(SizePapel sizePapel, int anguloRotacionEnZ)  {

    float sizeLadoMarcador = 0.025f;
    float sizeAnchoA4 = 0.025f * 8.5f;
    float sizeLargoA4 = 0.025f * 12.66f;
    float sizeAnchoA5 = 0.025f * 8.5f;
    float sizeLargoA5 = 0.025f * 12.66f / 2;
    float sizeAnchoA6 = 0.025f * 8.5f / 2;
    float sizeLargoA6 = 0.025f * 12.66f / 2;

    glRotated(anguloRotacionEnZ, 0, 0, 5);

    glColor3f (1, 1, 1);

    glBegin(GL_QUADS);

    switch(sizePapel)  {
    case SizeMarcador:
        glVertex3f(-sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeLadoMarcador, sizeLadoMarcador,  0);
        glVertex3f(-sizeLadoMarcador, sizeLadoMarcador,  0);
        break;

    case A4:
        glVertex3f(-sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeAnchoA4-sizeLadoMarcador, -sizeLadoMarcador,  0);
        glVertex3f( sizeAnchoA4-sizeLadoMarcador, sizeLargoA4-sizeLadoMarcador, 0);
        glVertex3f( -sizeLadoMarcador, sizeLargoA4-sizeLadoMarcador,  0);
        break;

    case A5:
        glVertex3f(-sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeAnchoA5-sizeLadoMarcador, -sizeLadoMarcador,  0);
        glVertex3f( sizeAnchoA5-sizeLadoMarcador, sizeLargoA5-sizeLadoMarcador, 0);
        glVertex3f( -sizeLadoMarcador, sizeLargoA5-sizeLadoMarcador,  0);
        break;

    case A6:
        glVertex3f(-sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeAnchoA6-sizeLadoMarcador, -sizeLadoMarcador,  0);
        glVertex3f( sizeAnchoA6-sizeLadoMarcador, sizeLargoA6-sizeLadoMarcador, 0);
        glVertex3f( -sizeLadoMarcador, sizeLargoA6-sizeLadoMarcador,  0);
        break;

    default:
        glVertex3f(-sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeLadoMarcador, -sizeLadoMarcador, 0);
        glVertex3f( sizeLadoMarcador, sizeLadoMarcador,  0);
        glVertex3f(-sizeLadoMarcador, sizeLadoMarcador,  0);
        break;
    }

    glEnd();
}


void Escena::actualizaServoSiCorresponde(int xActualMarcador, int yActualMarcador, int franjaCentral = 30)  {
    qDebug() << "Actualizar servo" << xActualMarcador << yActualMarcador;

    if (xActualMarcador > 0 && xActualMarcador < RESOLUTION_W)  {  // Si cae fuera de esto es porque no se detecto una cara en la imagen

        if (xActualMarcador < RESOLUTION_W/2 - franjaCentral)  {  // Dejamos una franja de 60 (30+30) pixeles para que la camara considere que estamos centrados
            valorServoHorizontal+=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
            valorServoHorizontal = qMin(valorServoHorizontal, 160);
//            char c1 = (char)valorServoHorizontal;
//            qDebug() << "posicionServo=" << valorServoHorizontal;
//            const char *pc1 = &c1;
//            serial.write(pc1, 1);

        }
        else  if (xActualMarcador > RESOLUTION_W/2 + franjaCentral)  {
            valorServoHorizontal-=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
            valorServoHorizontal = qMax(valorServoHorizontal, 20);
//            char c1 = (char)valorServoHorizontal;
//            qDebug() << "posicionServo=" << valorServoHorizontal;

//            const char *pc1 = &c1;
//            serial.write(pc1, 1);

        }

        // Para el movimiento vertical controlamos el servo del Kinect
        if (yActualMarcador > 0 && yActualMarcador < RESOLUTION_H)  {  // Si cae fuera de esto es porque no se detecto una cara en la imagen

            if (yActualMarcador < RESOLUTION_H/2 - franjaCentral)  {  // Dejamos una franja de 60 (30+30) pixeles para que la camara considere que estamos centrados
                valorServoVertical+=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
                valorServoVertical = qMin(valorServoVertical, 30);
            }
            else  if (yActualMarcador > RESOLUTION_H/2 + franjaCentral)  {
                valorServoVertical-=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
                valorServoVertical = qMax(valorServoVertical, -30);
            }
        }


        if (valorServoHorizontal<0)
            valorServoHorizontal = 0;
        if (valorServoHorizontal>127)
            valorServoHorizontal = 127;

        if (valorServoVertical<-30)
            valorServoVertical = -30;
        if (valorServoVertical>30)
            valorServoVertical = 30;


        char c1 = (char)valorServoHorizontal;
        const char *pc1 = &c1;
        serial.write(pc1, 1);

        char c2 = (char)valorServoVertical;
//        const char *pc2 = &c2;

        char dosValoresJuntos[2];
        dosValoresJuntos[0] = c1;
        dosValoresJuntos[1] = c2;

        qDebug() << "Corresponde actualizar " << (int)dosValoresJuntos[0] << (int)dosValoresJuntos[1];
        qDebug() << "Corresponde actualizar servos" << valorServoHorizontal << valorServoVertical;

        const char* datos = dosValoresJuntos;

//        serial.write(datos, 2);


    }
}


void Escena::abrirPuerto(QString numeroCom)  {

    bool exito = true;


    serial.setPortName("com" + numeroCom);

    if (serial.open(QIODevice::ReadWrite))  {
        if (!serial.setBaudRate(QSerialPort::Baud9600))
            exito = false;
        if (!serial.setDataBits(QSerialPort::Data8))
            exito = false;
        if (!serial.setParity(QSerialPort::NoParity))
            exito = false;
        if (!serial.setStopBits(QSerialPort::OneStop))
            exito = false;
        if (!serial.setFlowControl(QSerialPort::NoFlowControl))
            exito = false;
    }
    else
        exito = false;


    if (!exito)
        qDebug() << "No se pudo abrir el puerto COM como lectura y escritura.";
    else
        qDebug() << "El com" << numeroCom << " se abrio con exito.";

    // Aqui lo ponemos en 90 grados
    int valorServoHorizontalInicial = 90;
    char c1 = (char)valorServoHorizontalInicial;
    const char *pc1 = &c1;
    serial.write(pc1, 1);

    int valorServoVerticalInicial = 0;
    char c2 = (char)valorServoVerticalInicial;
    const char *pc2 = &c2;

    char dosValoresJuntos[2];
    dosValoresJuntos[0] = c1;
    dosValoresJuntos[1] = c2;

    const char* datos = dosValoresJuntos;

//    serial.write(datos, 2);


//    serial.write(dosValoresJuntos, 2);

    valorServoHorizontal = valorServoHorizontalInicial;
    valorServoVertical = valorServoVerticalInicial;

}

void Escena::slot_fotocopiar()  {

    QImage frameActual = this->grabFrameBuffer(false);
    frameActual.save(Configuracion::carpetaDeTrabajo + "/fotocopia.png");

    QStringList argumentos;
    argumentos << Configuracion::carpetaDeTrabajo + "/fotocopiador.py"
               << Configuracion::carpetaDeTrabajo + "/fotocopia.png";

    qDebug() << "Se ejecuto?" << QProcess::execute("python", argumentos);

    QString nombreUsuario = "jose.feliciano.canta";
    QString clave = "58699932";
    QString server = "smtp.gmail.com";
    QString mailDestino = "cesarosimani@gmail.com";
    QString asunto = "Verano TEC 2014";
    QString mensaje = "Fotocopia!!";

    QFile archivoAdjunto(Configuracion::carpetaDeTrabajo + "/p_fotocopia.png");

    Smtp* smtp = new Smtp(nombreUsuario, clave, server);
    smtp->sendMail(nombreUsuario, mailDestino, asunto, mensaje, &archivoAdjunto);

}

// Este slot esta conectado con una signal que el navegador envia cuando la descarga del sitio finaliza
void Escena::slot_navegadorLoadFinished()  {
    QImage imagenNavegador = navegador.getImagenNavegador(180, 35, 590, 195);
    this->prepararImagenNavegador(imagenNavegador);
    this->widgetTexturizado = true;
}
