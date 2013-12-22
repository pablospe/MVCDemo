#include "glwidget.h"
#include "mvcshaders.h";
#include <iostream>
#include <QtGui>
#include <QtOpenGL>

#include <math.h>
#include "utils.h"

#include <QCoreApplication>
#include <GL/glu.h>

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent){
    
    target_w = 400;
    target_h = 400;
    
    init = false;
    mode = MVC;
    antsOn = true;
    antColor = 1.0;			
    
    selection.vertices = 0;
    
    selection.tx = 0;
    selection.ty = 0;
    selection.dx = 0;
    selection.dy = 0;
	
	selection.rangle = 0.0;
	selection.flip = 0.0;
	selection.scale = 1.0;	
	
	method = HIER1;
	
	sourceImage = NULL;
	targetImage = NULL;
	
}

GLWidget::~GLWidget(){
    
	// Instantiated only once. Let OS do the cleaning

    makeCurrent();
}

void GLWidget::initializeGL(){
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);
    
    target = GL_TEXTURE_RECTANGLE_ARB;
    glEnable(target);
    
    loadShaders();
    showMVC();
}

void GLWidget::loadShaders(){
	
    if (method==HIER1){
		compileAttachLinkShaderFromSource(getSamplingVS(), getSamplingFS());
		return;
    }
    if (method==HIER2){	
		compileAttachLinkShaderFromSource(getSampling2DFilterVS(), getSamplingFS());
		return;
    }	
    if (method==ADAP1 ||method==ADAP2){
		compileAttachLinkShaderFromSource(getAlphaVS(), getAlphaFS());
		return;
    }		
}

void GLWidget::compileAttachLinkShaderFromSource(const std::string& vs, const std::string& fs) {
	m_vertexShader = new GLVertexShader(vs.c_str(), vs.length());
	m_fragmentShader = new GLFragmentShader(fs.c_str(), fs.length());
	m_program = new GLProgram;
	m_program->attach(*m_vertexShader);
	m_program->attach(*m_fragmentShader);
	if (m_program->failed()) {
		qWarning("Failed to compile and link shader program");
		qWarning("Vertex shader log:");
		qWarning() << m_vertexShader->log();
		qWarning() << "Fragment shader log:";
		qWarning() << m_fragmentShader->log();
		qWarning("Shader program log:");
		qWarning() << m_program->log();

		delete m_vertexShader;
		delete m_fragmentShader;
		delete m_program;
	}
}

void GLWidget::compileAttachLinkShader(QString & vs_filename, QString & fs_filename){
    
	m_vertexShader = new GLVertexShader(vs_filename);
	m_fragmentShader = new GLFragmentShader(fs_filename);
	m_program = new GLProgram;
	m_program->attach(*m_vertexShader);
    m_program->attach(*m_fragmentShader);
	if (m_program->failed()) {
		qWarning("Failed to compile and link shader program");
		qWarning("Vertex shader log:");
		qWarning() << m_vertexShader->log();
		qWarning() << "Fragment shader log:";
		qWarning() << m_fragmentShader->log();
		qWarning("Shader program log:");
		qWarning() << m_program->log();
		
		delete m_vertexShader;
		delete m_fragmentShader;
		delete m_program;
	}
	
}

void GLWidget::paintGL(){
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Blit the target image
    glBegin(GL_QUADS);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,0);
    glVertex2f(0.0, 0.0);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,target_w,0);
    glVertex2f(target_w, 0);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,target_w,target_h);
    glVertex2f(target_w, target_h);
    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,target_h);
    glVertex2f(0, target_h);
    glEnd();
    
    // Transform the selection patch
	glTranslatef(selection.tx+selection.dx, selection.ty+selection.dy, 0);
	glTranslatef(-selection.toOrigin.x, -selection.toOrigin.y, 0);
	glRotatef(selection.rangle,0,0,1);
	glTranslatef(selection.toOrigin.x, selection.toOrigin.y, 0);
	
	glTranslatef(-selection.toOrigin.x, -selection.toOrigin.y, 0);
	if (selection.flip)
		glScalef(-1*selection.scale,1*selection.scale,1);
	else
		glScalef(selection.scale,1*selection.scale,1);
	
	glTranslatef(selection.toOrigin.x, selection.toOrigin.y, 0);
    
    // Show the selection (according to the viewing mode)
    if (mode==TRI)
        paintMesh();
	else
		paintSelection();
    
}

