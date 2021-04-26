#include "XAudioThread.h"

XAudioThread::XAudioThread()
{

}

XAudioThread::~XAudioThread()
{
	// �ȴ��߳��˳�
	m_isExit = true;
	wait();
}

// ��
bool XAudioThread::open(AVCodecParameters* para, int sampleRate, int channels)
{
	if (!para)return false;
	m_mutex.lock();
	if (!m_decode) m_decode = new XDecode();
	if (!m_resample) m_resample = new XResample();
	if (!m_audioPlay) m_audioPlay = XAudioPlay::Get();
	bool nRet = true;
	if (!m_resample->open(para, false))
	{
		cout << "XResample open failed!" << endl;
		nRet = false;
	}
	m_audioPlay->m_sampleRate = sampleRate;
	m_audioPlay->m_channels = channels;
	if (!m_audioPlay->open())
	{
		nRet = false;
		cout << "XAudioPlay open failed!" << endl;
	}
	if (!m_decode->open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		nRet = false;
	}
	m_mutex.unlock();
	cout << "XAudioThread::Open :" << nRet << endl;
	return nRet;
}

// ��AVPacket���뵽�����еȴ�����ת��
void XAudioThread::push(AVPacket *pkt)
{
	if (!pkt) return; 
	// ����
	while (!m_isExit)
	{
		m_mutex.lock();
		if (m_packs.size() < m_maxList)
		{
			m_packs.push_back(pkt);
			m_mutex.unlock();
			break;
		}
		m_mutex.unlock();
		msleep(1);
	}
}

// �Ӷ����л�ȡAVPacket���н���
void XAudioThread::run()
{
	unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
	while (!m_isExit)
	{
		m_mutex.lock();

		//û������
		if (m_packs.empty() || !m_decode || !m_resample || !m_audioPlay)
		{
			m_mutex.unlock();
			msleep(1);
			continue;
		}

		AVPacket *pkt = m_packs.front();
		m_packs.pop_front();
		bool nRet = m_decode->send(pkt);
		if (!nRet)
		{
			m_mutex.unlock();
			msleep(1);
			continue;
		}
		//һ��send ���recv
		while (!m_isExit)
		{
			AVFrame * frame = m_decode->recv();
			if (!frame) break;
			//�ز��� 
			int size = m_resample->resample(frame, pcm);
			//������Ƶ
			while (!m_isExit)
			{
				if (size <= 0) break;
				//����δ���꣬�ռ䲻��
				if (m_audioPlay->getFree() < size)
				{
					msleep(1);
					continue;
				}
				m_audioPlay->write(pcm, size);
				break;
			}
		}
		m_mutex.unlock();
	}
	delete pcm;
}
