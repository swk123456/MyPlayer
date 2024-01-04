#include "Slider.h"

void Slider::mousePressEvent(QMouseEvent* e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());
	//原有事件处理
	//QSlider::mousePressEvent(e);
	QSlider::sliderReleased();
}

void Slider::mouseMoveEvent(QMouseEvent* e)
{
	QSlider::mouseMoveEvent(e);

	int moveTime = (double)e->pos().x() / (double)width() * (double)totalTime;

	// 获取预览信息，这里是简单的示例，你需要替换为实际的预览信息
	QString previewInfo = QString("%1:%2").arg(moveTime / 60).arg(moveTime % 60, 2, 10, QLatin1Char('0'));

	// 在鼠标移动时显示预览信息
	//QToolTip::showText(e->globalPos(), previewInfo, this);
	emit showPreview(moveTime, e->pos().x(), e->pos().y());
}

void Slider::leaveEvent(QEvent* e)
{
	QWidget::leaveEvent(e);

	emit closePreview();
}

Slider::Slider(QWidget* parent)
	: QSlider(parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
}

Slider::~Slider()
{
}