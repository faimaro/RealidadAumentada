#include "faceDetector.h"
#include "configuracion.h"



FaceDetector::FaceDetector()  {
    face_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/haarcascade_frontalface_alt_osi.xml";

    mouth_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/haarcascade_mcs_mouth.xml";
//    mouth_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/Mouth.xml";

    nose_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/haarcascade_mcs_nose.xml";

    ojoDerecho_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/haarcascade_mcs_lefteye.xml";
    ojoIzquierdo_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/haarcascade_mcs_righteye.xml";
//    ojoDerecho_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/ojoD.xml";
//    ojoIzquierdo_cascade_name = Configuracion::carpetaDeTrabajo.toStdString() + "/ojoI.xml";

    if( !face_cascade.load( face_cascade_name ) )  {
        qDebug() << "--(!)Error loading face cascade\n";
    }

    if( !mouth_cascade.load( mouth_cascade_name ) )  {
        qDebug() << "--(!)Error loading mouth cascade\n";
    }

    if( !nose_cascade.load( nose_cascade_name ) )  {
        qDebug() << "--(!)Error loading nose cascade\n";
    }

    if( !ojoDerecho_cascade.load( ojoDerecho_cascade_name ) )  {
        qDebug() << "--(!)Error loading Ojo Derecho cascade\n";
    }

    if( !ojoIzquierdo_cascade.load( ojoIzquierdo_cascade_name ) )  {
        qDebug() << "--(!)Error loading Ojo Izquierda cascade\n";
    }

}


Cara FaceDetector::detectBiggestFace(Mat frame){
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n");};
    std::vector<Rect> faces;
    std::vector<int> areas;
    Mat frame_gray;
    faces.clear();
    areas.clear();

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    int indMax=0;

    for ( size_t i = 0; i < faces.size(); i++ )
    {
        //find biggest face;
        int area;
        area = faces.at(i).width * faces.at(i).height;
        areas.push_back(area);
        //Mat faceROI = frame_gray( faces[i] );
    }
    for(size_t i = 0; (i < areas.size() && faces.empty() == false); i++){
        if(areas.at(i) > areas.at(indMax))
            indMax = i;
    }
    //qDebug() << "indMax = " << indMax;
    Cara face;
    Point center;
    int diametro = 0;
    if(faces.empty() == false)  {
        center.x = faces.at(indMax).x + faces.at(indMax).width/2;
        center.y = faces.at(indMax).y + faces.at(indMax).height/2;
        diametro =  faces.at(indMax).width;
        face.width = faces.at(indMax).width/2;
        face.height = faces.at(indMax).height/2;
        face.center = center;
        face.diametro = diametro;
    }

    return face;
}

void FaceDetector::drawFace(Cara face, Mat &frame, int width, int heigth){
    ellipse( frame, face.center, Size( width, heigth ), 0, 0, 360, Scalar( 0, 0, 255 ), 4, 8, 0 );
    imshow("cara", frame);
}

vector<Rect> FaceDetector::detectFacesRect(const Mat frame){
//    if( !face_cascade.load( face_cascade_name ) )  {
//        printf("--(!)Error loading face cascade\n");
//    }

    vector<Rect> faces;

    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
//    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, CASCADE_FIND_BIGGEST_OBJECT, Size(50, 50) );

//    CASCADE_DO_CANNY_PRUNING CASCADE_SCALE_IMAGE CASCADE_FIND_BIGGEST_OBJECT CASCADE_DO_ROUGH_SEARCH

    return faces;
}

vector<Rect> FaceDetector::detectMouthsRect(const Mat frame)  {
    vector<Rect> mouths;

    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    mouth_cascade.detectMultiScale( frame_gray, mouths, 1.1, 2, 0|CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );

    return mouths;
}

vector<Rect> FaceDetector::detectOjoIzquierdoRect(const Mat frame)  {
    vector<Rect> ojoDerecho;

    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    ojoDerecho_cascade.detectMultiScale( frame_gray, ojoDerecho, 1.1, 2, CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );

    return ojoDerecho;
}

vector<Rect> FaceDetector::detectOjoDerechoRect(const Mat frame)  {
    vector<Rect> ojoIzquierdo;

    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    ojoIzquierdo_cascade.detectMultiScale( frame_gray, ojoIzquierdo, 1.1, 2, CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );

    return ojoIzquierdo;
}


vector<Rect> FaceDetector::detectNosesRect(const Mat frame)  {
    vector<Rect> noses;

    Mat frame_gray;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    nose_cascade.detectMultiScale( frame_gray, noses, 1.1, 2, 0|CASCADE_FIND_BIGGEST_OBJECT, Size(30, 30) );

    return noses;
}

vector<Cara> FaceDetector::detectFaces(Mat frame){
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n");};
    vector<Rect> faces;
    vector<int> areas;
    vector<Cara> facesDetected;
    Mat frame_gray;
    faces.clear();
    areas.clear();

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

//    int indMax=0;

//    for ( size_t i = 0; i < faces.size(); i++ )
//    {
//        //find biggest face;
//        int area;
//        area = faces.at(i).width * faces.at(i).height;
//        areas.push_back(area);
//        //Mat faceROI = frame_gray( faces[i] );
//    }
//    for(size_t i = 0; (i < areas.size() && faces.empty() == false); i++){
//        if(areas.at(i) > areas.at(indMax))
//            indMax = i;
//    }
    //qDebug() << "indMax = " << indMax;
    for ( size_t i = 0; i < faces.size(); i++ ) {
        Cara face;
        Point center;
        int diametro = 0;
        if(faces.empty() == false){
        center.x = faces.at(i).x + faces.at(i).width/2;
        center.y = faces.at(i).y + faces.at(i).height/2;
        diametro =  faces.at(i).width;
        face.width = faces.at(i).width/2;
        face.height = faces.at(i).height/2;
        face.center = center;
        face.diametro = diametro;
        facesDetected.push_back(face);
        }
    }

    return facesDetected;
}

void FaceDetector::drawMultipleFaces(vector<Cara> faces, Mat &frame){
    for ( size_t i = 0; i < faces.size(); i++ ) {
        ellipse( frame, faces.at(i).center, Size( faces.at(i).width, faces.at(i).height ), 0, 0, 360, Scalar( 0, 0, 255 ), 4, 8, 0 );
    }
}

void FaceDetector::drawMultipleRect(vector<Rect> faces, Mat &frame)  {
    for ( size_t i = 0; i < faces.size(); i++ )  {
//        Point centro(faces.at(i).tl().x + faces.at(i).width/2, faces.at(i).tl().y + faces.at(i).height/2);
//        ellipse( frame, centro, Size( faces.at(i).width, faces.at(i).height ), 0, 0, 360, Scalar( 0, 0, 255 ), 4, 8, 0 );

        rectangle(frame, faces.at(i), Scalar( 0, 0, 255 ), 2, 8, 0);

    }
}


void FaceDetector::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator)  {
    std::ifstream file(filename.c_str(), ifstream::in);

    if (!file)  {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }

    string line, path, classlabel;
    while (getline(file, line))  {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            Mat aux;
            aux = imread(Configuracion::carpetaDeTrabajo.toStdString() + path, 0);

            images.push_back(aux);
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}


