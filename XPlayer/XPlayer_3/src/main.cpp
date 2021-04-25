#include "XPlayer.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>
#include <future>
#include "XDemux.h"
#include "XDecode.h"
#include "XVideoWidget.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")

using namespace std;

int main(int argc, char* argv[])
{

	QApplication a(argc, argv);

	// ���Ž���
	XPlayer w;
	w.show();

	//=================1�����װ����====================
	XDemux demux;
	const char* url = "dove_640x360.mp4";
	cout << "demux.Open = " << demux.open(url);
	demux.read();
	cout << "demux.Open = " << demux.open(url); // openһ�εĻ����ܾ�֮��ſ�ʼ����
	cout << "CopyVPara = " << demux.copyVPara() << endl;
	cout << "CopyAPara = " << demux.copyAPara() << endl;

	// ��ʼ��openGl����
	w.video->init(demux.getVideoInfo().width, demux.getVideoInfo().height);

	//=================2���������====================
	XDecode vdecode;
	XDecode adecode;
	cout << "vdecode.Open() = " << vdecode.Open(demux.copyVPara()) << endl;
	cout << "adecode.Open() = " << adecode.Open(demux.copyAPara()) << endl;
	// �����첽�߳̽��н��벥�ţ���������GUI
	auto futureLambda = std::async([&]() {
		for (;;)
		{
			AVPacket* pkt = demux.read();
			if (!pkt)
			{
				// �첽�߳��˳��󣬲��������
				demux.close();
				vdecode.Close();
				break;
			}
			if (demux.isAudio(pkt))
			{
				//adecode.Send(pkt);
				//AVFrame *frame = adecode.Recv();
				//cout << "Audio:" << frame << endl;
			}
			else
			{
				vdecode.Send(pkt);
				AVFrame* frame = vdecode.Recv();
				// OpenGl�ӽ����ػ�
				w.video->myRepaint(frame);
				QThread::msleep(40); // 25֡����
			}
		}
	});

	return a.exec();
}
