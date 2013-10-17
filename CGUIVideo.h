#ifndef _CGUI_VIDEO_H_
#define _CGUI_VIDEO_H_
#include "SVideo.h"
//!compatible graphique library
//#define SDL_SUPPORT
#define IRRLICHT_SUPPORT
//!futur compatible graphique labrary ?
//////////////#define DIRECTX_SUPPORT
//////////////#define OPENGL_SUPPORT
//////////////#define OGRE_SUPPORT
//!futur compatible Sound labrary ?
//////////////#define FMOD_SUPPORT
//////////////#define IRRKLANG_SUPPORT
//////////////#define SDL_AUDIO_SUPPORT
//////////////#define DIRECT_SOUND_SUPPORT
#if defined SDL_SUPPORT || SDL_AUDIO_SUPPORT
    #include <SDL.h>
    #include <SDL_audio.h>
    #undef FMOD_SUPPORT
    #undef IRRKLANG_SUPPORT
    #define SDL_AUDIO_SUPPORT
#elif defined IRRLICHT_SUPPORT
    #include <irrlicht.h>
#elif defined OPENGL_SUPPORT
    #include <GL/gl.h>
    #include <GL/glu.h>
#elif ( defined DIRECTX_SUPPORT || defined DIRECT_SOUND_SUPPORT ) && defined WINDOWS
    #include <d3d9.h>
    #include <dsound.h>
#endif


/**                                 * * * * * * * * * * * * * * * * * * * * * * * * *                                    **/
/**                                 *                                               *                                    **/
/**                                 *           created by Ovan or Magun            *                                    **/
/**                                 *   compile with : lavformat,lavcodec,lavutil   *                                    **/
/**                                 *        and other library defined last         *                                    **/
/**                                 *                                               *                                    **/
/**                                 *                 conctact me :                 *                                    **/
/**                                 *                www.irrlicht.fr                *                                    **/
/**                                 *            www.immortal-galaxy.com            *                                    **/
/**                                 *                                               *                                    **/
/**                                 * * * * * * * * * * * * * * * * * * * * * * * * *                                    **/

namespace interne
{
    namespace _interface
    {
        class CGUIVideo
        {
            public:
                CGUIVideo(SVideo*, bool run=true, bool loop=false);
                virtual ~CGUIVideo();//free memory class
                virtual bool refresh(),// no limite refresh video
                             refreshByTime(),//limite refresh by time
                             refreshByFPS();//sync video fps and window fps for slow computer or slow render window ... no finished
                //virtual bool creatFrameScreenshot(char *name);
                virtual void setFPS( double fps );//set the video decoding frame rate, if 0 fps = video.fps
                virtual void setConsole( bool b = false );//!sdl note suported
                #if defined SDL_SUPPORT
                    virtual bool draw(SDL_Surface *screen, SDL_Rect rect);
                #elif defined IRRLICHT_SUPPORT
                    virtual irr::video::ITexture *draw();
                    virtual void setVideoDriver(irr::video::IVideoDriver*drive, irr::ITimer *time);
                    virtual irr::video::IImage   *getImage();
                    virtual irr::video::ITexture *getTexture();
                #endif
                SVideo *getVideoData() { return  data; }
            protected:
                SVideo *data;
                bool writeConsol;
            private:
                int lastTime;
                #if defined SDL_SUPPORT
                    SDL_Overlay *bmp;
                #elif defined OGRE_SUPPORT
                #elif defined IRRLICHT_SUPPORT
                    irr::ITimer *Timer;
                    irr::video::IImage   *VImage;
                    irr::video::ITexture *VTexture;
                    irr::video::IVideoDriver*driver;
                #endif
                #if defined SDL_AUDIO_SUPPORT
                  SDL_AudioSpec   wanted_spec, spec;
                #endif
                void WindowFPS();
        };
    }
}
#endif // CGUIVideo_H