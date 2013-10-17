#ifndef _GAME_INTRO_H_
#define _GAME_INTRO_H_

#include "CGUIVideo.h"
namespace interne
{
    namespace _interface
    {
        class GameIntro
        {
            public:
                GameIntro(irr::IrrlichtDevice*,char*,bool rsz = false);
                virtual ~GameIntro();
                virtual bool externalEvent(irr::SEvent event);
                bool draw();

                SVideo vi;
                CGUIVideo *intro;
            protected:
                bool resize_for_render;
                irr::IrrlichtDevice *device;
                irr::core::dimension2d<irr::u32> current_render_size;
        };
    }
}
#endif // _GAME_INTRO_H_