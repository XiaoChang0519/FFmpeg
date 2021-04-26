#pragma once

class XAudioPlay
{
public:
	XAudioPlay();
	virtual ~XAudioPlay();
	
	virtual bool Open() = 0; // ����Ƶ����
	virtual void Close() = 0;
	virtual void Clear() = 0;
	virtual long long GetNoPlayMs() = 0; // ���ػ����л�û�в��ŵ�ʱ�䣨���룩
	virtual bool Write(const unsigned char *data, int datasize) = 0; // ������Ƶ
	virtual int GetFree() = 0;
	virtual void SetPause(bool isPause) = 0;
	static XAudioPlay* Get();

	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;
};

