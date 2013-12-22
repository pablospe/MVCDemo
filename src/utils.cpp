
#include "utils.h"
#include <iostream>
#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QImage>
#include <QtGui>


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
