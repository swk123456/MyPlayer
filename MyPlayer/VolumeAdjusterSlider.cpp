#include "VolumeAdjusterSlider.h"

VolumeAdjusterSlider::VolumeAdjusterSlider(QWidget* parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
}

VolumeAdjusterSlider::~VolumeAdjusterSlider()
{
}

void VolumeAdjusterSlider::mousePressEvent(QMouseEvent* e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());

	//原有事件处理
	//QSlider::mousePressEvent(e);
	QSlider::sliderReleased();
}
