#ifndef MVCDEMO_H
#define MVCDEMO_H

#include <QtGui/QMainWindow>
#include "ui_mvcdemo.h"

class MVCDemo : public QMainWindow
{
	Q_OBJECT

public:
	MVCDemo(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MVCDemo();

private:
	Ui::MVCDemoClass ui;
};

#endif // MVCDEMO_H
