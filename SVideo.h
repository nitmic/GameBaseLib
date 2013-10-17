#ifndef _SVIDEO_H_
#define _SVIDEO_H_

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

#include <stdint.h>
#if defined WIN32 || defined WIN64 || defined _WIN32_ || defined _WIN64_
    #define WINDOWS
#endif

struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVPicture;
struct AVFrame;

namespace interne
{
    namespace _interface { class CGUIVideo; }
    struct SVideo
    {
        public :
            SVideo();
            SVideo(char*file,bool sound = false);
            ~SVideo();
            bool open( char *file, bool sound);
            void play( bool p = true ), stop(), setLoop(bool b = false);
            bool restart(), isPlay(), isLoop();

            int     getWindowFPS(),         getVideoFPS();
            double  getOriginalFPS();       //! get video file fps info
            int     getFrame(),             getTotalFrame();
            int     getTime(),              getTotalTime();//!
            void    goToFrame(int frm);
            void    goToTime(int sec = 0, int min = 0, int h = 0);
            //! file info ... update ffmpeg for use this
            char   *getAuthor(),      *getAlbum();
            char   *getTitle(),       *getCopyright();
            char   *getComment(),     *getGenre();
            int     getYear(),         getTrack();

            bool makeFrame();
            signed int width, height;
        private :
            friend class _interface::CGUIVideo;
            char *file;

            AVFrame *Frame,*FrameRGB;
            AVFormatContext *FormatCtx;
            AVCodecContext  *videoCodecCtx ,*audioCodecCtx ,*dataCodecCtx;
            AVCodec         *videoCodec    ,*audioCodec    ,*dataCodec;

            uint8_t *buffer;

            double videoFPS;
            int  nFrm, frame, numBytes, windowFPS;
            int  videoStream, audioStream, dataStream;
            bool drawFrame, Sound, Play, Replay, writeConsol;
    };
}
#endif