/****************************************************************************
 ** Image Cloning tool, based on Mean-Value Coordinates.
 **
 ** For more details see "Coordinates for Instant Image Cloning" by
 ** Farbman, Hoffer, Cohen-Or, Lipman and Lischinski 2009
 ****************************************************************************/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <iostream>
#include "MVCCloner.h"

typedef struct {
    float x, y;
} Vertex2D;

typedef struct {
    float x, y ;
} TexCoords;

typedef struct {
    int p1, p2, p3;
} Triangles;

// Data structures that hold the information on the selection
typedef struct  {
    int numPoints;
    int numTriangles;
    int boundarySize;

    Vertex2D *  vertices;   // 2D coordinates of the mesh
    Triangles * triangles;  // Indices into vertices

    float * boundaryCoordsTex;
    float * weightsTex;

	// Trnsformation vars
    float tx, ty; // Accum. translation coords
    float dx, dy; // Current translation coords
	
	Vertex2D toOrigin;
	float rangle;
	float flip;
	float scale;
	
} Selection;

/* GLWidget provides an OpenGL context for blitting of the target image and shows
   the result of seamless cloning of the source patch into the target image.
   It also holds the data structures needed to compute the MVC with GLSL
 */
class GLWidget : public QGLWidget
{
    Q_OBJECT


public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    /* Gets boundary and the selection polygon(needed for triangle inside/outside test).
       Converts CGAL data-structures to data usable for shaders.

       TODO: very inefficient at the moment.
     */
    void updateSelection(std::vector<Point> & boundaryVector, QPolygonF & selectionPoly, bool reeset = true);

    void setTargetImage(const QImage & image);
    void setSourcePatch(const QImage & image);

    int getTargetWidth() { return target_w; }
    int getTargetHeight() { return target_h; }
    int getSourceWidth() { return source_w; }
    int getSourceHeight() { return source_h; }

    void paintSelection();
    void paintMesh();

    void showMVC() { mode = MVC; update(); }
	void showComp() { mode = COMP; update(); }
	void showMembrane() { mode = MEMBRANE; update(); }
	void showMesh() { mode = TRI; update(); }
	
	void setMethod(int method);
	
	void rotateCW() { selection.rangle +=3; update(); }
	void rotateCCW() { selection.rangle -=3; update(); }	
	void scaleUp() { selection.scale *= 1.05; update(); }
	void scaleDown() { selection.scale /= 1.05; update(); }
	void flip() { selection.flip = 1.0-selection.flip; update(); }	
	void resetSelection() {
        selection.tx = target_w/2.1;
        selection.ty = target_h/2.1;
		selection.dx = 0;
		selection.dy = 0;		
		selection.rangle = 0.0;
		selection.flip = 0.0;
		selection.scale = 1.0;
		update();
	}
	
	void notInit() { init = false; }
	
    void compileAttachLinkShaderFromSource(const QString& vs, const QString& fs);

	void errCheck(QString mymsg = QString());
	void bindSource();
	void bindTarget();

protected:
    void initializeGL();
	void loadShaders();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

private:
    enum {MVC,COMP,MEMBRANE,TRI} mode;
	enum {HIER1,HIER2,ADAP1,ADAP2} method;
	
	bool antsOn;
	float antColor;
	
    Selection selection;

    // Pointer to CGAL data. Used in conversion process
    CoordinatesMesh* cmesh;
    DelaunayTriangulation* adaptiveMesh;

    bool init;
	
    int target_w, target_h;
    int source_w, source_h;
    int selection_w, selection_h;

    GLenum target; // as in texture target (not source/target)
    GLuint tex0_id, tex1_id, tex2_id, tex3_id;

    QOpenGLShaderProgram *m_program;

    int click_x, click_y; // Coordinates of the mouse click which started the current event
	
	std::vector<Point> boundaryVector;
	QPolygonF selectionPoly;
	
	QImage * sourceImage;	
	QImage * targetImage;

};
#endif
