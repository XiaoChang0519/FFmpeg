#pragma once
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>

class XAudioPlay
{
public:
	XAudioPlay();
	virtual ~XAudioPlay();

	static XAudioPlay* Get();
	//����Ƶ����
	virtual bool open() = 0;
	virtual void close() = 0;
	//������Ƶ
	virtual bool write(const unsigned char *data, int datasize) = 0;
	virtual int getFree() = 0;

	int m_sampleRate = 44100;
	int m_sampleSize = 16;
	int m_channels = 2;
};

