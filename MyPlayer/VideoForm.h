#pragma once

#include <QApplication>
#include <QtWidgets/QMainWindow>
#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QTime>
#include <QOpenGLWidget>
#include <QPainter>

class VideoForm : public QMainWindow
{
    Q_OBJECT
        class Ui_VideoForm;
public:
    explicit VideoForm(QWidget* parent = nullptr);
    ~VideoForm();
    void onSendImage(const QImage& image, const int& pts, const int& delayTime = 10);
    void delay(const int& ms);//��ʱ, ����ֱ��sleep��ʱ��UI���̲߳���ֱ�ӱ���������Ȼ���������

private:
    QScopedPointer<Ui_VideoForm> ui;
};

class ctOpenglWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    ctOpenglWidget(QWidget* parent = nullptr);
    ~ctOpenglWidget();

    void slot_showImage(const QImage& image);

protected:
    void paintEvent(QPaintEvent* e);

private:
    QImage m_image;

};

class VideoForm::Ui_VideoForm
{
public:
    QWidget* videoOutWidget;
    QVBoxLayout* mainLayout;
    QLabel* labelTitle;
    ctOpenglWidget* labelPic;


    //��ʼ������
    void setupUi(QMainWindow* VideoFormClass);
    //��ʼ�����ƴ�С��
    void retranslateUi(QMainWindow* VideoFormClass);
    //��ʼ���źŲ�
    void signalSlot(QMainWindow* VideoFormClass);

};