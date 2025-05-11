#ifndef PULLFLOWTHREAD_H
#define PULLFLOWTHREAD_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <QMutex>


extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/time.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/error.h>  // 添加头文件
}

#define MAX_AUDIO_FRAME_SIZE  192000

extern QMutex mutex;

class PullFlowThread : public QObject
{
    Q_OBJECT

public:
    PullFlowThread();
    ~PullFlowThread();

    bool isClosePullFlow();

    void toStartPullFlow(const QString &strUrl);
    void closePullFlow();

signals:
    void startPullFlow(const QString &strUrl);
    void displayVideo(const QImage &image);

public slots:
    void onStartPullFlow(const QString &strUrl);

private:
    bool m_isClosePullFlow;
    QThread *m_thread;
};

#endif // PULLFLOWTHREAD_H
