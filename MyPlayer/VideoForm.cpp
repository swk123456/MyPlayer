#include "VideoForm.h"
#include <Windows.h>

VideoForm::VideoForm(QWidget* parent)
	: QMainWindow(parent), ui(new Ui_VideoForm)
{
	ui->setupUi(this);
	ui->retranslateUi(this);
	ui->signalSlot(this);
	//onSendImage(QImage("player.png"), -1);

	this->show();

	for (int i = 0; i < 2332; i += 4)
	{
		//MessageBox(NULL, L"", L"", MB_OK);
		//onSendImage(QImage(QString("./pictures/%1.png").arg(int(i))), -2, 40);
	}
}

VideoForm::~VideoForm()
{
}

void VideoForm::onSendImage(const QImage& image, const int& pts, const int& delayTime)
{
	/*//image.save(QString("./pictures/%1.png").arg(pts), "PNG", 100);
	QPixmap pixmap = QPixmap::fromImage(image);
	QPixmap fitpixmap = pixmap.scaled(ui->labelPic->width(), ui->labelPic->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	ui->labelPic->setPixmap(fitpixmap);
	//ui->labelPic->setPixmap(QPixmap::fromImage(image).scaled(ui->labelPic->size()));
	//ui->labelPic->show();
	delay(delayTime);*/
	ui->labelPic->slot_showImage(image);
	delay(delayTime);
}

void VideoForm::delay(const int& ms)
{
	QTime stopTime;
	stopTime.start();
	//qDebug()<<"start:"<<QTime::currentTime().toString("HH:mm:ss.zzz");
	while (stopTime.elapsed() < ms)//stopTime.elapsed()返回从start开始到现在的毫秒数
	{
		QCoreApplication::processEvents();
	}
	//qDebug()<<"stop :"<<QTime::currentTime().toString("HH:mm:ss.zzz");
}

void VideoForm::Ui_VideoForm::setupUi(QMainWindow* VideoFormClass)
{
	if (VideoFormClass->objectName().isEmpty())
		VideoFormClass->setObjectName(QString::fromUtf8("VideoFormClass"));

	videoOutWidget = new QWidget(VideoFormClass);
	VideoFormClass->setCentralWidget(videoOutWidget);

	mainLayout = new QVBoxLayout(videoOutWidget);

	labelTitle = new QLabel;
	//labelPic = new QLabel;
	labelPic = new ctOpenglWidget;
	mainLayout->addWidget(labelTitle);
	mainLayout->addWidget(labelPic);
	
}

void VideoForm::Ui_VideoForm::retranslateUi(QMainWindow* VideoFormClass)
{
	VideoFormClass->setWindowTitle(QCoreApplication::translate("VideoFormClass", "MyPlayer", nullptr));
	VideoFormClass->setMinimumWidth(800);
	VideoFormClass->setMinimumHeight(800);

	labelTitle->setText("player---------------------------------------------------title");

	labelPic->setMinimumHeight(360);
	labelPic->setMinimumWidth(640);
}

void VideoForm::Ui_VideoForm::signalSlot(QMainWindow* VideoFormClass)
{
	
}








ctOpenglWidget::ctOpenglWidget(QWidget* parent) : QOpenGLWidget(parent)
{
}

ctOpenglWidget::~ctOpenglWidget()
{
}

void ctOpenglWidget::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e)
		QPainter painter;

	painter.begin(this);//清理屏幕
	painter.drawImage(QPoint(0, 0), m_image);//绘制FFMpeg解码后的视频
	painter.end();
}

void ctOpenglWidget::slot_showImage(const QImage& image)
{
	m_image = image;

	update();
}