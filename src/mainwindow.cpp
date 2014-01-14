#include <QtGui>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QLayout>

#include "mainwindow.h"
#include "glwidget.h"
#include "sourcewidget.h"
#include "utils.h"

MainWindow::MainWindow(){
	
	// Set the current directory to Resources inside the bundle
	// Should be called before glWidget init
#ifdef MACOSX
	bool ok = setResourcesDir();
	assert(ok);
#endif
	
    glWidget = new GLWidget(this);
    setCentralWidget(glWidget);
	glWidget->setFixedSize(400, 300); 
	
    setWindowTitle(tr("Target Image"));
	layout()->setSizeConstraint(QLayout::SetFixedSize);

    createActions();
    createMenus();

    grabKeyboard();

    sourceWindow = 0;
	
	QString message = tr("Cmd-U to open default images. Cmd-O to open images");
	statusBar()->showMessage(message);	
}

void MainWindow::createActions(){

	// File
    openImagesAct = new QAction(tr("&Open Images"), this);
    openImagesAct->setShortcut(QKeySequence::Open);
    openImagesAct->setStatusTip(tr("Open source/target images"));
    connect(openImagesAct, SIGNAL(triggered()), this, SLOT(openImages()));

    quickOpenAct = new QAction(tr("Q&uick Open"), this);
    quickOpenAct->setShortcut(tr("Ctrl+U"));
    quickOpenAct->setStatusTip(tr("Open default source/target images"));
    connect(quickOpenAct, SIGNAL(triggered()), this, SLOT(quickOpen()));

    saveImageAct = new QAction(tr("&Save Result"), this);
    saveImageAct->setShortcut(QKeySequence::Save);
    saveImageAct->setStatusTip(tr("Save the result"));
    connect(saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));
	
	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("About MVCloner"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
//	optionsAct = new QAction(tr("&Preferences"), this);
//	optionsAct->setStatusTip(tr("Show help"));
//	connect(optionsAct, SIGNAL(triggered()), this, SLOT(options()));	
	
	// View
	viewMVCAct = new QAction(tr("&MVC"), this);
	viewMVCAct->setShortcut(tr("S"));
	viewMVCAct->setStatusTip(tr("View seamless cloning result"));
	viewMVCAct->setCheckable(true);
	connect(viewMVCAct, SIGNAL(triggered()), this, SLOT(viewMVC()));
	
	viewCompAct = new QAction(tr("&Composite"), this);
	viewCompAct->setShortcut(tr("C"));
	viewCompAct->setStatusTip(tr("View source patch"));
	viewCompAct->setCheckable(true);
	connect(viewCompAct, SIGNAL(triggered()), this, SLOT(viewComp()));	
	
	viewMembraneAct = new QAction(tr("&Membrane"), this);
	viewMembraneAct->setShortcut(tr("M"));
	viewMembraneAct->setStatusTip(tr("View (shifted) membrane"));
	viewMembraneAct->setCheckable(true);
	connect(viewMembraneAct, SIGNAL(triggered()), this, SLOT(viewMembrane()));		
	
	viewTriAct = new QAction(tr("&Mesh"), this);
	viewTriAct->setShortcut(tr("T"));
	viewTriAct->setStatusTip(tr("View triangulation"));
	viewTriAct->setCheckable(true);
	connect(viewTriAct, SIGNAL(triggered()), this, SLOT(viewMesh()));	
	
	viewGroup = new QActionGroup(this);
	viewGroup->addAction(viewMVCAct);
	viewGroup->addAction(viewCompAct);
	viewGroup->addAction(viewMembraneAct);
	viewGroup->addAction(viewTriAct);
	viewMVCAct->setChecked(true);	
	
	
	// Transform
	scaleUpAct = new QAction(tr("&Scale Up"), this);
	scaleUpAct->setShortcut(tr("+"));
	scaleUpAct->setStatusTip(tr("Scale selection up"));
	connect(scaleUpAct, SIGNAL(triggered()), this, SLOT(scaleUp()));	
	
	scaleDownAct = new QAction(tr("&Scale Down"), this);
	scaleDownAct->setShortcut(tr("-"));
	scaleDownAct->setStatusTip(tr("Scale selection down"));
	connect(scaleDownAct, SIGNAL(triggered()), this, SLOT(scaleDown()));	
	
	rotateCWAct = new QAction(tr("&Rotate Clockwise"), this);
	rotateCWAct->setShortcut(tr("["));
	rotateCWAct->setStatusTip(tr("Rotate selection clockwise"));
	connect(rotateCWAct, SIGNAL(triggered()), this, SLOT(rotateCW()));	
	
	rotateCCWAct = new QAction(tr("&Rotate Counterclockwise"), this);
	rotateCCWAct->setShortcut(tr("]"));
	rotateCCWAct->setStatusTip(tr("Rotate selection counterclockwise"));
	connect(rotateCCWAct, SIGNAL(triggered()), this, SLOT(rotateCCW()));	
	
	flipAct = new QAction(tr("&Flip"), this);
	flipAct->setShortcut(tr("F"));
	flipAct->setStatusTip(tr("Flip selection horizontally"));
	connect(flipAct, SIGNAL(triggered()), this, SLOT(flip()));
	
	resetSelectionAct = new QAction(tr("&Reset"), this);
	resetSelectionAct->setShortcut(tr("R"));
	resetSelectionAct->setStatusTip(tr("Undo all transformations"));
	connect(resetSelectionAct, SIGNAL(triggered()), this, SLOT(resetSelection()));	
	
	
	// Other
	setHierarchicalAct = new QAction(tr("    &Fast"), this);
	setHierarchicalAct->setStatusTip(tr("Use adaptive mesh and hierarchical sampling (fast)"));
	setHierarchicalAct->setCheckable(true);
	connect(setHierarchicalAct, SIGNAL(triggered()), this, SLOT(setHierarchical()));	
	
	setHierarchical2DFilterAct = new QAction(tr("    &2D Filter"), this);
	setHierarchical2DFilterAct->setStatusTip(tr("Use adaptive mesh and hierarchical sampling with 2D filtering(slow)"));
	setHierarchical2DFilterAct->setCheckable(true);
	connect(setHierarchical2DFilterAct, SIGNAL(triggered()), this, SLOT(setHierarchical2DFilter()));	
	
	blendAct = new QAction(tr("    &Blending"), this);
	blendAct->setStatusTip(tr("Adaptive mesh without hierarchical sampling (slow)"));
	blendAct->setCheckable(true);
	connect(blendAct, SIGNAL(triggered()), this, SLOT(toBlend()));	
	
	noBlendAct = new QAction(tr("    &No Blending"), this);
	noBlendAct->setStatusTip(tr("Adaptive mesh without hierarchical sampling, with boundary blending (very slow)"));
	noBlendAct->setCheckable(true);
	connect(noBlendAct, SIGNAL(triggered()), this, SLOT(notToBlend()));
	
	methodGroup = new QActionGroup(this);
	methodGroup->addAction(setHierarchicalAct);
	methodGroup->addAction(setHierarchical2DFilterAct);
	methodGroup->addAction(blendAct);
	methodGroup->addAction(noBlendAct);
	setHierarchicalAct->setChecked(true);	
	


}

