#include "MyPlayer.h"

MyPlayer::MyPlayer(QWidget* parent)
	: QWidget(parent), ui(new Ui_MyPlayer)
{
	ui->setupUi(this);
	startTimer(40);
}

MyPlayer::~MyPlayer()
{
}

VideoOutput* MyPlayer::getVideo()
{
	return ui->video;
}

void MyPlayer::SliderPress()
{
	isSliderPress = true;
}

void MyPlayer::SliderRelease()
{
	isSliderPress = false;
	double pos = 0.0;
	pos = (double)ui->playPos->value() / (double)ui->playPos->maximum();
	playerControl.ResetStart(pos);
}

void MyPlayer::SliderOn(int moveTime, int x, int y)
{
	ui->previewLabel->setTime(fileName, moveTime);
	ui->previewLabel->setVisible(true);
}

void MyPlayer::SliderLeave()
{
	ui->previewLabel->closeLabel();
	ui->previewLabel->setVisible(false);
}

void MyPlayer::SetNowPts(int time)
{
	ui->nowtimeLabel->setText(QString("%1:%2").arg(time / 60).arg((time) % 60, 2, 10, QLatin1Char('0')));
}

// ��ʱ�� ��������ʾ
void MyPlayer::timerEvent(QTimerEvent* e)
{
	if (isSliderPress)return;
	double total = playerControl.GetTotalPts();
	if (total > 0)
	{
		double now = playerControl.GetNowPts();
		double pos = now / total;
		SetNowPts(now);
		int v = ui->playPos->maximum() * pos;
		ui->playPos->setValue(v);

		if (total - now < 0.1)//�Ѿ�������
		{
			PlayOrPause();
		}
	}
}

// ˫��ȫ��
void MyPlayer::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen())
		this->showNormal();
	else
		this->showFullScreen();
}

// ���ڳߴ�仯
void MyPlayer::resizeEvent(QResizeEvent* e)
{
	ui->nowtimeLabel->move(10, this->height() - 100);
	ui->playPos->move(ui->nowtimeLabel->x() + ui->nowtimeLabel->width() + 5, this->height() - 100);
	ui->playPos->resize(this->width() - 130, ui->playPos->height());
	ui->endtimeLabel->move(ui->playPos->x() + ui->playPos->width() + 5, this->height() - 100);
	ui->openFile->move(100, this->height() - 150);
	ui->isplay->move(ui->openFile->x() + ui->openFile->width() + 10, ui->openFile->y());
	ui->video->resize(this->size());
	ui->previewLabel->move(this->width() / 2 - 240, this->height() / 2 - 210);
	playerControl.SetSize(this->size().width(), this->size().height());
}

void MyPlayer::PlayOrPause()
{
	isPause = !isPause;
	if (isPause)
	{
		ui->isplay->setText(QString::fromLocal8Bit("��ͣ"));
		playerControl.status = 0;
		playerControl.Start();
	}
	else
	{
		ui->isplay->setText(QString::fromLocal8Bit("����"));
		playerControl.status = 1;
		playerControl.Pause();
	}
}

void MyPlayer::OpenFile()
{
	// ѡ���ļ�
	fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
	if (fileName.isEmpty())return;
	this->setWindowTitle(fileName);
	if (playerControl.Init(fileName.toStdString(), ui->video) < 0)
	{
		QMessageBox::information(0, "error", "open file failed!");
		return;
	}
	int totalTime = playerControl.GetTotalPts();
	ui->nowtimeLabel->setText(QString("00:00"));
	ui->endtimeLabel->setText(QString("%1:%2").arg(totalTime / 60).arg(totalTime % 60, 2, 10, QLatin1Char('0')));
	ui->playPos->setValue(0);
	ui->playPos->totalTime = totalTime;
	playerControl.SetSize(this->size().width(), this->size().height());
}