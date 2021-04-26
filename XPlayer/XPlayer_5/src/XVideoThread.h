#pragma once
#include <iostream>
#include <list>
#include <mutex>
#include <QThread>
#include "XDecode.h"
#include "IVideoCall.h"

using namespace std;

// �������ʾ��Ƶ
struct AVPacket;
struct AVCodecParameters;
class XDecode;

class XVideoThread:public QThread
{
public:	
	virtual bool open(AVCodecParameters *para, IVideoCall *call, int width,int height); // �򿪣����ܳɹ��������
	virtual void push(AVPacket *pkt); // ��AVPacket���뵽�����еȴ�����ת��
	void run(); // �Ӷ����л�ȡAVPacket���н���

	XVideoThread();
	virtual ~XVideoThread();
	
	int maxList = 100; // ������
	bool isExit = false; // �߳��Ƿ����

protected:
	std::list <AVPacket *> m_pktList; // AVPacket����
	std::mutex m_mutex;
	XDecode *decode = 0; // ������
	IVideoCall *call = 0;
};

