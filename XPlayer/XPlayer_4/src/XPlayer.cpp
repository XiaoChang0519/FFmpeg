#include "XPlayer.h"

XPlayer::XPlayer(QWidget* parent)
	: QWidget(parent)
{
	// ��ʼ�����ڴ�С
	this->setFixedSize(600, 400);

	// ��ʼ�����ڲ���
	video = new XVideoWidget;
	m_pBtnOpen = new QPushButton;
	m_pBtnPlay = new QPushButton;
	m_pSliderVideo = new QSlider;
	m_pLabCurTime = new QLabel;
	m_pLabTotal = new QLabel;
	m_pBtnOpen->setFixedSize(53, 23);
	m_pBtnPlay->setFixedSize(53, 23);
	m_pBtnOpen->setText(QStringLiteral("��"));
	m_pBtnPlay->setText(QStringLiteral("����"));
	m_pLabCurTime->setText("00:00:00");
	m_pLabTotal->setText("00:00:00");
	m_pLabCurTime->setStyleSheet("QLabel{color:white;font-size:16px;}");
	m_pLabTotal->setStyleSheet("QLabel{color:white;font-size:16px;}");
	m_pLabCurTime->hide();
	m_pLabTotal->hide();

	// ����Ƶ�����Ű�ť-ˮƽ����
	QHBoxLayout* pHLayoutBtn = new QHBoxLayout;
	pHLayoutBtn->addStretch();
	pHLayoutBtn->addWidget(m_pBtnOpen);
	pHLayoutBtn->addSpacing(12);
	pHLayoutBtn->addWidget(m_pBtnPlay);
	pHLayoutBtn->addStretch();

	// ��ǰ����ʱ���ǩ���-ˮƽ����
	QHBoxLayout* pHLayoutLab = new QHBoxLayout;
	pHLayoutLab->addSpacing(16);
	pHLayoutLab->addWidget(m_pLabCurTime);
	pHLayoutLab->addStretch();
	pHLayoutLab->addWidget(m_pLabTotal);
	pHLayoutLab->addSpacing(16);

	// ������
	QVBoxLayout* pVLayout = new QVBoxLayout(this);
	pVLayout->addWidget(video);
	pVLayout->addWidget(m_pSliderVideo);
	pVLayout->addLayout(pHLayoutLab);
	pVLayout->addLayout(pHLayoutBtn);
	pVLayout->addSpacing(12);

	// ��ʼ����Ƶ������
	m_pSliderVideo->setOrientation(Qt::Horizontal);
	m_pSliderVideo->setMinimum(0);
	m_pSliderVideo->setMaximum(100);
	m_pSliderVideo->setValue(0);
	m_pSliderVideo->setFixedHeight(16);
}
