#include "escena.h"
#include <QDebug>
Escena::Escena() : linesWidth( 1 )
{
    videoCapture = new cv::VideoCapture(0);

    clasificadorCara.load( "../Clasificadores/ClasificadorCaraFrontal.xml" );
    clasificadorOjoIzquierdo.load( "../Clasificadores/ClasificadorOjoIzquierdo.xml" );
    clasificadorOjoDerecho.load( "../Clasificadores/ClasificadorOjoDerecho.xml" );

    timerEscena = new QTimer;
    timerEscena->start(100);
    connect( timerEscena, SIGNAL( timeout() ), SLOT( slot_actualizarEscena() ) );
    cant=0;
    // Size de la imagen de la Camara
    // this->setFixedSize( 640, 480 );
}

void Escena::initializeGL()
{   
    glClearColor( 0, 0, 0, 0 );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );

    texturas[0].nombreTextura = "TexturaCamara";
    glGenTextures( 1, &texturas[0].idTextura );
}

void Escena::resizeGL( int w, int h )
{
    glViewport( 0, 0, w, h );
}

void Escena::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, RESOLUCION_ANCHO, 0, RESOLUCION_ALTO, 1, 1000 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1, 1, 1 );

    // Dibujamos imagen de la Camara

    glBindTexture( GL_TEXTURE_2D, texturas[0].idTextura );

    glBegin( GL_QUADS );

        glTexCoord2f( 0, 0 );     glVertex3f( 0, RESOLUCION_ALTO, -999 );
        glTexCoord2f( 1, 0 );     glVertex3f( RESOLUCION_ANCHO, RESOLUCION_ALTO, -999 );
        glTexCoord2f( 1, 1 );     glVertex3f( RESOLUCION_ANCHO, 0, -999 );
        glTexCoord2f( 0, 1 );     glVertex3f( 0, 0, -999 );

    glEnd();

    glDisable( GL_TEXTURE_2D );

    glFlush();
}

void Escena::slot_actualizarEscena()
{
    // Por cada timeout de timerEscena, genera una textura de camara nueva

    videoCapture->operator >>( texturas[0].matTextura );

    procesar( texturas[0].matTextura );

    texturas[0].generarDesdeMat();

    this->repaint();
}

void Escena::slot_aumentarAnchoLineas()
{
    linesWidth++;
}

void Escena::slot_disminuirAnchoLineas()
{
    if( linesWidth > 1 ) linesWidth--;
}

void Escena::slot_detenerProcesamiento()
{
    if( timerEscena->isActive() )
    {
        timerEscena->stop();
    }
    else
    {
        timerEscena->start();
    }
}

