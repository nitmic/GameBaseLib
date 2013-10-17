#include "CGUIVideo.h"

using namespace std;
extern "C"
{
    #include <libavcodec/avcodec.h>
}

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

namespace interne
{
    namespace _interface
    {
        CGUIVideo::CGUIVideo( SVideo *d, bool run, bool loop )
        {
            data = d;
            data->Play = data->Replay = writeConsol = data->drawFrame = false;
            #if defined SDL_SUPPORT
                bmp = false;
            #elif defined IRRLICHT_SUPPORT
                VTexture = false;
                VImage = false;
            #endif
            data->Play = run;
            data->Replay = loop;
            writeConsol = data->writeConsol = false;
            lastTime = 0;
        }
        CGUIVideo::~CGUIVideo(){ }
        void CGUIVideo::setConsole(bool b){ data->writeConsol = writeConsol = b; }
        /********** VIDEO VIEW *********/
        void CGUIVideo::setFPS(double fps) { data->videoFPS = fps; }
        //bool CGUIVideo::creatFrameScreenshot(char *name){}
        void CGUIVideo::WindowFPS()
        {
            #if defined SDL_SUPPORT
            static int time, frame,i;
                i+=1;
                if(SDL_GetTicks() - time > 1000)
                {
                    windowFPS = i - frame;
                    frame = i;
                    time = SDL_GetTicks();
                }
            #elif defined IRRLICHT_SUPPORT
                data->windowFPS = driver->getFPS();
            #elif SFML_SUPPORT
            static double time, frame,i;
                i+=1;
                if(timer.GetElapsedTime() - time)
                {
                    windowFPS = i - frame;
                    frame = i;
                    time = timer.GetElapsedTime();
                }
            #endif
        }
        bool CGUIVideo::refreshByTime()
        {
            bool rf;
            if(data->Play
                    #if defined   SDL_SUPPORT
                        && SDL_GetTicks() - lastTime > videoFPS*1000
                    #elif defined SFML_SUPPORT
                        && timer.GetElapsedTime() > videoFPS
                    #elif defined IRRLICHT_SUPPORT
                        && Timer->getRealTime() - lastTime > data->videoFPS*1000
                    #endif
              )
            {
                #if defined SDL_SUPPORT
                    lastTime = SDL_GetTicks();
                #elif defined SFML_SUPPORT
                    timer.Reset();
                #elif defined IRRLICHT_SUPPORT
                    lastTime = Timer->getRealTime();
                #endif
                return refresh();
            }
        }
        bool CGUIVideo::refreshByFPS()
        {
            WindowFPS();
            static int i = (int)(data->getTotalFrame() / data->videoFPS), io = 0;
            io = io+i;
            data->goToFrame((int)io);
			return true;// ‰ž‹}ˆ’u
        }
        bool CGUIVideo::refresh() { return data->makeFrame(); }
        #if defined SDL_SUPPORT
        bool CGUIVideo::draw(SDL_Surface *screen, SDL_Rect rect)
        {
            WindowFPS();
            if(Play && frame)
            {
                if(!bmp)
                    bmp = SDL_CreateYUVOverlay(data->width,data->height, SDL_YV12_OVERLAY,screen);

                SDL_LockYUVOverlay(bmp);
                pict.data[0] = bmp->pixels[0];
                pict.data[1] = bmp->pixels[2];
                pict.data[2] = bmp->pixels[1];

                pict.linesize[0] = bmp->pitches[0];
                pict.linesize[1] = bmp->pitches[2];
                pict.linesize[2] = bmp->pitches[1];

                img_convert(&pict, PIX_FMT_YUVJ420P,(AVPicture *)Frame,videoCodecCtx->pix_fmt,width,height);
                SDL_UnlockYUVOverlay(bmp);
                SDL_DisplayYUVOverlay(bmp, &rect);
            }
        }
        #elif defined IRRLICHT_SUPPORT
        irr::video::IImage* CGUIVideo::getImage() { return VImage; }
        irr::video::ITexture* CGUIVideo::getTexture() { return VTexture; }
        void CGUIVideo::setVideoDriver(irr::video::IVideoDriver*drive,irr::ITimer *time)
        {
            driver = drive; Timer = time;
        }
        irr::video::ITexture* CGUIVideo::draw()
        {
            WindowFPS();//temporaire, wait to dev syncronisation video
            if(data->Play && data->frame && data->drawFrame)
            {
                if(!VImage)   VImage = driver->createImageFromData(irr::video::ECF_A1R5G5B5,irr::core::dimension2d<irr::u32>(data->width,data->height),data->FrameRGB->data[0],true);
                if(!VTexture) VTexture = driver->addTexture("Movie", VImage);

                irr::u32 *tBits = (irr::u32*)VTexture->lock();
                for(irr::u32 j=0; j<data->height; ++j)
                    for(irr::u32 i=0; i<data->width; ++i)
                        tBits[j*data->width+i] = VImage->getPixel(i,j).color;
                VTexture->unlock();

                data->drawFrame = false;
                return VTexture;
            }
        }
        #endif
    }
}