void MainWindow::createMenus(){

	fileMenu = new QMenu(tr("&File"), this);
	//fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openImagesAct);
    fileMenu->addAction(quickOpenAct);
	fileMenu->addSeparator();
    fileMenu->addAction(saveImageAct);
	menuBar()->addMenu(fileMenu);
	
	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(viewMVCAct);
	viewMenu->addAction(viewCompAct);
	viewMenu->addAction(viewMembraneAct);
	viewMenu->addAction(viewTriAct);
	menuBar()->addMenu(viewMenu);
	
	transformMenu = new QMenu(tr("&Transform"), this);
	transformMenu->addAction(scaleUpAct);
	transformMenu->addAction(scaleDownAct);
	transformMenu->addAction(rotateCWAct);
	transformMenu->addAction(rotateCCWAct);
	transformMenu->addAction(flipAct);
	transformMenu->addSeparator();
	transformMenu->addAction(resetSelectionAct);
	
	menuBar()->addMenu(transformMenu);	
	
	methodMenu = new QMenu(tr("&Method"), this);
	
	hierarchicalActionLabel = new QAction(tr("&Hierarchical"), this);
	QFont boldFont = hierarchicalActionLabel->font();
    boldFont.setBold(true);
    hierarchicalActionLabel->setFont(boldFont);	
	
	methodMenu->addAction(hierarchicalActionLabel);
	methodMenu->addAction(setHierarchicalAct);
	methodMenu->addAction(setHierarchical2DFilterAct);
	methodMenu->addSeparator();
	adaptiveActionLabel = new QAction(tr("&Adaptive"), this);
	adaptiveActionLabel->setFont(boldFont);	
	methodMenu->addAction(adaptiveActionLabel);
	methodMenu->addAction(noBlendAct);
	methodMenu->addAction(blendAct);
	
	menuBar()->addMenu(methodMenu);
	
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	menuBar()->addMenu(helpMenu);
    
}