void GLWidget::paintSelection(){
    
    if (!init)
        return;
    
    // Enable the GLSL program
	m_program->bind();
	// and pass data inside
	m_program->setInt("tex0", 0);
	m_program->setInt("tex1", 1);
	m_program->setInt("tex2", 2);
	m_program->setInt("tex3", 3);
    m_program->setInt("mode", mode);
	
    if(method==HIER1 || method==HIER2){
		m_program->setFloat("target_h", target_h);
		m_program->setFloat("source_h", selection_h);
    }else if(method==ADAP1){
		m_program->setInt("blend", 0);
		m_program->setInt("boundarySize", selection.boundarySize);
	}else if(method==ADAP2){
		m_program->setInt("blend", 1);
		m_program->setInt("boundarySize", selection.boundarySize);
	}
    
    float fx=selection.tx+selection.dx;
    float fy=selection.ty+selection.dy;
    
	glNormal3f(fx, fy, 0);
	
    Vertex2D v1, v2, v3;
    int ind1, ind2, ind3;
    
    for (int i=0; i<selection.numTriangles; i++){
        
        ind1 = selection.triangles[i].p1;
        ind2 = selection.triangles[i].p2;
        ind3 = selection.triangles[i].p3;
        
        v1 = selection.vertices[ind1];
        v2 = selection.vertices[ind2];
        v3 = selection.vertices[ind3];
        
        glBegin(GL_TRIANGLES);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v1.x+fx, v1.y+fy);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, v1.x, v1.y);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0, ind1);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);
        glVertex2f(v1.x, v1.y);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v2.x+fx, v2.y+fy);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, v2.x, v2.y);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0, ind2);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);
        glVertex2f(v2.x, v2.y);
        
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB, v3.x+fx, v3.y+fy);
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, v3.x, v3.y);
        glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0, ind3);
        glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);
        glVertex2f(v3.x, v3.y);
        
        glEnd();
    }
    m_program->unbind(); //glUseProgram(0);
}

void GLWidget::paintMesh(){
    
    if (!init)
        return;
    
    int ind1, ind2, ind3;
    Vertex2D v1, v2, v3;
    
    for (int i=0; i<selection.numTriangles; i++){
        
        glBegin(GL_LINES);
        ind1 = selection.triangles[i].p1;
        ind2 = selection.triangles[i].p2;
        ind3 = selection.triangles[i].p3;
        
        v1 = selection.vertices[ind1];
        v2 = selection.vertices[ind2];
        v3 = selection.vertices[ind3];
        
        glVertex2f(v1.x, v1.y);
        glVertex2f(v2.x, v2.y);
        glVertex2f(v2.x, v2.y);
        glVertex2f(v3.x, v3.y);
        glVertex2f(v3.x, v3.y);
        glVertex2f(v1.x, v1.y);
        
        glEnd();
    }
}

void GLWidget::resizeGL(int width, int height){
    
    /* Identity mapping between image and GL coordinates */
    glViewport(0, 0, (GLsizei)width, (GLsizei)height) ;
    glMatrixMode(GL_PROJECTION) ;
    glLoadIdentity() ;
    gluOrtho2D(0, (GLdouble)width, 0, (GLdouble)height) ;
    
}

void GLWidget::mousePressEvent(QMouseEvent *event){
    
    if (event->buttons() & Qt::LeftButton) {
        click_x = event->pos().x();
        click_y = event->pos().y();
    }
    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event){
    
    if (event->button() == Qt::LeftButton) {
        
        click_x = 0;
        click_y = 0;
        
        selection.tx += selection.dx;
        selection.ty += selection.dy;
        
        selection.dx = 0;
        selection.dy = 0;
    }
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event){
    
    if (event->buttons() & Qt::LeftButton) {
        selection.dx = event->pos().x()-click_x;
        selection.dy = click_y-event->pos().y();
    }
    update();
}

