#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QPainter>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QMouseEvent>
#include <QEvent>
#include <QStyle>
#include <QDebug>
#include "XVideoWidget.h"

class XPlayer : public QWidget
{
    Q_OBJECT

public:
    XPlayer(QWidget *parent = Q_NULLPTR);

	XVideoWidget* video;

private:
	//bool eventFilter(QObject* obj, QEvent* event); // ��������л��������鶨λ
	//void sliderMouseLPress(QObject* slider, QEvent* event); // ���������鶨λ
	//char* getTimeStr(const int& nSecCnt); // ������������ȡ"ʱ�����ַ���"

	QPushButton* m_pBtnOpen; // ����Ƶ��ť
	QPushButton* m_pBtnPlay; // ���Ű�ť
	QSlider* m_pSliderVideo; // ��Ƶ������
	QLabel* m_pLabCurTime; // ��ǰ����ʱ��
	QLabel* m_pLabTotal; // ��ǰ����ʱ��

	QImage m_image; // ���ܴ��FFmpegת������ÿһ֡ͼƬ����
	//VPlayThread* m_vPlayThread; // ��Ƶ�����߳�
	QTimer* m_pTimerPts;

	bool m_isPressSlider = false; // �Ƿ��»�����
	bool m_bIsPlay = false; // �Ƿ��ڲ���
};
