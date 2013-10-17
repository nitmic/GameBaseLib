#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Movie.h"

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
}

//--------------------------------- Struct ------------------------------------------
//-----------------------------------------------------------------------------------

struct MovieFile;
struct MovieStream;

typedef std::shared_ptr<MovieFile> FilePtr;
typedef std::shared_ptr<MovieStream> StreamPtr;
typedef std::shared_ptr<AVFrame> FramePtr;
typedef std::shared_ptr<struct SwsContext> SwsCtxPtr;


struct MovieStream{
	MovieStream() : CodecCtx(NULL){init();}
	MovieStream(FilePtr parent, AVCodecContext * context, int idx) : CodecCtx(context), StreamIdx(idx), parent(parent){init();}
	~MovieStream(){
		avcodec_close(CodecCtx);
		if(Data) free(Data);
		if(DecodedData) free(DecodedData);
	}
	void init(){
		Data = NULL;
		DataSize =0;
		DataSizeMax = 0;
		DecodedData = NULL;
		DecodedDataSize = 0;
	}

	AVCodecContext * CodecCtx;

	int StreamIdx;

	char * Data;
	size_t DataSize;
	size_t DataSizeMax;
	char * DecodedData;
	size_t DecodedDataSize;

	std::weak_ptr<MovieFile> parent;
};

struct MovieFile{
	std::shared_ptr<AVFormatContext> FmtCtx;
	std::vector<StreamPtr> Streams;
	~MovieFile(){
		Streams.clear();
		FmtCtx.reset();
	}
};

//------------------------------ Function -------------------------------------------
//-----------------------------------------------------------------------------------

void AVFormatContextRelease(AVFormatContext * p){
	av_close_input_file(p);
}

void AVFrameRelease(AVFrame * p){
	av_free(p);
}

void SwsContextRelease(struct SwsContext * p){
	sws_freeContext(p);
}

namespace{
	FramePtr initFrame(){
		// フレーム領域を確保
		AVFrame *frame_raw = avcodec_alloc_frame();
		if(frame_raw==nullptr) return nullptr;
		return std::shared_ptr<AVFrame>(frame_raw, AVFrameRelease);
	}

	FramePtr initFrame(int width, int height, std::shared_ptr<uint8_t> * pBuffer){
		auto frame = initFrame();

		// 保存用バッファを確保
		int numBytes  = avpicture_get_size(PIX_FMT_RGB32, width, height);
		auto buffer_raw = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
		if(buffer_raw==nullptr) return nullptr;
		*pBuffer = std::shared_ptr<uint8_t>(buffer_raw, av_free);

		// バッファをフレームへセット
		avpicture_fill(
			(AVPicture *)(frame.get()), buffer_raw, PIX_FMT_RGB32, width, height
		);

		return frame;
	}

	FilePtr loadFile(std::string fileName){
		auto file_raw = new MovieFile;
		if(file_raw==nullptr) return nullptr;
		
		auto file = std::shared_ptr<MovieFile>(file_raw);
		// ファイルのヘッダを読み、フォーマットを得る
		AVFormatContext * pFormatContext = NULL;
		if (avformat_open_input(&pFormatContext, fileName.c_str(), NULL, NULL)!=0) return nullptr;
		file->FmtCtx = std::shared_ptr<AVFormatContext>(pFormatContext, AVFormatContextRelease);

		// ファイルの中身からストリーム情報を得る
		if (av_find_stream_info(file->FmtCtx.get()) < 0){
			return nullptr;
		}
		return file;
	}

	StreamPtr createStream(FilePtr file, int index, int width, int height){
		auto stream = std::make_shared<MovieStream>(file, file->FmtCtx->streams[index]->codec, index);
		if (stream==nullptr) return nullptr;

		// codecを探す 
		AVCodec *codec = avcodec_find_decoder(stream->CodecCtx->codec_id);
		if (codec->capabilities & CODEC_CAP_TRUNCATED) stream->CodecCtx->flags |= CODEC_FLAG_TRUNCATED;
		
		// codecを開く
		if (codec==nullptr || avcodec_open2(stream->CodecCtx, codec, NULL) < 0) {
			return false;
		}

		// MovieFileが責任を持つ
		file->Streams.push_back(stream);

		return stream;
	}

	StreamPtr getStream(FilePtr file, int * streamIndex){
		/* ビデオストリームを探す */
		for(auto i=0; i<file->FmtCtx->nb_streams; i++) {
			*streamIndex = i;
			if (file->FmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO) continue;
			
			for(auto j=0; j<file->Streams.size(); j++) {
				if (file->Streams[j]->StreamIdx == i){
					return file->Streams[j];
				}
			}
			return nullptr;
		}
	}

	bool getNextPacket(std::vector<AVFrame> & frameBuffer, FilePtr file, StreamPtr streamV, int * noDecodedBytes){
		AVPacket packet;
		while(av_read_frame(file->FmtCtx.get(), &packet) >= 0){
			for(auto it=file->Streams.begin();it!=file->Streams.end(); it++) {
				// まったく関係ないものは飛ばす
				if((*it)->StreamIdx!=packet.stream_index) continue;
				
				// 対象のストリームでなければ飛ばす
				if (packet.stream_index!=streamV->StreamIdx) continue;
				
				auto frame = initFrame();
				*noDecodedBytes += packet.size;
				uint8_t * rawData = packet.data;
				while (*noDecodedBytes > 0) {
					/* パケットからフレームを復号する  */
					int isFinish = 0;
					auto bytesDecoded = avcodec_decode_video2((*it)->CodecCtx, frame.get(), &isFinish, &packet);

					*noDecodedBytes -= bytesDecoded;
					rawData += bytesDecoded;
					
					/* 復号がまだの場合は次のパケットまで処理を飛ばす */
					if (!isFinish) continue;

					frameBuffer.push_back(*frame);

					return true;
				}
			}
			av_free_packet(&packet);
		}
		return false;
	}
	
	FramePtr getNextFrame(std::vector<AVFrame> & frameBuffer, StreamPtr streamV, int * noDecodedBytes){
		// フレームを拾ってくる
		while (frameBuffer.size()==0) {
			if (!getNextPacket(frameBuffer, streamV->parent.lock(), streamV, noDecodedBytes)) {
				break;
			}
		}
		
		if (frameBuffer.empty()) return nullptr;

		auto output = initFrame();
		*output = frameBuffer.back();
		frameBuffer.erase(frameBuffer.begin());
		return output;
	}
};