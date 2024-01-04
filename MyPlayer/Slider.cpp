#include "Slider.h"

void Slider::mousePressEvent(QMouseEvent* e)
{
	double pos = (double)e->pos().x() / (double)width();
	setValue(pos * this->maximum());
	//ԭ���¼�����
	//QSlider::mousePressEvent(e);
	QSlider::sliderReleased();
}

void Slider::mouseMoveEvent(QMouseEvent* e)
{
	QSlider::mouseMoveEvent(e);

	int moveTime = (double)e->pos().x() / (double)width() * (double)totalTime;

	// ��ȡԤ����Ϣ�������Ǽ򵥵�ʾ��������Ҫ�滻Ϊʵ�ʵ�Ԥ����Ϣ
	QString previewInfo = QString("%1:%2").arg(moveTime / 60).arg(moveTime % 60, 2, 10, QLatin1Char('0'));

	// ������ƶ�ʱ��ʾԤ����Ϣ
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