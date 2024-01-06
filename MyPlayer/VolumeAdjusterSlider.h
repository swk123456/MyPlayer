#pragma once
#include <QObject>
#include <QMouseEvent>
#include <QSlider>

class VolumeAdjusterSlider : public QSlider
{
	Q_OBJECT
public:
	VolumeAdjusterSlider(QWidget* parent = NULL);
	~VolumeAdjusterSlider();
	void mousePressEvent(QMouseEvent* e);

private:

};

