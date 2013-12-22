/****************************************************************************
 ** Image Cloning tool, based on Mean-Value Coordinates.
 **
 ** For more details see "Coordinates for Instant Image Cloning" by
 ** Farbman, Hoffer, Cohen-Or, Lipman and Lischinski 2009
 ****************************************************************************/

#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDialog>

#include "glwidget.h"

/* SourceWidget displays an image and allows the user to scribble a selection on
   top of the image.
 */
class SourceWidget : public QDialog
{
    Q_OBJECT

public:
    SourceWidget(QWidget * parent, GLWidget * gl);
    void setImage(const QImage & img);
    void setPenColor(const QColor & newColor);
    void setPenWidth(int newWidth);
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent * event);
    void resizeEvent(QResizeEvent * event);

private:
    void resizeImage(QImage * image, const QSize & newSize);

    // Boundary tracing aux method
    QPoint findStartingFromDir(QPolygonF & selectionPoly, int x, int y, int & dir);

    // Once we get the selection, we need to update the glWidget
    GLWidget * glWidget;

    // Does the drawing
    QPainterPath path;

    QPoint lastPoint;
    int myPenWidth;
    QColor myPenColor;
    QImage image;

};

#endif
