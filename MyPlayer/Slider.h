#pragma once

#include <QObject>
#include <QMouseEvent>
#include <QSlider>
#include <QToolTip>

class Slider : public QSlider
{
	Q_OBJECT

public:
	Slider(QWidget* parent = NULL);
	~Slider();
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void leaveEvent(QEvent* e);

public:
	int totalTime = 0;

signals:
	void showPreview(int moveTime, int x, int y);
	void closePreview();
};

