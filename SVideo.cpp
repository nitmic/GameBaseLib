#include "SVideo.h"
#define IRRLICHT_SUPPORT

/** * * * * * * * * * * * * * * * * * * * * * * * * *
    *                                               *
    *           created by Ovan or Magun            *
    *   compile with : lavformat,lavcodec,lavutil   *
    *        and other library defined last         *
    *                                               *
    *                 conctact me :                 *
    *                www.irrlicht.fr                *
    *            www.immortal-galaxy.com            *
    *                                               *
    * * * * * * * * * * * * * * * * * * * * * * * * * **/

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_MAJOR > 51
    int img_convert(AVPicture* dst, PixelFormat dst_pix_fmt, AVPicture* src, PixelFormat pix_fmt, int width, int height)
    {
       int av_log = av_log_get_level();
       av_log_set_level(AV_LOG_QUIET);
       SwsContext *img_convert_ctx = sws_getContext(width, height, pix_fmt, width, height, dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
       int result = sws_scale(img_convert_ctx, src->data, src->linesize, 0, height, dst->data, dst->linesize);
       sws_freeContext(img_convert_ctx);
       av_log_set_level(av_log);
       return result;
    }
#endif
namespace interne
{
    SVideo::SVideo()
    {
        av_register_all();
    }
    SVideo::SVideo(char*file,bool sound)
    {
        av_register_all();
        open(file,sound);
    }
    SVideo::~SVideo()
    {
        if(buffer        != NULL) av_free(buffer);
        if(FrameRGB      != NULL) av_free(FrameRGB);
        if(Frame         != NULL) av_free(Frame);
        if(videoCodecCtx != NULL) avcodec_close(videoCodecCtx);
        if(FormatCtx     != NULL) av_close_input_file(FormatCtx);
    }
    bool SVideo::open(char *f,bool sound)
    {
        file = f;
        Sound = sound;
		FormatCtx = NULL;
		
		if(avformat_open_input(&FormatCtx, file, NULL, NULL)!=0)
        {
          Play = false;
          return true;
        }
        if(av_find_stream_info(FormatCtx)<0)
        {
          Play = false;
          return true;
        }
		av_dump_format(FormatCtx, 0, file, 0);

        videoStream=-1;
        audioStream=-1;
        dataStream=-1;
        for(nFrm=0; nFrm<FormatCtx->nb_streams; nFrm++)
        {
          if(FormatCtx->streams[nFrm]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
          {
             videoStream = nFrm;
          }
		  if(FormatCtx->streams[nFrm]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
          {
             audioStream = nFrm;
          }
		  if(FormatCtx->streams[nFrm]->codec->codec_type == AVMEDIA_TYPE_DATA)
          {
             dataStream = nFrm;
          }
        }
        if(dataStream > -1)
            dataCodecCtx = FormatCtx->streams[dataStream]->codec;
        if(audioStream > -1 && sound)
        {
            audioCodecCtx = FormatCtx->streams[audioStream]->codec;
            audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);
            avcodec_open2(audioCodecCtx, audioCodec, NULL);
        }
        if(videoStream > -1)
        {
            videoCodecCtx = FormatCtx->streams[videoStream]->codec;
            videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
            avcodec_open2(videoCodecCtx, videoCodec, NULL);
            videoFPS = (double)FormatCtx->streams[videoStream]->r_frame_rate.den / FormatCtx->streams[videoStream]->r_frame_rate.num;
            Frame = avcodec_alloc_frame();
            FrameRGB = avcodec_alloc_frame();
        }
        #if defined IRRLICHT_SUPPORT
            numBytes = avpicture_get_size(PIX_FMT_RGB555, videoCodecCtx->width,videoCodecCtx->height);
            buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
            avpicture_fill((AVPicture *)FrameRGB, buffer, PIX_FMT_RGB555, videoCodecCtx->width, videoCodecCtx->height);
        #else
            numBytes = avpicture_get_size(PIX_FMT_RGB32, videoCodecCtx->width,videoCodecCtx->height);
            buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
            avpicture_fill((AVPicture *)FrameRGB, buffer, PIX_FMT_RGB32, videoCodecCtx->width, videoCodecCtx->height);
        #endif
        nFrm = 0;
        width = videoCodecCtx->width;
        height = videoCodecCtx->height;
        return true;
    }
    bool SVideo::makeFrame()
    {
        if(Play)
        {
            AVPacket packet;
            if(av_read_frame(FormatCtx, &packet) >= 0)
            {
                if(packet.stream_index == videoStream)
                {
                    avcodec_decode_video2(videoCodecCtx, Frame, &frame, &packet);
                    if(frame)
                    {
                        #if defined SDL_SUPPORT
                        #elif defined IRRLICHT_SUPPORT
                            img_convert((AVPicture *)FrameRGB, PIX_FMT_RGB555, (AVPicture*)Frame, videoCodecCtx->pix_fmt,width,height);
                        #else
                            img_convert((AVPicture *)FrameRGB, PIX_FMT_RGB32 , (AVPicture*)Frame, videoCodecCtx->pix_fmt,width,height);
                        #endif
                        nFrm++;
                        av_free_packet(&packet);
                    }
                }
                else if(packet.stream_index == audioStream);
                else av_free_packet(&packet);
            }
            else
            {
                if(Replay == true) restart();
                else
                {
                    if(writeConsol) printf("\n> END <\n\n");
                    play(false);
                    return false;
                }
            }
            drawFrame = true;
        }
        return true;
    }
    void SVideo::play(bool b)   { Play = b; }
    void SVideo::stop()         { restart(); Play = false; }
    void SVideo::setLoop(bool b){ Replay = b; }
    /******************************/
    bool SVideo::isPlay(){ return Play;   }
    bool SVideo::isLoop(){ return Replay; }
    void SVideo::goToTime( int sec, int min, int h ) { sec += ((h*60)+min)*60; goToFrame((int)(sec/videoFPS)); }
    void SVideo::goToFrame(int frm)
    {
        if( frm > nFrm) av_seek_frame(FormatCtx, videoStream, frm, AVSEEK_FLAG_BACKWARD);
        else
        {
            restart();
            av_seek_frame(FormatCtx, videoStream, frm, AVSEEK_FLAG_BACKWARD);
        }
        nFrm = frm;
    }
    bool SVideo::restart()
    {
        if (FormatCtx)
        {
            if (avformat_open_input(&FormatCtx, file, NULL, NULL) != 0)
                return false;
            nFrm = 0;
            if(writeConsol) printf("\n> RESTART <\n\n");
            return true;
        }
    }
    /********** FILE INFO *********/
	namespace{
		AVDictionaryEntry *getDict(char* key, AVDictionary * dict){
			return av_dict_get(dict, key, NULL, AV_DICT_IGNORE_SUFFIX);
		}
	};

	char *SVideo::getAuthor()   { return getDict("artist", FormatCtx->metadata)->value; }
    char *SVideo::getAlbum()    { return getDict("album", FormatCtx->metadata)->value; }
    char *SVideo::getTitle()    { return getDict("title", FormatCtx->metadata)->value; }
    char *SVideo::getCopyright(){ return getDict("copyright", FormatCtx->metadata)->value; }
    char *SVideo::getComment()  { return getDict("comment", FormatCtx->metadata)->value; }
    char *SVideo::getGenre(){ return getDict("genre", FormatCtx->metadata)->value; }
	int SVideo::getYear()   { return 0;}//FormatCtx->year; }
    int SVideo::getTrack()  { return *getDict("track", FormatCtx->metadata)->value - '0'; }
    int SVideo::getVideoFPS() { return (int)videoFPS; }
    double SVideo::getOriginalFPS(){ return (double)FormatCtx->streams[videoStream]->r_frame_rate.den / FormatCtx->streams[videoStream]->r_frame_rate.num;  }
    int SVideo::getFrame()      { return nFrm; }
    int SVideo::getTime()       { return (int)(nFrm/videoFPS)/60; }
    int SVideo::getTotalFrame() { return FormatCtx->streams[videoStream]->nb_frames; }
    int SVideo::getTotalTime()  { return (int)(FormatCtx->streams[videoStream]->nb_frames/videoFPS); }
    int SVideo::getWindowFPS()  { return windowFPS; }
    /******************************/
}