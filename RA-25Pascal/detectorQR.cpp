//#include "detectorQR.h"
//#include <QDebug>

//DetectorQR::DetectorQR()  {

//    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

//}

//QVector<CodigoQR> DetectorQR::detectarCodigos(Mat & frame, bool dibujarRecuadro)  {
//    Mat grey;
//    cvtColor(frame, grey, CV_RGB2GRAY);

//    int width = frame.cols;
//    int height = frame.rows;
//    uchar *raw = (uchar *)grey.data;
//    // wrap image data
//    zbar::Image image(width, height, "Y800", raw, width * height);
//    // scan the image for barcodes
//    int n = scanner.scan(image);

//    QVector<CodigoQR> vCodigos;

//    // extract results
//    for(Image::SymbolIterator symbol = image.symbol_begin() ; symbol != image.symbol_end(); ++symbol)  {

//        CodigoQR codigo;

//        // do something useful with results
//        QString data = "Tipo: " + QString(symbol->get_type_name().c_str())
//                                + " - Data: " + QString(symbol->get_data().c_str());

//        codigo.data = data;

//        QVector<Point> vPoints;

//        int n = symbol->get_location_size();
//        for(int i=0;i<n;i++)  {
//            codigo.point.x = symbol->get_location_x(i);
//            codigo.point.y = symbol->get_location_y(i);
//            vPoints.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
//            vCodigos.push_back(codigo);
//        }


//        if (dibujarRecuadro)  {
//            RotatedRect r = minAreaRect(vPoints.toStdVector());
//            Point2f pts[4];
//            r.points(pts);
//            for(int i=0;i<4;i++)
//                line(frame, pts[i], pts[(i+1)%4], Scalar(255,0,0),3);
//        }
//    }
//    return vCodigos;
//}
