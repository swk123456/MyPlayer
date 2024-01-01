#include "Slider.h"

void Slider::mousePressEvent(QMouseEvent* e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());
	//ԭ���¼�����
	//QSlider::mousePressEvent(e);
	QSlider::sliderReleased();
}

Slider::Slider(QWidget* parent)
	: QSlider(parent)
{
}

Slider::~Slider()
{
}