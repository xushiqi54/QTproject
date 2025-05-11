#include <QDebug>
#include "pullflowthread.h"

QMutex mutex;// 全局互斥锁，用于保护共享变量m_isClosePullFlow

PullFlowThread::PullFlowThread()
{
    connect(this, SIGNAL(startPullFlow(QString)), this, SLOT(onStartPullFlow(QString)));

    m_isClosePullFlow = false;

    //创建并启动工作线程
    m_thread = new QThread;
    moveToThread(m_thread);
    m_thread->start();
}

PullFlowThread::~PullFlowThread()
{
    qDebug() << "~PullFlowThread";
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
    m_thread = nullptr;
}

bool PullFlowThread::isClosePullFlow()
{
    QMutexLocker locker(&mutex);// 加锁保护共享变量
    // 返回值后，locker析构并自动解锁
    return m_isClosePullFlow;
}

void PullFlowThread::toStartPullFlow(const QString &strUrl)
{
    emit startPullFlow(strUrl);// 发射信号触发拉流操作
}

void PullFlowThread::closePullFlow()
{
    qDebug() << "closePullFlow";
    QMutexLocker locker(&mutex);
    m_isClosePullFlow = true;// 设置关闭标志，通知线程退出循环
    // 函数结束时自动解锁
}

void PullFlowThread::onStartPullFlow(const QString &strUrl)
{
    AVFormatContext *formatCtx = nullptr;
    AVCodec *videoCodec = nullptr;
    AVCodecContext *videoCodecCtx = nullptr;
    AVFrame *videoYUVFrame = nullptr, *videoRGBFrame = nullptr;
    AVPacket pkt;
    int videoStream = -1;
    formatCtx = avformat_alloc_context();

    //打开rtmp流

    const char* url;
    QByteArray ba = strUrl.toLatin1();
    url = ba.data();
    qDebug()<<url<<endl;
    int ret = avformat_open_input(&formatCtx, url, nullptr,nullptr);

    qDebug()<<ret<<endl;

    if (ret != 0) {
        char errorBuf[1024];
        av_strerror(ret, errorBuf, sizeof(errorBuf));
        qDebug() << " 无法打开网址： " << strUrl << "  ，错误信息： " << errorBuf;
        if (formatCtx) avformat_free_context(formatCtx);
        return;
    }


    /*if (avformat_open_input(&formatCtx, url, nullptr, nullptr) != 0) {
        qDebug() << " 无法打开网址： " << strUrl;
        return;
    }*/

    qDebug()<<"  open rtmp successful "<<endl;
    //获取流信息
    if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
        qDebug() << " 未获取流信息 ";
        avformat_close_input(&formatCtx);
        return;
    }

    qDebug()<<"  获取流信息成功 "<<endl;


    // 步骤3: 查找视频流索引
    for(int i = 0; i < formatCtx->nb_streams; i++)
    {
       if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
       }
    }
    if (videoStream == -1) {
        qDebug() << " 未检测到视频流 ";
        avformat_close_input(&formatCtx);
        return;
    }
    //qDebug() << " 检测到视频流 ";
    //查找视频解码器
    videoCodecCtx = formatCtx->streams[videoStream]->codec;
    videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
    if (!videoCodec) {
        qDebug() << " 未查找到视频解码器 ";
        avformat_close_input(&formatCtx);
        return;
    }
    //qDebug() << " 查找到视频解码器 ";

    //打开视频解码器
    if (avcodec_open2(videoCodecCtx, videoCodec, nullptr) < 0) {
        qDebug() << " 打开视频解码器失败 ";
        avformat_close_input(&formatCtx);
        return ;
    }
    //qDebug() << " 打开视频解码器  ";
    //视频解码
    videoYUVFrame = av_frame_alloc();
    videoRGBFrame = av_frame_alloc();

    //为videoRGBFrame分配缓存空间
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height);
    uint8_t *rgbBuffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture *) videoRGBFrame, rgbBuffer, AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height);

    // 步骤8: 初始化图像转换上下文（YUV→RGB）
    SwsContext *imgConvertCtx = sws_getContext(videoCodecCtx->width,
                                                                                 videoCodecCtx->height,
                                                                                 videoCodecCtx->pix_fmt,
                                                                                 videoCodecCtx->width,
                                                                                 videoCodecCtx->height,
                                                                                    AV_PIX_FMT_RGB24,
                                                                                    SWS_BICUBIC,
                                                                                 nullptr, nullptr, nullptr);
    // 主循环：拉取并解码视频帧
    while (!m_isClosePullFlow) {
        if (av_read_frame(formatCtx, &pkt) < 0) {
            qDebug() << " 未读取到视频 ";
            break;
        }
        //qDebug() << " 读取到视频 ";
        if (pkt.stream_index == videoStream) {
            //发送视频帧到解码器
            if (avcodec_send_packet(formatCtx->streams[videoStream]->codec, &pkt) != 0) {
                av_packet_unref(&pkt);
                continue;
            }

            //接受后对视频帧进行解码
            if (avcodec_receive_frame(formatCtx->streams[videoStream]->codec, videoYUVFrame) != 0) {
                av_packet_unref(&pkt);
                continue;
            }

            //图像色彩空间转换YUV转为RGB
            sws_scale(imgConvertCtx,
                            static_cast<const uint8_t* const*>(videoYUVFrame->data),
                            videoYUVFrame->linesize,
                            0,
                            videoCodecCtx->height,
                            videoRGBFrame->data,
                            videoRGBFrame->linesize);

            // 步骤12: 将RGB数据转换为QImage并发送信号显示
            QImage image(static_cast<uchar*>(videoRGBFrame->data[0]),
                                    videoCodecCtx->width,
                                    videoCodecCtx->height,
                                    QImage::Format_RGB888);
            //qDebug()<<" 发送image "<<endl;
            emit displayVideo(image);
       }
       av_packet_unref(&pkt);// 释放包资源
       QThread::msleep(10);// 控制帧率，可能需优化为动态延迟
    }

    av_free(rgbBuffer);
    sws_freeContext(imgConvertCtx);
    av_frame_free(&videoYUVFrame);
    av_frame_free(&videoRGBFrame);
    avcodec_close(videoCodecCtx);

    avformat_close_input(&formatCtx);
    avformat_free_context(formatCtx);
}
