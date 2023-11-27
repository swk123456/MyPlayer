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
    void delay(const int& ms);//延时, 不能直接sleep延时，UI主线程不能直接被阻塞，不然会有问题的

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


    //初始化布局
    void setupUi(QMainWindow* VideoFormClass);
    //初始化名称大小等
    void retranslateUi(QMainWindow* VideoFormClass);
    //初始化信号槽
    void signalSlot(QMainWindow* VideoFormClass);

};