void GLWidget::bindTarget(){
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex0_id);
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(target, tex0_id);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // Don't filter
    glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
	if (method==HIER2) {
		// Create a mipmap - used in HIER2 method
		QImage * res = createMipMap(*targetImage);
		glTexImage2D(target, 0, GL_RGBA, target_w, res->height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, res->bits());
		delete res;
	}
	else
		glTexImage2D(target, 0, GL_RGBA, target_w, target_h, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, targetImage->mirrored().bits());	
}

void GLWidget::bindSource(){
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex1_id);
    
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(target, tex1_id);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // Don't filter
    glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    
	if (method==HIER2) {
		// Create a mipmap - used in HIER2 method
		QImage * res = createMipMap(*sourceImage);
		glTexImage2D(target, 0, GL_RGBA, sourceImage->width(), res->height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, res->bits());
		delete res;
	}
	else
		glTexImage2D(target, 0, GL_RGBA, sourceImage->width(), sourceImage->height(), 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, sourceImage->mirrored().bits());	
}

void GLWidget::setTargetImage(const QImage & image){ 
	
	if (targetImage)
		delete targetImage;
	
	targetImage = new QImage(image.copy());
    target_w = image.width();
    target_h = image.height();
	
	bindTarget();
}

void GLWidget::setSourcePatch(const QImage & image)
{
	if (sourceImage)
		delete sourceImage;	
    
	sourceImage = new QImage(image.copy());
    selection_w = image.width();
    selection_h = image.height();
    
	bindSource();
}

/* Gets boundary and the selection polygon(needed for triangle inside/outside test).
 Converts CGAL data-structures to data usable for shaders.
 
 TODO: very inefficient at the moment.
 Due to the fact that we proably miss some bits form the CGAL documentation
 the methods does a lot of probably unnessecary work and it's a major bottleneck
 right now.
 
 */