void Escena::procesar( Mat &frame )
{
     Mat matImagenActual = frame.clone();

     // PROCESAMIENTO

     // Declaramos un vector para guardar las caras detectadas
     std::vector<Rect> vectorCaras;
     vectorCaras.clear();

     // Detectamos las caras en toda la extStream.reimagen completa
     clasificadorCara.detectMultiScale( matImagenActual, vectorCaras, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size( 100,100 ) );

     for ( unsigned int i = 0; i < vectorCaras.size(); i++ )
     {
         // Dibujamos el rectangulo de la cara
         //rectangle( matImagenActual, vectorCaras.at(i), Scalar( 0, 255, 0 ), linesWidth );

         // Declaramos y dibujamos un contenedor para el ojo izquierdo

         Rect rectParaOjosIzquierdos( vectorCaras.at(i).x + vectorCaras.at(i).width/2, vectorCaras.at(i).y + vectorCaras.at(i).height*MARGEN_VERTICAL_CONTENEDOR_OJOS/100, vectorCaras.at(i).width*ANCHO_CONTENEDOR_OJO/100, vectorCaras.at(i).height*ALTO_CONTENEDOR_OJO/100);
         // rectangle( matImagenActual, rectParaOjosIzquierdos, Scalar( 0, 0, 200 ), linesWidth );

         // Declaramos y dibujamos un contenedor para el ojo derecho
         Rect rectParaOjosDerechos( vectorCaras.at(i).x + vectorCaras.at(i).width*MARGEN_LATERAL_CONTENEDOR_OJOS/100, vectorCaras.at(i).y + vectorCaras.at(i).height*MARGEN_VERTICAL_CONTENEDOR_OJOS/100, vectorCaras.at(i).width*ANCHO_CONTENEDOR_OJO/100, vectorCaras.at(i).height*ALTO_CONTENEDOR_OJO/100);
         // rectangle( matImagenActual, rectParaOjosDerechos, Scalar( 0, 0, 200 ), linesWidth );

         // Declaramos un vector para guardar los ojos izquierdos detectados
         std::vector<Rect> vectorOjosIzquierdos;
         vectorOjosIzquierdos.clear();

         // Detectamos ojos izquierdos en el mat designado por el rectOjosIzquierdos
         Mat matParaOjosIzquierdos( matImagenActual, rectParaOjosIzquierdos );
         clasificadorOjoIzquierdo.detectMultiScale( matParaOjosIzquierdos, vectorOjosIzquierdos, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size( 30, 30 ) );

         Rect ojoIzquierdoLocalAlContenedor;

         if ( vectorOjosIzquierdos.size() > 0 )
         {
             // Descartamos los ojos izquierdos que no sean el mas grande
             ojoIzquierdoLocalAlContenedor = rectanguloMasGrande( vectorOjosIzquierdos );
             Rect rectanguloOjoIzquierdoDesplazado( rectParaOjosIzquierdos.x + ojoIzquierdoLocalAlContenedor.x, rectParaOjosIzquierdos.y + ojoIzquierdoLocalAlContenedor.y, ojoIzquierdoLocalAlContenedor.width, ojoIzquierdoLocalAlContenedor.height );
             rectangle( matImagenActual, rectanguloOjoIzquierdoDesplazado, Scalar( 255, 255, 255 ), linesWidth );
         }

         // Declaramos un vector para guardar los ojos derechos detectados
         std::vector<Rect> vectorOjosDerechos;
         vectorOjosDerechos.clear();

         // Detectamos ojos derechos en el mat designado por el rectOjosDerechos
         Mat matParaOjosDerechos( matImagenActual, rectParaOjosDerechos );
         clasificadorOjoDerecho.detectMultiScale( matParaOjosDerechos, vectorOjosDerechos, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size( 30, 30 ) );

         Rect ojoDerechoLocalAlContenedor;

         if ( vectorOjosDerechos.size() > 0 )
         {
             // Descartamos los ojos derechos que no sean el mas grande
             ojoDerechoLocalAlContenedor = rectanguloMasGrande( vectorOjosDerechos );
             Rect rectanguloOjoDerechoDesplazado( rectParaOjosDerechos.x + ojoDerechoLocalAlContenedor.x, rectParaOjosDerechos.y + ojoDerechoLocalAlContenedor.y, ojoDerechoLocalAlContenedor.width, ojoDerechoLocalAlContenedor.height );
             rectangle( matImagenActual, rectanguloOjoDerechoDesplazado, Scalar( 255, 255, 255 ), linesWidth );
         }

        //Creo la matriz en la que voy a meter los posibles circulos
        //metodo para deteccion de parpadeo
         int threshold_value = 0;
         int threshold_type = 3;;
         int const max_value = 255;
         int const max_type = 4;
         int const max_BINARY_value = 255;
         //Mat src, src_gray, dst;

         std::vector<Vec3f> storageCir;


         Mat img;
         int dp = 2;
         float minDist = 300.0 ;
         int param1 = 32 ;
         int param2 = 60;
         int minRadius = 10;
         int maxRadius = 22;
         //GaussianBlur( img, img, Size(7,7), 2, 2 );
         cvtColor( matParaOjosDerechos, img, CV_BGR2GRAY );
         threshold( img, img, threshold_value, max_BINARY_value,threshold_type );

         HoughCircles(img, storageCir, CV_HOUGH_GRADIENT  , dp, minDist,
                                             param1, param2, minRadius, maxRadius);


        if (storageCir.empty())
        {

            //qDebug()<<"ojo cerrado";
            //storageCir.clear();
            cant++;
         }

        else{
            cant=0;
            //qDebug()<<"ojo abierto";
            //storageCir.clear();
        }
        storageCir.clear();


        if (cant==3){
        qDebug()<<"parpadeo";
        cant=0;
        }
         if ( vectorOjosIzquierdos.size() > 0 && vectorOjosDerechos.size() > 0 )
         {
             // A estos puntos les llamo centros globales, pero son los centros desplazados (1/4 y 3/4)
             Point centroOjoIzquierdoGlobal( rectParaOjosIzquierdos.x + ojoIzquierdoLocalAlContenedor.x + ojoIzquierdoLocalAlContenedor.width*3/4, rectParaOjosIzquierdos.y + ojoIzquierdoLocalAlContenedor.y + ojoIzquierdoLocalAlContenedor.height/2 );
             Point centroOjoDerechoGlobal( rectParaOjosDerechos.x + ojoDerechoLocalAlContenedor.x + ojoDerechoLocalAlContenedor.width*1/4, rectParaOjosDerechos.y + ojoDerechoLocalAlContenedor.y + ojoDerechoLocalAlContenedor.height/2 );
            // qDebug()<<cv::Point ('centroOjoDerechoGlobal');
             //QDebug(centroOjoDerechoGlobal);
             //QDebug(centroOjoIzquierdoGlobal);
             line( matImagenActual, centroOjoIzquierdoGlobal, centroOjoDerechoGlobal, Scalar( 100, 50, 255 ), linesWidth );

             float anguloEntreOjos = anguloEntre ( centroOjoDerechoGlobal, centroOjoIzquierdoGlobal );
             float largoLineaOjoBoca = vectorCaras.at(i).height/9*6;

             float baseTrianguloOjoBoca = sin( anguloEntreOjos ) * largoLineaOjoBoca;
             if ( centroOjoDerechoGlobal.y < centroOjoIzquierdoGlobal.y ) baseTrianguloOjoBoca = -baseTrianguloOjoBoca;
             float alturaTrianguloOjoBoca = cos( anguloEntreOjos ) * largoLineaOjoBoca;

             Point finLineaOjoIzquierdo( centroOjoIzquierdoGlobal.x + baseTrianguloOjoBoca, centroOjoIzquierdoGlobal.y + alturaTrianguloOjoBoca );
             Point finLineaOjoDerecho( centroOjoDerechoGlobal.x + baseTrianguloOjoBoca, centroOjoDerechoGlobal.y + alturaTrianguloOjoBoca );

             line( matImagenActual, centroOjoIzquierdoGlobal, finLineaOjoIzquierdo, Scalar( 150, 20, 255 ), linesWidth );
             line( matImagenActual, centroOjoDerechoGlobal, finLineaOjoDerecho, Scalar( 150, 20, 255 ), linesWidth );

             // Saco dos promedios consecutivos
             Point bocaSupIzq( (finLineaOjoIzquierdo.x + centroOjoIzquierdoGlobal.x)/2, ( finLineaOjoIzquierdo.y + centroOjoIzquierdoGlobal.y)/2 );
             Point bocaSupDer( (finLineaOjoDerecho.x + centroOjoDerechoGlobal.x)/2, ( finLineaOjoDerecho.y + centroOjoDerechoGlobal.y)/2 );

             line( matImagenActual, bocaSupIzq, bocaSupDer, Scalar( 150, 20, 255 ), linesWidth );
             line( matImagenActual, finLineaOjoIzquierdo, finLineaOjoDerecho, Scalar( 150, 20, 255 ), linesWidth );

             //Point bocaInfIzq( (finLineaOjoIzquierdo.x + centroOjoIzquierdoGlobal.x)/2, (finLineaOjoIzquierdo.y + centroOjoIzquierdoGlobal.y)/2 );
             //Point bocaInfDer( (finLineaOjoIzquierdo.x + centroOjoIzquierdoGlobal.x)/2, (finLineaOjoIzquierdo.y + centroOjoIzquierdoGlobal.y)/2 );
         }

         // Estiramos un poco la cara y la dibujamos
         vectorCaras.at(i).height *= 1.2;
         rectangle( matImagenActual, vectorCaras.at(i), Scalar( 0, 0, 0 ), linesWidth );
     }

     // FIN PROCESAMIENTO

     frame = matImagenActual.clone();
}

Rect Escena::rectanguloMasGrande( std::vector<Rect> rectangulos )
{
    Rect rectMaximo( 0, 0, 0, 0 );
    int areaMaxima = rectMaximo.x * rectMaximo.y;
    for ( unsigned int i = 0; i < rectangulos.size(); i++)
    {
        int areaActual = rectangulos.at(i).x * rectangulos.at(i).y;
        if (areaActual > areaMaxima)
        {
            rectMaximo.x = rectangulos.at(i).x;
            rectMaximo.y = rectangulos.at(i).y;
            rectMaximo.width = rectangulos.at(i).width;
            rectMaximo.height = rectangulos.at(i).height;
        }
    }
    return rectMaximo;
}

float Escena::anguloEntre( Point a, Point b )
{
    float angRes = 0;

    float opuesto;
    if ( a.y > b.y ) opuesto = a.y - b.y ;
    else opuesto = b.y - a.y;

    float adyacente = b.x - a.x;

    if ( adyacente != 0 ) angRes = atan( opuesto / adyacente );

    return angRes; // En radianes
}
