#pragma once
#include <iostream>
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	bool open(const char* url); // ��ý���ļ�������ý�壨rtsp��rtmp��http��
	AVPacket* read(); // ��ȡһ֡AVPacket
	AVCodecParameters* copyVPara(); // ��ȡ��Ƶ����
	AVCodecParameters* copyAPara(); // ��ȡ��Ƶ����
	bool isAudio(AVPacket* pkt); // �Ƿ�Ϊ��Ƶ
	bool seek(double pos); // seekλ�ã�pos 0.0~1.0��
	void clear(); // ����
	void close(); // �ر� 
	AVPacket* readVideo(); // ֻ����Ƶ����Ƶ�����ռ��ͷ�

	// ý����ʱ�������룩
	int totalMs = 0;
	int width = 0;
	int height = 0;
	int sampleRate = 0;
	int channels = 0;

protected:
	std::mutex m_mutex;
	
	AVFormatContext *pFormatCtx = NULL; // ���װ������
	// ����Ƶ��������ȡʱ��������Ƶ
	int nVStreamIndex = 0;
	int nAStreamIndex = 1;
};