void GLWidget::updateSelection(std::vector<Point> & boundaryVector, QPolygonF & selectionPoly, bool reset)
{
    
	// Copy the data (we needed it in order to change the settings on the fly)
	this->selectionPoly = selectionPoly;
	this->boundaryVector = boundaryVector;
	
	//qDebug() << boundaryVector.size() << " " << selectionPoly.size();
	
	if (reset)
		resetSelection();
	
    MVCCloner cloner;
    CloningParameters params;	
    
    if (method==HIER1 || method==HIER2)
        params.setHierarchic(); 	
    
    // TODO: Ask Gil how about freeing the memory of those guys
    // TODO: CGAL still throws something this thing doesnt catch
    try {		
        cmesh = cloner.preprocess(boundaryVector,params);
        adaptiveMesh = cmesh->mesh;
		cloner.tagFaces(adaptiveMesh);
    }
    catch (...)
    {
        qDebug() << "CGAL is not happy with the selection";
    }
    
    // Clean/allocate selection
    if (selection.vertices){ //sufficient indication for the rest
        delete[] selection.vertices;
        delete[] selection.triangles;
        delete[] selection.boundaryCoordsTex;
        delete[] selection.weightsTex;
    }
    selection.boundarySize = boundaryVector.size();
    selection.numPoints = adaptiveMesh->number_of_vertices();
    selection.numTriangles = adaptiveMesh->number_of_faces();
    
    selection.vertices = new Vertex2D[selection.numPoints];        
    selection.boundaryCoordsTex = new float[selection.boundarySize * 2];
    selection.weightsTex = new float[selection.boundarySize * selection.numPoints];
    
    // Associate a number with each vertex
    std::map<Point, int> mapping;
    std::list<Point> orderedPoints;
    int index = 0;
    for (FiniteFacesIterator iter = adaptiveMesh->finite_faces_begin() ; iter != adaptiveMesh->finite_faces_end() ; ++iter) {
        Triangle triangle = adaptiveMesh->triangle(iter);
        Point v1 = triangle.vertex(0);
        if (mapping.find(v1) == mapping.end()) {
            mapping[v1] = index++;
            orderedPoints.push_back(v1);
        }
        Point v2 = triangle.vertex(1);
        if (mapping.find(v2) == mapping.end()) {
            mapping[v2] = index++;
            orderedPoints.push_back(v2);
        }
        Point v3 = triangle.vertex(2);
        
        if (mapping.find(v3) == mapping.end()) {
            mapping[v3] = index++;
            orderedPoints.push_back(v3);
        }
    }
    
    // Verices
    int i = 0;
    for (std::list<Point>::const_iterator iter = orderedPoints.begin(); iter != orderedPoints.end() ; iter++)
    {
        selection.vertices[i].x = iter->x() ;
        selection.vertices[i].y = iter->y() ;
        i++;
    }
	
	
    // Triangles
	// Find inside triangles
	bool * trianglesToSkip = new bool[selection.numTriangles];
	selection.numTriangles = 0;
    i = 0;
	
	QRectF bb = selectionPoly.boundingRect();
	QPolygonF rect(bb);
	
	QPolygonF substr = selectionPoly.subtracted(rect);
	
    for (FiniteFacesIterator iter = adaptiveMesh->finite_faces_begin() ; iter != adaptiveMesh->finite_faces_end() ; ++iter)
    {
        Triangle triangle = adaptiveMesh->triangle(iter);
        Point v1 = triangle.vertex(0);
        Point v2 = triangle.vertex(1);
        Point v3 = triangle.vertex(2);
        
		
        if ((*iter).is_in_domain()) {
			selection.numTriangles++;
            trianglesToSkip[i] = false;
		}
        else
            trianglesToSkip[i] = true;
        
        i++;
    }
	selection.triangles = new Triangles[selection.numTriangles];
	qDebug() << selection.numTriangles << " inside the selection.";
    i = 0; int j = 0;
	// Add triangles
    for (FiniteFacesIterator iter = adaptiveMesh->finite_faces_begin() ; iter != adaptiveMesh->finite_faces_end() ; ++iter)
    {
        Triangle triangle = adaptiveMesh->triangle(iter);
        Point v1 = triangle.vertex(0);
        int index1 = mapping[v1];
        Point v2 = triangle.vertex(1);
        int index2 = mapping[v2];
        Point v3 = triangle.vertex(2);
        int index3 = mapping[v3];
        
		if (!trianglesToSkip[i]) {
			selection.triangles[j].p1 = index1;
			selection.triangles[j].p2 = index2;
			selection.triangles[j].p3 = index3;
			j++;
		}
		i++;
    }	
    delete [] trianglesToSkip;
	
    // Boundary
	float meanX = 0;
	float meanY = 0;	
    i = 0;
    for(std::vector<Point>::const_iterator it = boundaryVector.begin(); it != boundaryVector.end(); ++it){
        selection.boundaryCoordsTex[i] = (*it).x();
        selection.boundaryCoordsTex[selection.boundarySize + i] = (*it).y();
        i++;
		
		meanX += (*it).x();
		meanY += (*it).y();	
    }
	
	selection.toOrigin.x = -meanX/selection.boundarySize;
	selection.toOrigin.y = -meanY/selection.boundarySize;
	
    glGenTextures(1, &tex3_id);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glBindTexture(target, tex3_id);
	// This causes Error 1280
//    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Don't filter
    glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(target, 0, GL_LUMINANCE32F_ARB, selection.boundarySize, 2, 0, GL_LUMINANCE, GL_FLOAT, selection.boundaryCoordsTex);
	errCheck("tex3_id");
    
    // Weights
    if (method==HIER1 || method==HIER2)	{
		int maxRelevantWeights = cloner.maxRelevantWeights;
		float * WID  = new float[maxRelevantWeights * selection.numPoints * 3];	
		
		//i=0;
		for (std::map<VertexHandle,HierarchicCoordinateVector*>::const_iterator iter = cmesh->beginVertexToHierarchicCoordinates() ;
			 iter != cmesh->endVertexToHierarchicCoordinates() ; ++iter) {
			Point v = iter->first->point();
			i = mapping[v];
			
			HierarchicCoordinateVector* hcv = iter->second;
			
			int numberOfCoords = hcv->getSize();
			//qDebug() << numberOfCoords << "  " <<  maxRelevantWeights;
			assert(numberOfCoords<=maxRelevantWeights);
			double* coords = hcv->getCoords();
			int* indices = hcv->getIndices();
			int* depths = hcv->getDepths();
			
			WID[(i*maxRelevantWeights)*3] = numberOfCoords;
			WID[(i*maxRelevantWeights)*3+1] = numberOfCoords;
			WID[(i*maxRelevantWeights)*3+2] = numberOfCoords;
			
			for (int j=0; j<numberOfCoords; j++){
				//qDebug() << indices[j];
				assert(indices[j]<selection.boundarySize);
				assert(coords[j]<=1.0);
				WID[(i*maxRelevantWeights + j+1)*3] = coords[j];
				if (coords[j]==1.0)
					qDebug() << coords[j];
				WID[(i*maxRelevantWeights + j+1)*3+1] = indices[j];
				WID[(i*maxRelevantWeights + j+1)*3+2] = depths[j];
			}
		}	

		
        // Upload to GPU
		glGenTextures(1, &tex2_id);
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glBindTexture(target, tex2_id);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// Don't filter
		glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(target, 0, GL_RGB32F_ARB, maxRelevantWeights, selection.numPoints, 0, GL_RGB, GL_FLOAT, WID) ;
		errCheck(QString("Line 579"));	
        delete[] WID;
    }
    else {
        
        
		for (std::map<VertexHandle,double*>::const_iterator iter = cmesh->beginVertexToCoordinates() ; iter != cmesh->endVertexToCoordinates() ; ++iter) {
			// Index correspond to the position in the texture
			Point v = (*iter).first->point();
			int index = mapping[v];
			for (int j=0; j<selection.boundarySize; j++){
				selection.weightsTex[index*selection.boundarySize + j] = (*iter).second[j];
			}
		}
		
        // Upload to GPU
        target = GL_TEXTURE_RECTANGLE_ARB;
        glGenTextures(1, &tex2_id);
        glActiveTextureARB(GL_TEXTURE2_ARB);
        glBindTexture(target, tex2_id);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
        // Don't filter
        glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(target, 0, GL_LUMINANCE32F_ARB, selection.boundarySize, selection.numPoints, 0, GL_LUMINANCE, GL_FLOAT, selection.weightsTex) ;
		errCheck();
	}
	
    init = true;
    
	QString message = tr("Drag the selection around the target image. For more help, see: about MVCloner");
	((QMainWindow *)this->parentWidget())->statusBar()->showMessage(message);
}

void GLWidget::errCheck(QString mymsg){
    GLenum e;
    if ((e=glGetError()) != GL_NO_ERROR)
        qDebug()  << mymsg << " Error " << e;
} 


void GLWidget::setMethod(int newMethod) {
	
	switch(newMethod)
    {
    case 0:
		if (method==HIER1)
			return;
		else
			method=HIER1;
        break;
			
	case 1:
		if (method==HIER2)
			return;
		else {
			method=HIER2;
			// Create mipmaps
			bindSource();
			bindTarget();
		}
		break;
			
	case 2:
		if (method==ADAP1)
			return;
		else
			method=ADAP1;	
		break;
			
	case 3:
		if (method==ADAP2)
			return;
		else
			method=ADAP2;
		break;	
    }	
		
	this->method = method;
	
	// Load other set of shaders and update the result
	loadShaders();
	if (!init) {
		return;
	}
	
	updateSelection(boundaryVector, selectionPoly, false);
	
	update();
}
