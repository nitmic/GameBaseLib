#include "GameMovie.h"

using namespace irr;
using namespace video;
using namespace core;

namespace interne
{
    namespace _interface
    {
        GameIntro::GameIntro(irr::IrrlichtDevice*dev,char*video_file,bool rsz) : device(dev)
        {
            current_render_size = device->getVideoDriver()->getScreenSize();
            resize_for_render = rsz;
            vi.open(video_file, true); vi.play(true);
            intro = new _interface::CGUIVideo(&vi);
            intro->setVideoDriver(device->getVideoDriver(),device->getTimer());
            device->getVideoDriver()->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
            if(resize_for_render) device->getVideoDriver()->OnResize(dimension2d<u32>(vi.width,vi.height));
        }
        GameIntro::~GameIntro()
        {
            vi.stop();
            //delete vi;
            delete intro;
            if(resize_for_render) device->getVideoDriver()->OnResize(current_render_size);
            device->getVideoDriver()->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);
        }
        bool GameIntro::externalEvent(SEvent event)
        {
            if(event.EventType == EET_MOUSE_INPUT_EVENT && event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP && intro)
            { return true; }
            if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown == false && intro)
            {
                switch(event.KeyInput.Key)
                {
                    case KEY_ESCAPE: case KEY_DELETE: case KEY_RETURN: case KEY_SPACE:
                    return true; break; default: break;
                }
            }
            return false;
        }
        bool GameIntro::draw()
        {
            if(intro && intro->refreshByTime() && device->run())
            {
                device->getVideoDriver()->draw2DImage(intro->draw(), position2d<s32>(0,0));
                return false;
            }
            return true;
        }
    }
}