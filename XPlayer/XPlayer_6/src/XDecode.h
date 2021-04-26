#pragma once
#include <iostream>
#include <mutex>
using namespace std;

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);

class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	bool Open(AVCodecParameters* codecPara); // �򿪽�����
	bool Send(AVPacket* pkt); // ���͵������߳�
	AVFrame* Recv(); // ��ȡ��������
	void Clear(); // ����
	void Close(); // �ر�

	bool m_isAudio = false; // �Ƿ�Ϊ��Ƶ�ı�־λ
	long long pts = 0; // ��ǰ���뵽��pts������ӣ�Ϊ������Ƶͬ����

private:
	AVCodecContext *m_VCodecCtx = 0;
	std::mutex m_mutex;
};