void MainWindow::quickOpen(){

    QString source(":/images/source.jpg");
    QString target(":/images/target.jpg");
#ifdef MACOSX
	setResourcesDir();
#endif
    setImages(source, target);

}

void MainWindow::openImages(){

    QString source = QFileDialog::getOpenFileName(this,
        tr("Open Source Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    QString target = QFileDialog::getOpenFileName(this,
        tr("Open Target Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    setImages(source, target);

}

void MainWindow::setImages(QString & source, QString & target){
	
    if (!source.isEmpty()) {
        QImage image(source);
        if (image.isNull()) {
            QMessageBox::information(this, tr("MVCloner"),
                                     tr("Cannot load %1.").arg(source));
            return;
        }
        if (!sourceWindow)
            sourceWindow = new SourceWidget(this, glWidget);
        sourceWindow->setImage(image);
        sourceWindow->setFixedSize(image.width(), image.height());
        sourceWindow->show();
     }

    if (!target.isEmpty()) {
        QImage image(target);
        if (image.isNull()) {
            QMessageBox::information(this, tr("MVCloner"),
                                     tr("Cannot load %1.").arg(target));
            return;
        }
        glWidget->setTargetImage(image);
        glWidget->resize(image.width(), image.height());
        glWidget->setFixedSize(image.width(), image.height()); 
        glWidget->show();
    }
	
	QString message = tr("Make a selection in the source image.");
	statusBar()->showMessage(message);
	
	glWidget->notInit(); // Till user selects something
	glWidget->update();
}

void MainWindow::saveImage(){

    QString filename = QFileDialog::getSaveFileName(this,
        tr("Choose a filename to save under"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    QImage image = glWidget->grabFrameBuffer();
    image.save(filename);
}

void MainWindow::keyPressEvent(QKeyEvent * event){
    Q_UNUSED(event)
	// All the keyboard shortcuts are delegated via QAction
	;
//    switch( event->key() )
//    {
//    case Qt::Key_C:
//        glWidget->showMVC();
//        glWidget->update();
//        break;
//    }

}

void MainWindow::about() {

	QString text0("MVCloner is a seamless cloning tool, based on Mean-Value Coordinates. \
				  For more information, visit <a href='http://www.cs.huji.ac.il/~danix/mvclone/'>our project site</a>. \
				  We are likely to update this tool, so stay tuned. If you encounter a showstopper bugs, \
				  please contact <a href='farbmv@cs.huji.ac.il'>Zeev Farbman</a> or <a href='gilhoffer@gmail.com'>Gil Hoffer</a>");
	QMessageBox::about(this, tr("About Menu"), text0);
	
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	// For the context menus you can either re-use existing menus/actions or create (even on the stack)
	// new menus.
	QMenu menu(this);
	
	menu.addAction(openImagesAct);
	menu.addAction(quickOpenAct);
	menu.addAction(saveImageAct);
	menu.addSeparator();
	
	menu.addMenu(viewMenu);		
	menu.addMenu(transformMenu);	
	menu.addMenu(methodMenu);

	menu.exec(event->globalPos());
}
