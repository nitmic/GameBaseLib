#pragma once
#include <memory>
#include <tString.hpp>

struct Mix_Chunk;
struct Mix_Music;

namespace SDLAdapter{
	template <class T>
	class NonCopyable{
	  protected:
		NonCopyable () {}
		~NonCopyable () {} /// protected な非仮想デストラクタ
	  private: 
		NonCopyable (const NonCopyable &);
		T & operator = (const T &);
	};

	

	class ITune;
	void ReserveTune(std::shared_ptr<ITune> bgm);
	void PlayTune();

	
	class ITune{
	public:
		virtual ~ITune () = 0 {};
	private:
		friend void ReserveTune(std::shared_ptr<ITune> bgm);
		friend void PlayTune();

		virtual void play() = 0;
		virtual void pause() = 0;
	};

	class Tune : public ITune, NonCopyable<Tune>{
	public:
		Tune(TUL::tString path);
		Tune() : bgm(nullptr){};
		~Tune();
	private:
		void play();
		void pause();
		Mix_Music * bgm;
		bool playing;
	};
	
	class Silent : public ITune{
		void play(){}
		void pause(){}
	};


	class GameSE : NonCopyable<GameSE>{
	public:
		enum State{
			UnInit,
			Play,
			Pause,
			Stop
		};
		GameSE() : _raw(nullptr), _channel(-1){};
		~GameSE(){
			freeze();
		}
		void ring(TUL::tString path);
		void freeze();
		State getState();
	private:
		int _channel;
		Mix_Chunk * _raw;
	};
};