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


/* Usage:
 *
 *     [... some opengl calls]
 *     check_gl_error();
 */
void _check_gl_error(const char *file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif

