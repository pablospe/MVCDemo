QT += core gui opengl

TARGET = MVCDemo
TEMPLATE = app

SRC_DIR = $${PWD}/src

SOURCES += $${SRC_DIR}/glwidget.cpp \
           $${SRC_DIR}/main.cpp \
           $${SRC_DIR}/mainwindow.cpp \
           $${SRC_DIR}/mvcshaders.cpp \
           $${SRC_DIR}/MVCCloner.cpp \
           $${SRC_DIR}/sourcewidget.cpp \
           $${SRC_DIR}/utils.cpp

HEADERS  += $${SRC_DIR}/CloningParameters.h \
            $${SRC_DIR}/CoordinatesMesh.h \
            $${SRC_DIR}/glwidget.h \
            $${SRC_DIR}/mainwindow.h \
            $${SRC_DIR}/mvcshaders.h \
            $${SRC_DIR}/MVCCloner.h  \
            $${SRC_DIR}/sourcewidget.h \
            $${SRC_DIR}/utils.h

# using pkg-config
linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += glu
}

# CGAL
*-g++* {
    QMAKE_CXXFLAGS += -frounding-math
}
LIBS += -lCGAL -lgmp

RESOURCES += images/images.qrc
