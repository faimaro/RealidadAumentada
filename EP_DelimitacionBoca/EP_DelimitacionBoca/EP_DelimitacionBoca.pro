QT += opengl widgets network

TEMPLATE = app
TARGET = EP_DelimitacionBoca

INCLUDEPATH += "/usr/include/GL/"

LIBS += "/usr/lib/x86_64-linux-gnu/libglut.so"

LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_core.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_highgui.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so"
LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_objdetect.so"

#LIBS += "/usr/lib/i386-linux-gnu/libglut.so"
#LIBS += "/usr/lib/i386-linux-gnu/libopencv_core.so"
#LIBS += "/usr/lib/i386-linux-gnu/libopencv_highgui.so"
#LIBS += "/usr/lib/i386-linux-gnu/libopencv_imgproc.so"
#LIBS += "/usr/lib/i386-linux-gnu/libopencv_objdetect.so"

HEADERS += \
    escena.h \
    textura.h \
    ventana.h \
    block.h \
    imagen.h

SOURCES += \
    escena.cpp \
    main.cpp \
    textura.cpp \
    ventana.cpp \
    block.cpp \
    imagen.cpp
