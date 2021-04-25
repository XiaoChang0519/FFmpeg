#pragma once

#include <iostream>
#include <mutex>

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"swresample.lib")

using namespace std;

struct AVCodecParameters;
struct AVFrame;
struct SwrContext;

class XResample
{
public:
	XResample();
	~XResample();

	// ����������������һ�³��˲�����ʽ�����ΪS16 ,���ͷ�para
	virtual bool Open(AVCodecParameters *para,bool isClearPara = false); // ��
	virtual void Close(); // �ر�

	// �����ز������С,���ܳɹ�����ͷ�indata�ռ�
	virtual int Resample(AVFrame *indata, unsigned char *data);
	
	int outFormat = 1; // �����ʽ��AV_SAMPLE_FMT_S16

protected:
	std::mutex m_mutex; // ������
	SwrContext *actx = 0; // ������
};

