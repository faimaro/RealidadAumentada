QT += opengl widgets network

TEMPLATE = app
TARGET = RealidadAumentadaEsqueleto

INCLUDEPATH += "/usr/include/GL/"
LIBS += "/usr/lib/x86_64-linux-gnu/libglut.so"

LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_core.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_highgui.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_objdetect.so"

SOURCES += \
    main.cpp \
    ventana.cpp \
    escena.cpp \
    clienteurl.cpp \
    textura.cpp \
    configuracion.cpp

HEADERS += \
    ventana.h \
    escena.h \
    clienteurl.h \
    textura.h \
    configuracion.h
