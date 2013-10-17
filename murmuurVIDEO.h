#ifndef __MURMUUR_VIDEO_H__ /////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Original code sources from juliusctw, Hansel, stepan1117
// Heavily Modified/Merged by theSpecial1
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// defines //////////////////////////////////////////////////////////////////////////////////////////////////
#define __MURMUUR_VIDEO_H__
#define NUM_BUFFERS 3
#define BUFFER_SIZE 19200
#define MAX_AUDIO_FRAME_SIZE 192000
//#define SOUND_OPENAL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <FPSModerator.h>
namespace IrrAdapter{
	class Image;
};


// includes /////////////////////////////////////////////////////////////////////////////////////////////////
#include <irrlicht.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
extern "C" {
   #include <string.h>
   #include <libavcodec/avcodec.h>
   #include <libavformat/avformat.h>
   #include <libswscale/swscale.h>
#ifdef SOUND_OPENAL
   #include <signal.h>
   #include <AL/al.h>
   #include <AL/alc.h>
   #include <AL/alut.h>
#endif
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// namespaces ///////////////////////////////////////////////////////////////////////////////////////////////
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// structures ///////////////////////////////////////////////////////////////////////////////////////////////
typedef struct MyFile *FilePtr;
typedef struct MyStream *StreamPtr;
struct MyStream {
   AVCodecContext *CodecCtx;
   int StreamIdx;

   char *Data;
   size_t DataSize;
   size_t DataSizeMax;
   char *DecodedData;
   size_t DecodedDataSize;

   FilePtr parent;
};

struct MyFile {
   AVFormatContext *FmtCtx;
   StreamPtr *Streams;
   size_t StreamsSize;
}; //////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ePlaystate { Closed, Playing, Paused, Stopped };

// main class definition ////////////////////////////////////////////////////////////////////////////////////
class murmuurVIDEO {
private: ////////////////////////////////////////////////////////////////////////////////////////////////////    
	TUL::FPSModerator fps;
   //irr::ITimer *_itTimer;
    irr::video::IVideoDriver *_vdVideoDriver;
    irr::video::IImage *_imCurrentImage;
    irr::video::ITexture *_txCurrentTexture;
	
   FilePtr _fpFile;
    StreamPtr _spStreamA, _spStreamV;
   bool _bHasAudio, _bHasVideo;
   int _iDesiredH;
   int _iDesiredW;
   
   // unsigned long _lLastTime;   
   std::vector<AVFrame> _frFrame_Buffer;    
   bool _bFrameDisplayed;
   AVFrame *_frFrame;
    AVFrame *_frFrameRGB;
    int _iNumBytes;
    uint8_t *_iBuffer;
   s32* _p;
    s32* _pimage;
   
#ifdef SOUND_OPENAL
    ALuint _aiBuffers[NUM_BUFFERS];
    ALuint _aiSource;
    ALint _aiState; 
    ALbyte *_abData;                   
   ALenum _aeOldFormat;
   ALenum _aeFormat;
#endif
    int _iBuffCount; 
    int _iOld_rate;        
    int _iChannels;
    int _iBits;
    int _iRate;
    int _iBasetime;
   
   bool _initAV(void);
   FilePtr _openAVFile(const char *fname);
   void _closeAVFile(FilePtr file);   
   bool _DumpFrame(AVFrame *pFrame, int width, int height, bool needResize);
   StreamPtr _getAVAudioStream(FilePtr file, int streamnum);
   StreamPtr _getAVVideoStream(FilePtr file, int streamnum);
   int _getAVAudioInfo(StreamPtr stream, int *rate, int *channels, int *bits);
   bool _getNextPacket(FilePtr file, int streamidx);
   int _getAVAudioData(StreamPtr stream, void *data, int length);
   AVFrame *_getNextFrame(void);
public: /////////////////////////////////////////////////////////////////////////////////////////////////////
   double dSecondsPerFrame;
   float fFramerate;
   float fDuration;
   int iActualFrame;
   int iNum_frames;
   ePlaystate psVideostate;
   IMeshSceneNode *mnOutputMesh;
   bool bVideoLoaded;

   murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, int desiredH);
   murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, int desiredH, IMeshSceneNode *outputMesh);   
   bool open(core::stringc sFileName);
   bool refresh(void);
   IrrAdapter::Image decode();

   void drawVideoTexture(void);
   void changeResolution(int w, int h);
   void close(void);
    ~murmuurVIDEO();
}; //////////////////////////////////////////////////////////////////////////////////////////////////////////

void SafeReleaseMurmuur(murmuurVIDEO * p){
	p->close();
}

#endif //////////////////////////////////////////////////////////////////////////////////////////////////////