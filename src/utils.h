/****************************************************************************
 ** Image Cloning tool, based on Mean-Value Coordinates.
 **
 ** For more details see "Coordinates for Instant Image Cloning" by
 ** Farbman, Hoffer, Cohen-Or, Lipman and Lischinski 2009
 ****************************************************************************/


#ifndef UTILS_H
#define UTILS_H

class QImage;
class QString;

QImage * createMipMap(const QImage &);
QString readFileContent(const QString &filename);


#endif

