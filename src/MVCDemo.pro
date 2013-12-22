#-------------------------------------------------
#
# Project created by QtCreator 2013-12-21T00:12:00
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MVCDemo
TEMPLATE = app


SOURCES += glshaders.cpp \
           main.cpp \
           mainwindow.cpp \
           MVCCloner.cpp \
           mvcshaders.cpp \
           utils.cpp \
           glwidget.cpp \
           mvcdemo.cpp \
           sourcewidget.cpp

HEADERS  += glshaders.h \
            mainwindow.h \
            CloningParameters.h \
            glwidget.h \
            MVCCloner.h  \
            mvcshaders.h \
            utils.h \
            CoordinatesMesh.h \
            mvcdemo.h \
            sourcewidget.h

FORMS    +=

RESOURCES +=

# using pkg-config
linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += glee glu
}

# CGAL
*-g++* {
    QMAKE_CXXFLAGS += -frounding-math
}
LIBS += -lCGAL -lgmp
