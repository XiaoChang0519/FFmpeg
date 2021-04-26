#include "XPlayer.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>
#include <future>
#include "XDemux.h"
#include "XDecode.h"
#include "XVideoWidget.h"
#include "XResample.h"
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"

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

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	// ��ʼ����ʾ
	XPlayer w;
	w.show();

	// XDemuxThread�̲߳���
	XDemuxThread dt;
	dt.open("dove_640x360.mp4", w.video);
	dt.start();

	return a.exec();
}
