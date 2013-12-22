/****************************************************************************
 ** Image Cloning tool, based on Mean-Value Coordinates.
 **
 ** For more details see "Coordinates for Instant Image Cloning" by
 ** Farbman, Hoffer, Cohen-Or, Lipman and Lischinski 2009
 ****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sourcewidget.h"
#include "MVCCLoner.h"

class QLabel;
class GLWidget;


/* Main Window is a parent of SourceWidget(selection scribble) and
   GLWidget(result of the cloning).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
	
protected:
    void keyPressEvent(QKeyEvent * event);
	void contextMenuEvent(QContextMenuEvent *event);
	
private slots:
    void openImages();
    void quickOpen();
    void saveImage();
	void about();
	
	void viewMVC() { glWidget->showMVC(); }
	void viewComp() { glWidget->showComp(); }
	void viewMembrane() { glWidget->showMembrane(); }
	void viewMesh() { glWidget->showMesh(); }

	void rotateCW() { glWidget->rotateCW(); }
	void rotateCCW() { glWidget->rotateCCW(); }
	void scaleUp() { glWidget->scaleUp(); }
	void scaleDown() { glWidget->scaleDown(); }	
	void flip() { glWidget->flip(); }	
	void resetSelection() { glWidget->resetSelection(); }	
	
	void setHierarchical(){ glWidget->setMethod(0); }
	void setHierarchical2DFilter(){ glWidget->setMethod(1); }
	void notToBlend(){ glWidget->setMethod(2);}
	void toBlend(){ glWidget->setMethod(3);}
	
private:
    void createActions();
    void createMenus();

    void setImages(QString & source, QString & target);

    GLWidget * glWidget;
    SourceWidget * sourceWindow;

    QMenu * fileMenu;
	QMenu * viewMenu;
	QMenu * transformMenu;
	QMenu * methodMenu;
	
	QMenu * helpMenu;
	
	QMenu * view;
	
	QAction * viewMVCAct;
	QAction * viewCompAct;
	QAction * viewMembraneAct;
	QAction * viewTriAct;
	
	QAction * scaleUpAct;
	QAction * scaleDownAct;
	QAction * rotateCWAct;
	QAction * rotateCCWAct;	
	QAction * flipAct;	
	QAction * resetSelectionAct;
	
    QAction * openImagesAct;
    QAction * quickOpenAct;
    QAction * saveImageAct;
	QAction * aboutAct;
	QAction * optionsAct;
	
	QAction * adaptiveActionLabel;
	QAction * hierarchicalActionLabel;	
	
	QAction * setHierarchicalAct;
	QAction * setHierarchical2DFilterAct;
	//QAction * setAdaptiveAct;
	
	QAction * blendAct;
	QAction * noBlendAct;	
	
	
	QActionGroup * viewGroup;
	
	QActionGroup * methodGroup;

};

#endif
