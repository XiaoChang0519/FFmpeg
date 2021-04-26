#include "XVideoThread.h"

XVideoThread::XVideoThread()
{

}


XVideoThread::~XVideoThread()
{
	// �ȴ��߳��˳�
	isExit = true;
	wait();
}

// �򿪣����ܳɹ��������
bool XVideoThread::open(AVCodecParameters *para, IVideoCall *call,int width,int height)
{
	if (!para)return false;
	m_mutex.lock();

	// ��ʼ����ʾ����
	this->call = call;
	if (call)
	{
		call->init(width, height);
	}

	// �򿪽�����
	if (!decode) decode = new XDecode();
	int nRet = true;
	if (!decode->open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		nRet = false;
	}
	m_mutex.unlock();
	cout << "XAudioThread::Open :" << nRet << endl;
	return nRet;
}

// ��AVPacket���뵽�����еȴ�����ת��
void XVideoThread::push(AVPacket *pkt)
{
	if (!pkt) return;

	// ����
	while (!isExit)
	{
		m_mutex.lock();
		if (m_pktList.size() < maxList)
		{
			m_pktList.push_back(pkt);
			m_mutex.unlock();
			break;
		}
		m_mutex.unlock();
		msleep(1);
	}
}

// �Ӷ����л�ȡAVPacket���н���
void XVideoThread::run()
{
	while (!isExit)
	{
		m_mutex.lock();

		// û������
		if (m_pktList.empty() || !decode)
		{
			m_mutex.unlock();
			msleep(1);
			continue;
		}

		AVPacket *pkt = m_pktList.front();
		m_pktList.pop_front();
		bool nRet = decode->send(pkt);
		if (!nRet)
		{
			m_mutex.unlock();
			msleep(1);
			continue;
		}

		// һ��send ���recv
		while (!isExit)
		{
			AVFrame * frame = decode->recv();
			if (!frame) break;
			// ��ʾ��Ƶ
			if (call)
			{
				call->myRepaint(frame);
			}
		}
		m_mutex.unlock();
	}
}