#include "MovieIrrAdapter.h"

#include <Singleton.hpp>
#include <Movie.h>

#include "IrrAdapter.h"
#include "ImageIrrAdapter.h"

namespace IrrAdapter{
	Movie::Movie(std::shared_ptr<Image> & output, int w, int h) : m_Raw(nullptr), m_W(w), m_H(h){
		if(output==nullptr) output = std::make_shared<Image>();
		m_Output = output;
		m_Decoder = std::make_shared<TUL::MovieDecoder>(w, h);
	}

	void Movie::changeDecodeSize(int w, int h){
		assert(m_Output!=nullptr);
		m_Decoder->setScreen(w,h);
	}

	bool Movie::open(std::string name){
		assert(m_Output!=nullptr);
		return m_Decoder->open(name.c_str());
	}

	bool Movie::refresh(){
		assert(m_Output!=nullptr);
		
		auto nextRaw = m_Decoder->decode();
		if(m_Decoder->getState()!=TUL::MovieDecoder::Playing) return false;

		if(!nextRaw) return true;
		if(m_Raw!=nextRaw){
			m_Image = GetSingleton<IrrApp>()->accessVideoDriver()->createImageFromData(
				irr::video::ECF_A8R8G8B8,
				irr::core::dimension2d<irr::u32>(m_W, m_H),
				nextRaw,
				true
			);
			m_Raw = nextRaw;
			*m_Output = Image(GetSingleton<IrrApp>()->accessVideoDriver()->addTexture("movie", m_Image));
		}
		auto p = (int*)(m_Output->getRaw()->lock());
		auto pimage = (int*)m_Image->lock ();
		for (int i = 0; i < m_W*m_H; i++) p[i] = pimage[i];
		m_Output->getRaw()->unlock();
		m_Image->unlock();

		return true;
	}

	void Movie::close(){
		assert(m_Output!=nullptr);
		if(m_Decoder->getState()==TUL::MovieDecoder::Closed) return;

		m_Decoder->close();
	}
};