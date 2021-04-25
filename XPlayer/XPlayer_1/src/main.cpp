#include <iostream>
#include <fstream>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
// ��ͳ��װ������Ҫ
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")

using namespace std;

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

int main(int argc, char* argv[])
{
	// ��rgb�ļ�
	FILE* outFileRgb = fopen("../bin/win64/dove_640x360.rgb", "wb");
	if (outFileRgb == NULL) {
		cout << "file not exist!" << endl;
		return false;
	}
	// ��pcm�ļ�
	FILE* outFilePcm = fopen("../bin/win64/dove.pcm", "wb");	
	if (outFilePcm == NULL) {
		cout << "file not exist!" << endl;
		return false;
	}

	// ��ʼ������� �����Դ�rtsp rtmp http Э�����ý����Ƶ��
	avformat_network_init();

	//===================1������Ƶ�ļ�===================
	const char* path = "dove_640x360.mp4";
	// ��������
	AVDictionary* opts = NULL;
	// ����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	// ������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);

	// ���װ������
	AVFormatContext* pFormatCtx = NULL;
	int nRet = avformat_open_input(
		&pFormatCtx,
		path,
		0,  // 0��ʾ�Զ�ѡ������
		&opts // �������ã�����rtsp����ʱʱ��
	);
	if (nRet != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf) - 1);
		cout << "open " << path << " failed! :" << buf << endl;
		return -1;
	}
	cout << "open " << path << " success! " << endl;

	// ̽���ȡ����Ϣ
	nRet = avformat_find_stream_info(pFormatCtx, 0);

	// ��ȡý����ʱ������λΪ����
	int totalMs = static_cast<int>(pFormatCtx->duration / (AV_TIME_BASE / 1000));
	cout << "totalMs = " << totalMs << endl;
	// ��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(pFormatCtx, 0, path, 0);

	//===================2����ȡ����Ƶ������===================
	int nVStreamIndex = -1; // ��Ƶ����������ȡʱ������������Ƶ��
	int nAStreamIndex = -1; // ��Ƶ������
	// ��ȡ��Ƶ���������°汾������ʹ��av_find_best_stream������	
	nVStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (nVStreamIndex == -1) {
		cout << "find videoStream failed!" << endl;
		return -1;
	}
	// ��ӡ��Ƶ��Ϣ�����pStreamֻ��ָ��pFormatCtx�ĳ�Ա��δ�����ڴ棬Ϊջָ�������ͷţ�����ͬ��
	AVStream* pVStream = pFormatCtx->streams[nVStreamIndex];
	cout << "=======================================================" << endl;
	cout << "VideoInfo: " << nVStreamIndex << endl;
	cout << "codec_id = " << pVStream->codecpar->codec_id << endl;
	cout << "format = " << pVStream->codecpar->format << endl;
	cout << "width=" << pVStream->codecpar->width << endl;
	cout << "height=" << pVStream->codecpar->height << endl;
	// ֡�� fps ����ת��
	cout << "video fps = " << r2d(pVStream->avg_frame_rate) << endl;
	// ֡�� fps ����ת��
	cout << "video fps = " << r2d(pFormatCtx->streams[nVStreamIndex]->avg_frame_rate) << endl;

	// ��ȡ��Ƶ������
	nAStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	if (nVStreamIndex == -1) {
		cout << "find audioStream failed!" << endl;
		return -1;
	}
	// ��ӡ��Ƶ��Ϣ
	AVStream* pAStream = pFormatCtx->streams[nAStreamIndex];
	cout << "=======================================================" << endl;
	cout << "AudioInfo: " << nAStreamIndex << endl;
	cout << "codec_id = " << pAStream->codecpar->codec_id << endl;
	cout << "format = " << pAStream->codecpar->format << endl;
	cout << "sample_rate = " << pAStream->codecpar->sample_rate << endl;
	// AVSampleFormat;
	cout << "channels = " << pAStream->codecpar->channels << endl;
	// һ֡���ݣ��� ��ͨ��������
	cout << "frame_size = " << pAStream->codecpar->frame_size << endl;

	//===================3������Ƶ������===================
	// ����codec_id�ҵ���Ƶ������
	AVCodec* pVCodec = avcodec_find_decoder(pVStream->codecpar->codec_id);
	if (!pVCodec)
	{
		cout << "can't find the codec id " << pVStream->codecpar->codec_id;
		return -1;
	}
	cout << "find the AVCodec " << pVStream->codecpar->codec_id << endl;

	// ������Ƶ������������
	AVCodecContext* pVCodecCtx = avcodec_alloc_context3(pVCodec);
	// ������Ƶ�����������Ĳ���
	avcodec_parameters_to_context(pVCodecCtx, pVStream->codecpar);
	// ���߳���Ƶ����
	pVCodecCtx->thread_count = 8;

	// ����Ƶ������������
	nRet = avcodec_open2(pVCodecCtx, 0, 0);
	if (nRet != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return -1;
	}
	cout << "video avcodec_open2 success!" << endl;

	//===================3������Ƶ������===================
	// �ҵ���Ƶ������
	AVCodec* pACodec = avcodec_find_decoder(pFormatCtx->streams[nAStreamIndex]->codecpar->codec_id);
	if (!pACodec)
	{
		cout << "can't find the codec id " << pFormatCtx->streams[nAStreamIndex]->codecpar->codec_id;
		return -1;
	}
	cout << "find the AVCodec " << pFormatCtx->streams[nAStreamIndex]->codecpar->codec_id << endl;

	// ������Ƶ������������
	AVCodecContext* pACodecCtx = avcodec_alloc_context3(pACodec);
	// /������Ƶ�����������Ĳ���
	avcodec_parameters_to_context(pACodecCtx, pFormatCtx->streams[nAStreamIndex]->codecpar);
	// ���߳���Ƶ����
	pACodecCtx->thread_count = 8;

	// ����Ƶ������������
	nRet = avcodec_open2(pACodecCtx, 0, 0);
	if (nRet != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return -1;
	}
	cout << "audio avcodec_open2 success!" << endl;

	//===================4��ѭ������ǰ��ʼ����������===================
	// malloc AVPacket����ʼ��
	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	// ���ظ�ʽ�ͳߴ�ת��������
	SwsContext* vSwsCtx = NULL;
	unsigned char* rgb = NULL;

	// ��Ƶ�ز��� �����ĳ�ʼ��
	SwrContext* actx = swr_alloc();
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),	// �����ʽ
		AV_SAMPLE_FMT_S16,					// ���������ʽ
		pACodecCtx->sample_rate,			// ���������
		av_get_default_channel_layout(pACodecCtx->channels), // �����ʽ
		pACodecCtx->sample_fmt,
		pACodecCtx->sample_rate,
		0, 0
	);
	// ��ʼ����Ƶ��������������
	nRet = swr_init(actx);
	if (nRet != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return -1;
	}
	unsigned char* pcm = NULL;
	// ��������С = ������(44100HZ) * ��������(16λ = 2�ֽ�)
	int MAX_AUDIO_SIZE = 44100 * 2;
	uint8_t* out_audio = (uint8_t*)av_malloc(MAX_AUDIO_SIZE);;
	// ��ȡ�������������
	int out_nb_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

	//===================5����ʼѭ������===================
	while(1)
	{
		int nRet = av_read_frame(pFormatCtx, pkt);
		if (nRet != 0)
		{
#if 0
			// ѭ��"����"
			cout << "==============================end==============================" << endl;
			int ms = 3000; // ����λ�� ����ʱ�������������ת��
			long long pos = (double)ms / (double)1000 * r2d(ic->streams[pkt->stream_index]->time_base);
			av_seek_frame(ic, nVStreamIndex, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
			continue;
#else
			// "����"��һ��֮���˳�
			break;
#endif
		}
		cout << "pkt->size = " << pkt->size << endl;
		// ��ʾ��ʱ��
		cout << "pkt->pts = " << pkt->pts << endl;
		// ת��Ϊ���룬������ͬ��
		cout << "pkt->pts ms = " << pkt->pts * (r2d(pFormatCtx->streams[pkt->stream_index]->time_base) * 1000) << endl;
		// ����ʱ��
		cout << "pkt->dts = " << pkt->dts << endl;

		AVCodecContext* pCodecCtx = 0;
		if (pkt->stream_index == nVStreamIndex)
		{
			cout << "ͼ��" << endl;
			pCodecCtx = pVCodecCtx;
		}
		if (pkt->stream_index == nAStreamIndex)
		{
			cout << "��Ƶ" << endl;
			pCodecCtx = pACodecCtx;
		}

		// ������Ƶ
		// ����packet�������߳�  send��NULL����ö��receiveȡ�����л���֡
		nRet = avcodec_send_packet(pCodecCtx, pkt);
		// �ͷţ����ü���-1 Ϊ0�ͷſռ�
		av_packet_unref(pkt);

		if (nRet != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(nRet, buf, sizeof(buf) - 1);
			cout << "avcodec_send_packet  failed! :" << buf << endl;
			continue;
		}

		for (;;)
		{
			// ���߳��л�ȡ����ӿ�,һ��send���ܶ�Ӧ���receive
			nRet = avcodec_receive_frame(pCodecCtx, frame);
			if (nRet != 0) break;
			cout << "recv frame " << frame->format << " " << frame->linesize[0] << endl;

			// ��Ƶ
			if (pCodecCtx == pVCodecCtx)
			{
				vSwsCtx = sws_getCachedContext(
					vSwsCtx,	// ��NULL���´���
					frame->width, frame->height,		// ����Ŀ��
					(AVPixelFormat)frame->format,	// �����ʽ YUV420p
					frame->width, frame->height,	// ����Ŀ��
					AV_PIX_FMT_RGBA,				// �����ʽRGBA
					SWS_BILINEAR,					// �ߴ�仯���㷨
					0, 0, 0);
				if (vSwsCtx)
				{
					// RGB�����������ڴ棬ֻ��һ�η���
					//����ȻҲ���Դ���pFrameRGB����avpicture_fill��ʼ��pFrameRGB��ʵ�֣�
					if (!rgb) rgb = new unsigned char[static_cast<long long>(frame->width) * frame->height * 4];
					uint8_t* data[2] = { 0 };
					data[0] = rgb;
					int lines[2] = { 0 };
					lines[0] = frame->width * 4;
					// ����ת����YUVת����RGB
					nRet = sws_scale(vSwsCtx,
						frame->data,		// ��������
						frame->linesize,	// �����д�С
						0,
						frame->height,		// ����߶�
						data,				// ������ݺʹ�С
						lines
					);
					cout << "sws_scale = " << nRet << endl;

					// �������Զ����Ƶ���ʽд���ļ���
					fwrite(data[0], static_cast<long long>(frame->width)* frame->height * 4, 1, outFileRgb);
				}
			}
			else // ��Ƶ
			{
				// ������Ƶ����������
				uint8_t* data[2] = { 0 };
				if (!pcm) pcm = new uint8_t[static_cast<long long>(frame->nb_samples) * 2 * 2];
				data[0] = pcm;
				// ����ת����ת����PCM
				nRet = swr_convert(actx,
					data, frame->nb_samples,		// ���
					(const uint8_t**)frame->data, frame->nb_samples	// ����
				);
				cout << "swr_convert = " << nRet << endl;

				// ��ȡ������ʵ�ʴ洢��С
				int out_buffer_size = av_samples_get_buffer_size(NULL, out_nb_channels, frame->nb_samples,
					AV_SAMPLE_FMT_S16, 1);
				// �������Զ����Ƶ���ʽд���ļ���
				fwrite(data[0], 1, out_buffer_size, outFilePcm);
			}
		}
	}

	//===================6���ڴ��ͷ�===================
	fclose(outFileRgb);
	fclose(outFilePcm);

	// �ͷŽ��װ�����ģ����Ұ�ic��0
	avformat_close_input(&pFormatCtx);
	// �ͷ���Ƶ������������
	avcodec_free_context(&pVCodecCtx);
	// �ͷ���Ƶ������������
	avcodec_free_context(&pVCodecCtx);
	// �ͷ�frame
	av_frame_free(&frame);
    // �ͷ�pkt
	av_packet_free(&pkt);

	return 0;
}
