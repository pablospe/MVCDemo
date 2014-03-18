
#include "utils.h"
#include <iostream>
#include <string>
#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QImage>
#include <QtGui>
#include <QFile>


void copyImageToSubImage(QImage & src, QImage & trg, int startx, int starty) {
	
	for ( int x = 0; x<src.width(); x++) {
		for ( int y = 0; y<src.height(); y++) {
			
			trg.setPixel(x+startx,y+starty, src.pixel(x,y));
		}
	}
}

QImage *  createMipMap(const QImage & img) {
	
	QImage * mipmap = new QImage(img.width(), img.height() * 2, img.format());
	//mipmap->fill(31);
	QImage scaledImg = img.mirrored().copy();
	int starty = 0;
	for (int i=0; i<=5/*numLevels*/; i++) {
		
		copyImageToSubImage(scaledImg, *mipmap, 0, starty);
		starty += scaledImg.height();
		
		int w = ceil(scaledImg.width()/2.0); 
		int h = ceil(scaledImg.height()/2.0);

		scaledImg = img.mirrored().scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		
	}
	
	return mipmap;
}

QString readFileContent(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly|QFile::Text)){
        qDebug() << "Could not open file: " << filename;
        return QString();
    }

    QString data(file.readAll());
    file.close();

    return data;
}


void _check_gl_error(const char *file, int line)
{
    GLenum err = glGetError();

    while (err != GL_NO_ERROR)
    {
        std::string error;

        switch (err)
        {
        // The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.
        case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION";
            break;

        // An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.
        case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM";
            break;

        // A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.
        case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE";
            break;

        // There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.
        case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY";
            break;

        // The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE). The offending command is ignored and has no other side effect than to set the error flag.
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        std::cout << "glGetError::" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
    }
}
