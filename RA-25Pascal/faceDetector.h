#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "cara.h"

#include <QDebug>
#include <fstream>

using namespace cv;
using namespace std;

class FaceDetector  {
private:
    String face_cascade_name;
    String mouth_cascade_name;
    String nose_cascade_name;
    String ojoDerecho_cascade_name;
    String ojoIzquierdo_cascade_name;
    CascadeClassifier face_cascade;
    CascadeClassifier mouth_cascade;
    CascadeClassifier nose_cascade;
    CascadeClassifier ojoDerecho_cascade;
    CascadeClassifier ojoIzquierdo_cascade;
    Ptr<FaceRecognizer> model;

public:
    FaceDetector();
    Cara detectBiggestFace(Mat frame);
    void drawFace(Cara face, Mat &frame, int width, int heigth);
    vector<Cara> detectFaces(Mat frame);

    vector<Rect> detectFacesRect(const Mat frame);
    vector<Rect> detectMouthsRect(const Mat frame);
    vector<Rect> detectNosesRect(const Mat frame);
    vector<Rect> detectOjoIzquierdoRect(const Mat frame);
    vector<Rect> detectOjoDerechoRect(const Mat frame);

    void drawMultipleFaces(vector<Cara> faces, Mat &frame);
    void drawMultipleRect(vector<Rect> faces, Mat &frame);

    void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');

    Ptr<FaceRecognizer> getModel()  {
        return model;
    }
};

#endif // FACEDETECTOR_H
