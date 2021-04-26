#include "XResample.h"

XResample::XResample()
{

}

XResample::~XResample()
{

}

// ����������������һ�³��˲�����ʽ�����ΪS16
bool XResample::open(AVCodecParameters *para,bool isClearPara)
{
	if (!para) return false;
	std::unique_lock<std::mutex> guard(m_mutex);
	// ��Ƶ�ز��� �����ĳ�ʼ��
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),	// �����ʽ
		(AVSampleFormat)outFormat,			// ���������ʽ 1 AV_SAMPLE_FMT_S16
		para->sample_rate,					// ���������
		av_get_default_channel_layout(para->channels), // �����ʽ
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	if(isClearPara)
		avcodec_parameters_free(&para);
	int nRet = swr_init(actx);
	if (nRet != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	//unsigned char *pcm = NULL;
	return true;
}

// �����ز������С,���ܳɹ�����ͷ�indata�ռ�
int XResample::resample(AVFrame *indata, unsigned char *d)
{
	if (!indata) return 0;
	if (!d)
	{
		av_frame_free(&indata);
		return 0;
	}
	uint8_t *data[2] = { 0 };
	data[0] = d;
	int nRet = swr_convert(actx,
		data, indata->nb_samples,		// ���
		(const uint8_t**)indata->data, indata->nb_samples	// ����
	);
	if (nRet <= 0)return nRet;
	int outSize = nRet * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
	return outSize;
}

void XResample::close()
{
	std::unique_lock<std::mutex> guard(m_mutex);
	if (actx)
		swr_free(&actx);
}