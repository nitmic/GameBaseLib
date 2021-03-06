
// header ///////////////////////////////////////////////////////////////////////////////////////////////////
#include "murmuurVIDEO.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

// audio hard quit //////////////////////////////////////////////////////////////////////////////////////////
static volatile int quitnow = 0;
static void handle_sigint(int signum) {
    (void)signum;
    quitnow = 1;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// constructor default //////////////////////////////////////////////////////////////////////////////////////
murmuurVIDEO::murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer,  
                     int desiredW, int desiredH) : _vdVideoDriver(irrVideoDriver), 
                     /*_itTimer(timer), */_iDesiredH(desiredH), _iDesiredW(desiredW) {    
   this->mnOutputMesh = NULL;
   psVideostate = Closed;
   _initAV();
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// constructor alternate mesh output ////////////////////////////////////////////////////////////////////////
murmuurVIDEO::murmuurVIDEO(irr::video::IVideoDriver *irrVideoDriver, irr::ITimer *timer, int desiredW, 
                     int desiredH, IMeshSceneNode *outputMesh) : _vdVideoDriver(irrVideoDriver), 
                     /*_itTimer(timer), */_iDesiredH(desiredH), _iDesiredW(desiredW), 
                     mnOutputMesh(outputMesh){
   _initAV();   
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// initialise audio/video ///////////////////////////////////////////////////////////////////////////////////
bool murmuurVIDEO::_initAV(void) {
   // initial video flags
   first_time = true;
   bVideoLoaded = false;
   psVideostate = Closed;
   _vdVideoDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
    _vdVideoDriver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
   _txCurrentTexture = NULL;
   _bFrameDisplayed = true;
   _iBasetime = 0;
   iActualFrame = 0;
    
   // Register all formats and codecs
   av_register_all();

#ifdef SOUND_OPENAL
   // signal handler
   if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        fprintf(stderr, "Unable to set handler for SIGINT!\n");
        return false;
    }

   // audio temp buffer
    _abData = (ALbyte *)malloc(BUFFER_SIZE);
    if (!_abData) {
        fprintf(stderr, "Out of memory allocating temp buffer!\n");
        return false;
    }

    // Initialize ALUT with default settings 
    if (alutInit(NULL, NULL) == AL_FALSE) {
        free(_abData);
        fprintf(stderr, "Could not initialize ALUT (%s)!\n", alutGetErrorString(alutGetError()));
        return false;
    }

    // Generate the buffers and source 
    alGenBuffers(NUM_BUFFERS, _aiBuffers);
    if (alGetError() != AL_NO_ERROR) {
        alutExit();
        free(_abData);
        fprintf(stderr, "Could not create buffers...\n");
        return false;
    }
    alGenSources(1, &_aiSource);
    if (alGetError() != AL_NO_ERROR) {
        alDeleteBuffers(NUM_BUFFERS, _aiBuffers);
        alutExit();
        free(_abData);
        fprintf(stderr, "Could not create source...\n");
        return false;
    }

    // Set parameters so mono sources won't distance attenuate 
    alSourcei(_aiSource, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(_aiSource, AL_ROLLOFF_FACTOR, 0);
    if (alGetError() != AL_NO_ERROR) {
        alDeleteSources(1, &_aiSource);
        alDeleteBuffers(NUM_BUFFERS, _aiBuffers);
        alutExit();
        free(_abData);
        fprintf(stderr, "Could not set source parameters...\n");
        return false;
    }
#endif

   return true;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// get the next frame from the buffer ///////////////////////////////////////////////////////////////////////
AVFrame *murmuurVIDEO::_getNextFrame(void) {
   // get more frames if buffer empty
   while (_frFrame_Buffer.size() == 0) {
      if (!_getNextPacket(_spStreamA->parent, _spStreamA->StreamIdx)) {
         break;
      }
   }
   
   // return a frame if we have one
   if (_frFrame_Buffer.size() > 0) { // we have frames
      AVFrame *t = avcodec_alloc_frame();
      *t = _frFrame_Buffer.back();
      _frFrame_Buffer.erase(_frFrame_Buffer.begin());
      return t;
   } else {
      return NULL;
   }   
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// refresh audio/video //////////////////////////////////////////////////////////////////////////////////////
bool murmuurVIDEO::refresh(void) {
	if(psVideostate!=Playing) return false;

   static struct SwsContext *img_convert_ctx;
   static int currentX = 0;
   static int currentY = 0;
   bool needResize = false;

#ifdef SOUND_OPENAL
   if (_bHasAudio) {
      // ensure we arnt out of sound
      if (_iBuffCount > 0 && !quitnow) {
         // Check if any buffers on the source are finished playing 
         ALint processed = 0;
         alGetSourcei(_aiSource, AL_BUFFERS_PROCESSED, &processed);
         if (processed == 0) {
            // All buffers are full. Check if the source is still playing.
            // If not, restart it, otherwise, print the time and rest 
            alGetSourcei(_aiSource, AL_SOURCE_STATE, &_aiState);
            if (alGetError() != AL_NO_ERROR) {
               fprintf(stderr, "Error checking source state...\n");
               return false;
            }
            if (_aiState != AL_PLAYING) {
               alSourcePlay(_aiSource);
               if (alGetError() != AL_NO_ERROR) {
                  _closeAVFile(_fpFile);
                  fprintf(stderr, "Error restarting playback...\n");
                  return false;
               }
            } else {
               ALint offset;
               alGetSourcei(_aiSource, AL_SAMPLE_OFFSET, &offset);
               // Add the base time to the offset. Each count of basetime
               // represents one buffer, which is BUFFER_SIZE in bytes 
               offset += _iBasetime * (BUFFER_SIZE/_iChannels*8/_iBits);
               //fprintf(stderr, "\rTime: %d:%05.02f", offset/_iRate/60, (offset%(_iRate*60))/(float)_iRate);
               //alutSleep((ALfloat)0.01);
            }

            // all done for this iteration
            //return true;
            goto allaudiodone;
         }

         // Read the next chunk of data and refill the oldest buffer 
         _iBuffCount = _getAVAudioData(_spStreamA, _abData, BUFFER_SIZE);
         if (_iBuffCount > 0) {
            ALuint buf = 0;
            alSourceUnqueueBuffers(_aiSource, 1, &buf);
            if (buf != 0) {
               alBufferData(buf, _aeFormat, _abData, _iBuffCount, _iRate);
               alSourceQueueBuffers(_aiSource, 1, &buf);
               // For each successfully unqueued buffer, increment the
               // base time. The retrieved sample offset for timing is
               // relative to the start of the buffer queue, so for every
               // buffer that gets unqueued we need to increment the base
               // time to keep the reported time accurate and not fall backwards 
               _iBasetime++;
            }
            if (alGetError() != AL_NO_ERROR) {
               fprintf(stderr, "Error buffering data...\n");
               return false;
            }
         }
      } else { // out of audio
         return false;
      }
   }
#endif

allaudiodone:
   if (_bHasVideo) {
      // process the next video frame from the buffer      
      //if (_itTimer->getRealTime() - _lLastTime > (dSecondsPerFrame*1000)) {
	   if(first_time || fps.step()){
         //_lLastTime = _itTimer->getRealTime();
         _frFrame = _getNextFrame();
         if (_frFrame != NULL) {
            if (img_convert_ctx == NULL) {
               currentX = _iDesiredW;
               currentY = _iDesiredH;

               int w = _spStreamV->CodecCtx->width;
               int h = _spStreamV->CodecCtx->height;

               img_convert_ctx = sws_getContext(w, h, _spStreamV->CodecCtx->pix_fmt, _iDesiredW, _iDesiredH, PIX_FMT_RGB32, 
                  SWS_FAST_BILINEAR | SWS_CPU_CAPS_MMX2, NULL, NULL, NULL);
               if (img_convert_ctx == NULL) {
                  fprintf(stderr, "Cannot initialize the conversion context!\n");
                  return false;
               }
            } else if (currentX != _iDesiredW || currentY != _iDesiredH) {
               needResize = true;
               currentX = _iDesiredW;
               currentY = _iDesiredH;

               int w = _spStreamV->CodecCtx->width;
               int h = _spStreamV->CodecCtx->height;

               sws_freeContext(img_convert_ctx);
               img_convert_ctx = NULL;

               img_convert_ctx = sws_getContext(w, h, _spStreamV->CodecCtx->pix_fmt, _iDesiredW, _iDesiredH, PIX_FMT_RGB32, 
                  SWS_FAST_BILINEAR | SWS_CPU_CAPS_MMX2, NULL, NULL, NULL);
               if (img_convert_ctx == NULL) {
                  fprintf(stderr, "Cannot re-initialize the conversion context!\n");
                  return false;
               }
            }

            sws_scale(img_convert_ctx, _frFrame->data, _frFrame->linesize, 0, _spStreamV->CodecCtx->height, _frFrameRGB->data, _frFrameRGB->linesize);

            //printf("Dumping Frame: %d  ::  FrameRate: %f\n", iActualFrame, fFramerate);

            // Dump the frame
            if(!_DumpFrame(_frFrameRGB, _iDesiredW, _iDesiredH, needResize)){
				printf("");
			}

            // increase frame/time counts
            iActualFrame++;
            
            _bFrameDisplayed = false;
         } else {
            return false;
         }
      } 
   }
   
   assert(_txCurrentTexture!=NULL);
   // success, more audio/video to follow
   return true;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ImageIrrAdapter.h"
IrrAdapter::Image murmuurVIDEO::decode(){
	assert(_txCurrentTexture!=NULL);
	if(_txCurrentTexture==NULL) return IrrAdapter::Image();
	return IrrAdapter::Image(_txCurrentTexture);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// draw the current texture onscreen ////////////////////////////////////////////////////////////////////////
void murmuurVIDEO::drawVideoTexture(void){
   if (_txCurrentTexture != NULL) {
      if (mnOutputMesh != NULL) {
         if (!_bFrameDisplayed) {
            mnOutputMesh->setMaterialTexture(0, _txCurrentTexture);
            _bFrameDisplayed = true;
         }
      } else {
         _vdVideoDriver->draw2DImage(_txCurrentTexture, irr::core::position2d<irr::s32>(0,0),
            irr::core::rect<irr::s32>(0,0,_iDesiredW,_iDesiredH), 0, irr::video::SColor(255,255,255,255), false);
      }
   }
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// refresh audio/video //////////////////////////////////////////////////////////////////////////////////////
bool murmuurVIDEO::open(core::stringc sFileName) {
   // reset if already playing
   if (psVideostate != Closed) {
      close();
      _initAV();
   }

   // reset initial vars
   _iBasetime = 0;

    // check the file opens up
    _fpFile = _openAVFile(sFileName.c_str());
   if (!_fpFile) {
      fprintf(stderr, "Could not open %s\n", sFileName.c_str());
        return false;
   }

   // create the stream objects
   _spStreamV = _getAVVideoStream(_fpFile, 0);
   _bHasVideo = !(!_spStreamV);
    /*if (!_spStreamV) {
        _closeAVFile(_fpFile);
        fprintf(stderr, "Could not open video in %s\n", sFileName.c_str());
        return false;
    }*/
    _spStreamA = _getAVAudioStream(_fpFile, 0);
   _bHasAudio = !(!_spStreamA);
    if (!_bHasAudio && _bHasVideo) {
        //_closeAVFile(_fpFile);
        //fprintf(stderr, "Could not open audio in %s\n", sFileName.c_str());
        //return false;
      _spStreamA = (StreamPtr)calloc(1, sizeof(*_spStreamA));
      _spStreamA->parent = _fpFile;      
      _spStreamA->StreamIdx = -1;
      void *temp = realloc(_fpFile->Streams, (_fpFile->StreamsSize+1) * sizeof(*_fpFile->Streams));      
      _fpFile->Streams = (StreamPtr*)temp;
      _fpFile->Streams[_fpFile->StreamsSize++] = _spStreamA;
    }
    
   // audio specific open init
   if (_bHasAudio) {
      // Get the stream format, and figure out the OpenAL format. We use the
      // AL_EXT_MCFORMATS extension to provide output of 4 and 5.1 audio streams 
      if (_getAVAudioInfo(_spStreamA, &_iRate, &_iChannels, &_iBits) != 0) {
         _closeAVFile(_fpFile);
         fprintf(stderr, "Error getting audio info for %s\n", sFileName.c_str());
         return false;
      }

#ifdef SOUND_OPENAL
	  _aeFormat = 0;
      // determine the sound formats
      if (_iBits == 8) {
         if (_iChannels == 1) _aeFormat = AL_FORMAT_MONO8;
         if (_iChannels == 2) _aeFormat = AL_FORMAT_STEREO8;
         if (alIsExtensionPresent("AL_EXT_MCFORMATS")) {
            if (_iChannels == 4) _aeFormat = alGetEnumValue("AL_FORMAT_QUAD8");
            if (_iChannels == 6) _aeFormat = alGetEnumValue("AL_FORMAT_51CHN8");
         }
      }
      if (_iBits == 16) {
         if (_iChannels == 1) _aeFormat = AL_FORMAT_MONO16;
         if (_iChannels == 2) _aeFormat = AL_FORMAT_STEREO16;
         if (alIsExtensionPresent("AL_EXT_MCFORMATS")) {
            if (_iChannels == 4) _aeFormat = alGetEnumValue("AL_FORMAT_QUAD16");
            if (_iChannels == 6) _aeFormat = alGetEnumValue("AL_FORMAT_51CHN16");
         }
      }
      if (_aeFormat == 0) {
         _closeAVFile(_fpFile);
         fprintf(stderr, "Unhandled format (%d channels, %d bits) for %s", _iChannels, _iBits, sFileName.c_str());
         return false;
      }

      // If the format of the last file matches the current one, we can skip
      // the initial load and let the processing loop take over (gap-less playback!) 
      _iBuffCount = 1;
      if (_aeFormat != _aeOldFormat || _iRate != _iOld_rate) {
         int j;

         _aeOldFormat = _aeFormat;
         _iOld_rate = _iRate;

         // Wait for the last song to finish playing 
         do {
            alutSleep((ALfloat)0.01);
            alGetSourcei(_aiSource, AL_SOURCE_STATE, &_aiState);
         } while(alGetError() == AL_NO_ERROR && _aiState == AL_PLAYING);

         // Rewind the source position and clear the buffer queue 
         alSourceRewind(_aiSource);
         alSourcei(_aiSource, AL_BUFFER, 0);

         // Fill and queue the buffers 
         for(j = 0;j < NUM_BUFFERS;j++) {
            // Make sure we get some data to give to the buffer 
            _iBuffCount = _getAVAudioData(_spStreamA, _abData, BUFFER_SIZE);
            if(_iBuffCount <= 0) return false;

            // Buffer the data with OpenAL and queue the buffer onto the source 
            alBufferData(_aiBuffers[j], _aeFormat, _abData, _iBuffCount, _iRate);
            alSourceQueueBuffers(_aiSource, 1, &_aiBuffers[j]);
         }
         if (alGetError() != AL_NO_ERROR) {
            _closeAVFile(_fpFile);
            fprintf(stderr, "Error buffering initial data...\n");
            return false;
         }

         // Now start playback! 
         alSourcePlay(_aiSource);
         if (alGetError() != AL_NO_ERROR) {
            _closeAVFile(_fpFile);
            fprintf(stderr, "Error starting playback...\n");
            return false;
         }
      } else {
         // When skipping the initial load of a file (because the previous
         // one is using the same exact format), set the base time to the
         // negative of the queued buffers. This is so the timing will be
         // from the beginning of this file, which won't start playing until
         // the next buffer to get queued does */
         _iBasetime = -NUM_BUFFERS;
      }
#endif
   }

   // video specific checks
   if (_bHasVideo) {
      if (mnOutputMesh != NULL) {
         mnOutputMesh->setVisible(true);
      }
   }

   // set state
   if (_bHasVideo || _bHasAudio) {
      psVideostate = Playing;
      bVideoLoaded = true;
      return true;
   } else {
      return false;
   }

} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// dump the frame to texture ////////////////////////////////////////////////////////////////////////////////
bool murmuurVIDEO::_DumpFrame(AVFrame *pFrame, int width, int height, bool needResize) {
    if (first_time) {
      _imCurrentImage = _vdVideoDriver->createImageFromData(irr::video::ECF_A8R8G8B8,
                       irr::core::dimension2d<irr::u32>(width, height),
                       pFrame->data[0],
                       true);
        first_time = false;
        _txCurrentTexture = _vdVideoDriver->addTexture("movie", _imCurrentImage);
    }

    if (needResize) {
       _vdVideoDriver->removeTexture(_txCurrentTexture);
       _imCurrentImage = _vdVideoDriver->createImageFromData(irr::video::ECF_A8R8G8B8,
                              irr::core::dimension2d<irr::u32>(width, height),
                              pFrame->data[0],
                              true);
        _txCurrentTexture = _vdVideoDriver->addTexture("movie", _imCurrentImage);
    }

	if(_txCurrentTexture==NULL) return false;
    _p = (s32*)_txCurrentTexture->lock ();
    _pimage = (s32*)_imCurrentImage->lock ();

    for (int i = 0; i < width*height; i++) _p[i] = _pimage[i];

    // unlock de texture and the image
    _txCurrentTexture->unlock();
    _imCurrentImage->unlock();

   return true;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal open file ///////////////////////////////////////////////////////////////////////////////////////
FilePtr murmuurVIDEO::_openAVFile(const char *fname) {
    static int done = 0;
    FilePtr file;

    // We need to make sure ffmpeg is initialized. Optionally silence warning output from the lib 
    if(!done) {
		av_register_all();
		av_log_set_level(AV_LOG_ERROR);
	}
    done = 1;

    file = (FilePtr)calloc(1, sizeof(*file));
    if (file && avformat_open_input(&file->FmtCtx, fname, NULL, NULL) == 0) {
        // After opening, we must search for the stream information because not
        // all formats will have it in stream headers (eg. system MPEG streams)
        if (av_find_stream_info(file->FmtCtx) >= 0) return file;
        av_close_input_file(file->FmtCtx);
    }
    free(file);
    return NULL;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal close file //////////////////////////////////////////////////////////////////////////////////////
void murmuurVIDEO::_closeAVFile(FilePtr file) {
    size_t i;

    if(!file) return;

    for(i = 0;i < file->StreamsSize;i++) {
        if(file->Streams[i]->CodecCtx) avcodec_close(file->Streams[i]->CodecCtx);
        if(file->Streams[i]->Data) free(file->Streams[i]->Data);
		if(file->Streams[i]->DecodedData) free(file->Streams[i]->DecodedData);
        free(file->Streams[i]);
    }
    free(file->Streams);

    av_close_input_file(file->FmtCtx);
    free(file);
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal find the relevent streams ///////////////////////////////////////////////////////////////////////
StreamPtr murmuurVIDEO::_getAVAudioStream(FilePtr file, int streamnum) {
    unsigned int i;
    if (!file) return NULL;
    for(i = 0;i < file->FmtCtx->nb_streams;i++) {
        if (file->FmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_AUDIO) continue;

        if (streamnum == 0) {
            StreamPtr stream;
            AVCodec *codec;
            void *temp;
            size_t j;

            // Found the requested stream. Check if a handle to this stream
            // already exists and return it if it does 
            for(j = 0;j < file->StreamsSize;j++) {
                if (file->Streams[j]->StreamIdx == (int)i) return file->Streams[j];
            }

            // Doesn't yet exist. Now allocate a new stream object and fill in its info 
            stream = (StreamPtr)calloc(1, sizeof(*stream));
            if (!stream) return NULL;
            stream->parent = file;
            stream->CodecCtx = file->FmtCtx->streams[i]->codec;
            stream->StreamIdx = i;

            // Try to find the codec for the given codec ID, and open it 
            codec = avcodec_find_decoder(stream->CodecCtx->codec_id);
            if (!codec || avcodec_open2(stream->CodecCtx, codec, NULL) < 0) {
                free(stream);
                return NULL;
            }

            // Allocate space for the decoded data to be stored in before it gets passed to the app 
            stream->DecodedData = (char *)malloc(MAX_AUDIO_FRAME_SIZE);
            if (!stream->DecodedData) {
                avcodec_close(stream->CodecCtx);
                free(stream);
                return NULL;
            }

            // Append the new stream object to the stream list. The original
            // pointer will remain valid if realloc fails, so we need to use
            // another pointer to watch for errors and not leak memory 
            temp = realloc(file->Streams, (file->StreamsSize+1) * sizeof(*file->Streams));
            if (!temp) {
                avcodec_close(stream->CodecCtx);
                free(stream->DecodedData);
                free(stream);
                return NULL;
            }
            file->Streams = (StreamPtr*)temp;
            file->Streams[file->StreamsSize++] = stream;
            return stream;
        }
        streamnum--;
    }   
    return NULL;
}
StreamPtr murmuurVIDEO::_getAVVideoStream(FilePtr file, int streamnum) {
    unsigned int i;
    if (!file) return NULL;
    for(i = 0;i < file->FmtCtx->nb_streams;i++) {
        if (file->FmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO) continue;

        if (streamnum == 0) {
            StreamPtr stream;
            AVCodec *codec;
            void *temp;
            size_t j;

            // Found the requested stream. Check if a handle to this stream
            // already exists and return it if it does 
            for(j = 0;j < file->StreamsSize;j++) {
                if (file->Streams[j]->StreamIdx == (int)i) return file->Streams[j];
            }

            // Doesn't yet exist. Now allocate a new stream object and fill in its info 
            stream = (StreamPtr)calloc(1, sizeof(*stream));
            if (!stream) return NULL;
            stream->parent = file;
            stream->CodecCtx = file->FmtCtx->streams[i]->codec;
            stream->StreamIdx = i;

            // Try to find the codec for the given codec ID, and open it 
            codec = avcodec_find_decoder(stream->CodecCtx->codec_id);
         if (codec->capabilities & CODEC_CAP_TRUNCATED) stream->CodecCtx->flags|=CODEC_FLAG_TRUNCATED;
            if (!codec || avcodec_open2(stream->CodecCtx, codec, NULL) < 0) {
                free(stream);
                return NULL;
            }

         // get the movie framerate
         fFramerate = (float)file->FmtCtx->streams[i]->r_frame_rate.num;
         dSecondsPerFrame = (double)file->FmtCtx->streams[i]->r_frame_rate.den / 
            file->FmtCtx->streams[i]->r_frame_rate.num;
		 fps.setFPS(1/dSecondsPerFrame);
         fDuration = (float)file->FmtCtx->streams[i]->duration;
         iNum_frames = (int)file->FmtCtx->streams[i]->nb_frames;

         // setup temp allocations         
         _frFrame = avcodec_alloc_frame();
         _frFrameRGB=avcodec_alloc_frame();
         if (_frFrameRGB == NULL) return NULL;
         _iNumBytes = avpicture_get_size(PIX_FMT_RGB32, _iDesiredW, _iDesiredH);
         _iBuffer = new uint8_t[_iNumBytes];
         avpicture_fill((AVPicture *)_frFrameRGB, _iBuffer, PIX_FMT_RGB32, _iDesiredW, _iDesiredH);

            // Append the new stream object to the stream list. The original
            // pointer will remain valid if realloc fails, so we need to use
            // another pointer to watch for errors and not leak memory 
            temp = realloc(file->Streams, (file->StreamsSize+1) * sizeof(*file->Streams));
            if (!temp) {
                avcodec_close(stream->CodecCtx);
                free(stream->DecodedData);
                free(stream);
                return NULL;
            }
            file->Streams = (StreamPtr*)temp;
            file->Streams[file->StreamsSize++] = stream;
            return stream;
        }
        streamnum--;
    }
    return NULL;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal grab audio stream bits etc //////////////////////////////////////////////////////////////////////
int murmuurVIDEO::_getAVAudioInfo(StreamPtr stream, int *rate, int *channels, int *bits) {
	if (!stream || stream->CodecCtx->codec_type != AVMEDIA_TYPE_AUDIO) return 1;

    if (rate) *rate = stream->CodecCtx->sample_rate;
    if (channels) *channels = stream->CodecCtx->channels;
    if (bits) *bits = 16;

    return 0;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal get next packet /////////////////////////////////////////////////////////////////////////////////
bool murmuurVIDEO::_getNextPacket(FilePtr file, int streamidx) {
    static AVPacket packet;
    static int bytesRemaining=0;
    static uint8_t  *rawData;
    static int bytesDecoded;
    static int frameFinished;
   AVFrame *pFrame;
   pFrame = avcodec_alloc_frame();

   // read frames until we have an audio packet to return
    while(av_read_frame(file->FmtCtx, &packet) >= 0) {
        StreamPtr *iter = file->Streams;
        size_t i;

        // Check each stream the user has a handle for, looking for the one this packet belongs to 
        for(i = 0;i < file->StreamsSize;i++,iter++) {
            if ((*iter)->StreamIdx == packet.stream_index) {
            if (packet.stream_index == streamidx) {  // audio packets      
               size_t idx = (*iter)->DataSize;

               // Found the stream. Grow the input data buffer as needed to
               // hold the new packet's data. Additionally, some ffmpeg codecs
               // need some padding so they don't overread the allocated buffer
               if (idx+packet.size > (*iter)->DataSizeMax) {
                  void *temp = realloc((*iter)->Data, idx+packet.size + FF_INPUT_BUFFER_PADDING_SIZE);
                  if (!temp) break;
                  (*iter)->Data = (char *)temp;
                  (*iter)->DataSizeMax = idx+packet.size;
               }

               // Copy the packet and free it 
               memcpy(&(*iter)->Data[idx], packet.data, packet.size);
               (*iter)->DataSize += packet.size;

               // Return if this stream is what we needed a packet for 
               if (streamidx == (*iter)->StreamIdx) {
                  av_free_packet(&packet);
                  return true;
               }               
               break;
            } else if (_bHasVideo) {  // continue decoding video frames to the buffer
               if (packet.stream_index == _spStreamV->StreamIdx) {
                  bytesRemaining += packet.size;
                  rawData = packet.data;
                  
                  // Work on the current packet until we have decoded all of it
                  while (bytesRemaining > 0) {
                     // Decode the next chunk of data
                     bytesDecoded = avcodec_decode_video2((*iter)->CodecCtx, pFrame, &frameFinished, &packet);

                     // Was there an error?
                     if (bytesDecoded < 0) {
                        fprintf(stderr, "Error while decoding frame\n");
                        //return false;
                     }

                     bytesRemaining -= bytesDecoded;
                     rawData += bytesDecoded;

                     // Did we finish the current frame? Then we can return
                     if (frameFinished) { // add the current frame to the buffer
                        _frFrame_Buffer.push_back(*pFrame);
                        av_free(pFrame);
                        frameFinished = false;
                        if (!_bHasAudio) {
                           return true;
                        }
                        pFrame = avcodec_alloc_frame();                        
                     }
                  }
               }
            }
            }
        }

        // Free the packet and look for another 
        av_free_packet(&packet);
      
    }

   if (pFrame != NULL)
      av_free(pFrame);
   return false;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// internal get audio data //////////////////////////////////////////////////////////////////////////////////
int murmuurVIDEO::_getAVAudioData(StreamPtr stream, void *data, int length) {
    int dec = 0;

    if (!stream || stream->CodecCtx->codec_type != AVMEDIA_TYPE_AUDIO) return 0;
    while(dec < length) {
        // If there's any pending decoded data, deal with it first 
        if (stream->DecodedDataSize > 0) {
            // Get the amount of bytes remaining to be written, 
         // and clamp to the amount of decoded data we have 
            size_t rem = length-dec;
            if (rem > stream->DecodedDataSize) rem = stream->DecodedDataSize;

            // Copy the data to the app's buffer and increment 
            memcpy(data, stream->DecodedData, rem);
            data = (char*)data + rem;
            dec += rem;

            // If there's any decoded data left, move it to the front of the
            // buffer for next time 
            if (rem < stream->DecodedDataSize)
                memmove(stream->DecodedData, &stream->DecodedData[rem], stream->DecodedDataSize - rem);
            stream->DecodedDataSize -= rem;
        }

        // Check if we need to get more decoded data 
        if (stream->DecodedDataSize == 0) {
            size_t insize;
            int size;
            int len;

            insize = stream->DataSize;
            if (insize == 0) {
                _getNextPacket(stream->parent, stream->StreamIdx);
                
            // If there's no more input data, break and return what we have 
                if (insize == stream->DataSize) break;
                insize = stream->DataSize;
                memset(&stream->Data[insize], 0, FF_INPUT_BUFFER_PADDING_SIZE);
            }

            // Clear the input padding bits 
            // Decode some data, and check for errors 
            size = MAX_AUDIO_FRAME_SIZE;
			
			AVPacket avpkt;
			av_init_packet(&avpkt);
			avpkt.data = (uint8_t*)stream->Data;
			avpkt.size = insize;

            while(
				(len=avcodec_decode_audio3(
					stream->CodecCtx, (int16_t*)stream->DecodedData, &size, &avpkt)
				) == 0
			){
				if (size > 0) break;
				_getNextPacket(stream->parent, stream->StreamIdx);
				if (insize == stream->DataSize) break;
				insize = stream->DataSize;
				memset(&stream->Data[insize], 0, FF_INPUT_BUFFER_PADDING_SIZE);
            }

            if (len < 0) break;

            if (len > 0) {
                // If any input data is left, move it to the start of the
                // buffer, and decrease the buffer size 
                size_t rem = insize-len;
                if (rem) memmove(stream->Data, &stream->Data[len], rem);
                stream->DataSize = rem;
            }
            // Set the output buffer size 
            stream->DecodedDataSize = size;
        }
    }

    // Return the number of bytes we were able to get 
    return dec;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// switch res ///////////////////////////////////////////////////////////////////////////////////////////////
void murmuurVIDEO::changeResolution(int w, int h) {
   if (_iDesiredW != w || _iDesiredH != h){
      std::cout << "Changing resolution from ["<< _iDesiredW << "x" << _iDesiredH << "] to [" << w << "x" << h << "]" << std::endl;

      _iDesiredW = w;
      _iDesiredH = h;

      delete [] _iBuffer;
      //av_free((AVPicture *)_frFrameRGB);
      _iNumBytes = avpicture_get_size(PIX_FMT_RGB32, _iDesiredW, _iDesiredH);

      _iBuffer = new uint8_t[_iNumBytes];

      // Assign appropriate parts of buffer to image planes in pFrameRGB
      avpicture_fill((AVPicture *)_frFrameRGB, _iBuffer, PIX_FMT_RGB32, _iDesiredW, _iDesiredH);
   }
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// close the current active file ////////////////////////////////////////////////////////////////////////////
void murmuurVIDEO::close(void) {
   if(psVideostate==Closed) return;
   // Free the RGB image
   if (_iBuffer != NULL)
      delete [] _iBuffer;

   if (_frFrameRGB != NULL)
      av_free(_frFrameRGB);

   // Free the YUV frame
   if (_frFrame != NULL)
      av_free(_frFrame);

   // clear the frame buffer
   _frFrame_Buffer.clear();
   _vdVideoDriver->removeTexture(_txCurrentTexture);

#ifdef SOUND_OPENAL
   // All data has been streamed in. Wait until the source stops playing it 
    do {
        alutSleep((ALfloat)0.01);
        alGetSourcei(_aiSource, AL_SOURCE_STATE, &_aiState);
    } while(alGetError() == AL_NO_ERROR && _aiState == AL_PLAYING);

    // All files done. Delete the source and buffers, and close OpenAL 
    alDeleteSources(1, &_aiSource);
    alDeleteBuffers(NUM_BUFFERS, _aiBuffers);
    alutExit();
    free(_abData);
#endif

   // close the file
   _closeAVFile(_fpFile);
    fprintf(stderr, "\nDone.\n");

   bVideoLoaded = false;
   psVideostate = Closed;
} ///////////////////////////////////////////////////////////////////////////////////////////////////////////


// destruct /////////////////////////////////////////////////////////////////////////////////////////////////
murmuurVIDEO::~murmuurVIDEO(){}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
