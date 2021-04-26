#include "XDecode.h"

extern "C"
{
#include<libavcodec/avcodec.h>
}

void XFreePacket(AVPacket **pkt)
{
	if (!pkt || !(*pkt))return;
	av_packet_free(pkt);
}

void XFreeFrame(AVFrame **frame)
{
	if (!frame || !(*frame))return;
	av_frame_free(frame);
}

XDecode::XDecode()
{
}


XDecode::~XDecode()
{
}

// �򿪽�����
bool XDecode::Open(AVCodecParameters *para)
{
	if (!para) return false;
	Close();

	// ���ݴ����para->codec_id�ҵ�������
	AVCodec *vcodec = avcodec_find_decoder(para->codec_id);
	if (!vcodec)
	{
		avcodec_parameters_free(&para);
		cout << "can't find the codec id " << para->codec_id << endl;
		return false;
	}
	cout << "find the AVCodec " << para->codec_id << endl;

	m_mutex.lock();
	// ����������������
	m_VCodecCtx = avcodec_alloc_context3(vcodec);
	// ���ý����������Ĳ���
	avcodec_parameters_to_context(m_VCodecCtx, para);
	// ��ձ����������������ڴ�й©������Ҫ��
	avcodec_parameters_free(&para);
	// ���߳̽���
	m_VCodecCtx->thread_count = 8;

	// �򿪽�����������
	int re = avcodec_open2(m_VCodecCtx, 0, 0);
	if (re != 0)
	{
		avcodec_free_context(&m_VCodecCtx);
		m_mutex.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}
	m_mutex.unlock();
	cout << " avcodec_open2 success!" << endl;

	return true;
}

// ���͵������̣߳����ܳɹ�����ͷ�pkt�ռ� �����ý�����ݣ�
bool XDecode::Send(AVPacket *pkt)
{
	// �ݴ���
	if (!pkt || pkt->size <= 0 || !pkt->data)return false;
	m_mutex.lock();
	if (!m_VCodecCtx)
	{
		m_mutex.unlock();
		return false;
	}
	int re = avcodec_send_packet(m_VCodecCtx, pkt);
	m_mutex.unlock();

	// ���۳ɹ���񣬶����AVPacket�������ڴ�й©������Ҫ��
	av_packet_free(&pkt);
	if (re != 0)return false;
	return true;
}

// ��ȡ�������ݣ�һ��send������Ҫ���Recv����ȡ�����е�����Send NULL��Recv���
// ÿ�θ���һ�ݣ��ɵ������ͷ� av_frame_free
AVFrame* XDecode::Recv()
{
	m_mutex.lock();
	if (!m_VCodecCtx)
	{
		m_mutex.unlock();
		return NULL;
	}
	AVFrame *frame = av_frame_alloc();
	int re = avcodec_receive_frame(m_VCodecCtx, frame);
	m_mutex.unlock();
	if (re != 0)
	{
		av_frame_free(&frame);
		return NULL;
	}
	//cout << "["<<frame->linesize[0] << "] " << flush;
	pts = frame->pts;
	return frame;
}

// ����
void XDecode::Clear()
{
	m_mutex.lock();
	// ������뻺��
	if (m_VCodecCtx)
		avcodec_flush_buffers(m_VCodecCtx);

	m_mutex.unlock();
}

// �ر�
void XDecode::Close()
{
	m_mutex.lock();
	if (m_VCodecCtx)
	{
		avcodec_close(m_VCodecCtx);
		avcodec_free_context(&m_VCodecCtx);
	}
	pts = 0;
	m_mutex.unlock();
}