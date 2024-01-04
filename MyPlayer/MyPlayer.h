#pragma once

#include <QCoreApplication>
#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "Slider.h"
#include "PreviewLabel.h"
#include "VideoOutput.h"
#include "PlayerControl.h"

class MyPlayer : public QWidget
{
	Q_OBJECT
		class Ui_MyPlayer;
public:
	MyPlayer(QWidget* parent = Q_NULLPTR);
	~MyPlayer();

	VideoOutput* getVideo();

	// 定时器 滑动条显示
	void timerEvent(QTimerEvent* e);
	// 窗口尺寸变化
	void resizeEvent(QResizeEvent* e);
	// 双击全屏
	void mouseDoubleClickEvent(QMouseEvent* e);

public slots:
	void OpenFile();
	void PlayOrPause();
	void SliderPress();
	void SliderRelease();
    void SliderOn(int moveTime, int x, int y);
    void SliderLeave();
	void SetNowPts(int time);

private:
    QString fileName;
	PlayerControl playerControl;
	bool isSliderPress = false;
	bool isPause = false;
	QScopedPointer<Ui_MyPlayer> ui;
};

class MyPlayer::Ui_MyPlayer
{
public:
    VideoOutput* video;
    QPushButton* openFile;
    QLabel* nowtimeLabel;
    Slider* playPos;
    QLabel* endtimeLabel;
    QPushButton* isplay;
    PreviewLabel* previewLabel;

    void setupUi(QWidget* PlayClass)
    {
        if (PlayClass->objectName().isEmpty())
            PlayClass->setObjectName(QString::fromUtf8("PlayClass"));
        PlayClass->resize(1280, 720);
        video = new VideoOutput(PlayClass);
        video->setObjectName(QString::fromUtf8("video"));
        video->setGeometry(QRect(0, 0, 640, 360));
        openFile = new QPushButton(PlayClass);
        openFile->setObjectName(QString::fromUtf8("openFile"));
        openFile->setGeometry(QRect(560, 630, 81, 41));
        nowtimeLabel = new QLabel(PlayClass);
        nowtimeLabel->setObjectName(QString::fromUtf8("nowtimeLabel"));
        nowtimeLabel->setGeometry(QRect(10, 680, 50, 31));
        playPos = new Slider(PlayClass);
        playPos->setObjectName(QString::fromUtf8("playPos"));
        playPos->setGeometry(QRect(65, 680, 1150, 31));
        playPos->setMaximum(999);
        playPos->setPageStep(1);
        playPos->setOrientation(Qt::Horizontal);
        endtimeLabel = new QLabel(PlayClass);
        endtimeLabel->setObjectName(QString::fromUtf8("endtimeLabel"));
        endtimeLabel->setGeometry(QRect(1220, 680, 50, 31));
        isplay = new QPushButton(PlayClass);
        isplay->setObjectName(QString::fromUtf8("isplay"));
        isplay->setGeometry(QRect(660, 630, 91, 41));
        previewLabel = new PreviewLabel(PlayClass);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        previewLabel->setGeometry(QRect(400, 150, 480, 420));
        previewLabel->setVisible(false);

        retranslateUi(PlayClass);
        QObject::connect(openFile, SIGNAL(clicked()), PlayClass, SLOT(OpenFile()));
        QObject::connect(isplay, SIGNAL(clicked()), PlayClass, SLOT(PlayOrPause()));
        QObject::connect(playPos, SIGNAL(sliderPressed()), PlayClass, SLOT(SliderPress()));
        QObject::connect(playPos, SIGNAL(sliderReleased()), PlayClass, SLOT(SliderRelease()));
        QObject::connect(playPos, SIGNAL(showPreview(int, int, int)), PlayClass, SLOT(SliderOn(int, int, int)));
        QObject::connect(playPos, SIGNAL(closePreview()), PlayClass, SLOT(SliderLeave()));
        //QObject::connect(&dt, SIGNAL(setNowTime(int)), PlayClass, SLOT(SetNowPts(int)));

        QMetaObject::connectSlotsByName(PlayClass);
    } // setupUi

    void retranslateUi(QWidget* PlayClass)
    {
        PlayClass->setWindowTitle(QCoreApplication::translate("PlayClass", "Player", nullptr));
        openFile->setText(QString::fromLocal8Bit("打开文件"));
        isplay->setText(QString::fromLocal8Bit("播放"));
        nowtimeLabel->setText(QCoreApplication::translate("PlayClass", "00:00", nullptr));
        endtimeLabel->setText(QCoreApplication::translate("PlayClass", "00:00", nullptr));
    } // retranslateUi

};