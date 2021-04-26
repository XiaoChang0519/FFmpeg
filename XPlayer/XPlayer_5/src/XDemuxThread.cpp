#include "XDemuxThread.h"

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread()
{
	m_isExit = true;
	wait();
}

// �������󲢴�
bool XDemuxThread::open(const char* url, IVideoCall* call)
{
	if (url == 0 || url[0] == '\0')
		return false;

	m_mutex.lock();
	if (!m_demux) m_demux = new XDemux();
	if (!m_vThread) m_vThread = new XVideoThread();
	if (!m_aThread) m_aThread = new XAudioThread();

	// �򿪽��װ
	bool nRet = m_demux->open(url);
	if (!nRet)
	{
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	// ����Ƶ�������ʹ����߳�
	if (!m_vThread->open(m_demux->copyVPara(), call, m_demux->getVideoInfo().width, m_demux->getVideoInfo().height))
	{
		nRet = false;
		cout << "vt->Open failed!" << endl;
	}
	// ����Ƶ�������ʹ����߳�
	if (!m_aThread->open(m_demux->copyAPara(), m_demux->getVideoInfo().sampleRate, m_demux->getVideoInfo().channels))
	{
		nRet = false;
		cout << "at->Open failed!" << endl;
	}
	m_mutex.unlock();
	cout << "XDemuxThread::Open " << nRet << endl;
	return nRet;
}

void XDemuxThread::run()
{
	while (!m_isExit)
	{
		m_mutex.lock();
		if (!m_demux)
		{
			m_mutex.unlock();
			msleep(5);
			continue;
		}
		AVPacket *pkt = m_demux->read();
		if (!pkt)
		{
			m_mutex.unlock();
			msleep(5);
			continue;
		}
		// �ж���������Ƶ
		if (m_demux->isAudio(pkt))
		{
			if(m_aThread)m_aThread->push(pkt);
		}
		else // ��Ƶ
		{
			if (m_vThread)m_vThread->push(pkt);
		}

		m_mutex.unlock();
	}
}

// ���������߳�
void XDemuxThread::start()
{
	m_mutex.lock();
	// ������ǰ�߳�
	QThread::start();
	if (m_vThread)m_vThread->start();
	if (m_aThread)m_aThread->start();
	m_mutex.unlock();
}
