#pragma once
#include <tString.hpp>
#include <Singleton.hpp>
#include <SDLWrapper.h>

namespace SDLAdapter{
	Mix_Music * g_CurrentMusic = nullptr;
	class GameMusic{
	public:
		enum State{
			Play,
			Pause,
			Stop
		};

		GameMusic() : _raw(nullptr){};
		~GameMusic(){
			if(getState()==State::Stop) return;
			Mix_HaltMusic();
			g_CurrentMusic = nullptr;
		}
		void load(TUL::tString path){
			if(_raw!=nullptr && getState()==State::Play) return;
			_raw = GetSingleton<TUL::SDLSound>()->loadMusic(path);
		}
		void play(int loop=-1){
			if(getState()==State::Stop){
				Mix_PlayMusic(_raw, loop);
				g_CurrentMusic = _raw;
				return;
			}

			if(getState()==State::Play) return;
			Mix_ResumeMusic(); //resume
		}
		void pause(){
			if(getState()==State::Stop) return;
			if(getState()==State::Pause) return;
			Mix_PauseMusic();
		}
		State getState(){
			if(_raw==nullptr) return State::Stop;
			if(_raw!=g_CurrentMusic) return State::Stop;
			if(Mix_PlayingMusic()) return State::Play;
			return State::Pause;
		}

	private:
		GameMusic (const GameMusic &);
		GameMusic & operator = (const GameMusic &);
		Mix_Music * _raw;
	};

	class GameSE{
	public:
		enum State{
			Play,
			Pause,
			Stop
		};
		GameSE() : _raw(nullptr), _channel(-1){};
		~GameSE(){
			Mix_HaltChannel(_channel);
		}
		void load(TUL::tString path){
			if(_raw!=nullptr && getState()==State::Play) return;
			_raw = GetSingleton<TUL::SDLSound>()->loadSound(path);
		}
		void play(){
			assert(_raw!=nullptr);
			if(getState()==State::Stop){
				_channel = Mix_PlayChannel(-1, _raw, 0);
				return;
			}

			if(getState()==State::Play) return;
			Mix_Resume(_channel); //resume
		}
		void pause(){
			assert(_raw!=nullptr);
			if(getState()==State::Stop) return;
			if(getState()==State::Pause) return;
			Mix_Pause(_channel);
		}
		State getState(){
			if(_channel==-1) return State::Stop;
			if(Mix_GetChunk(_channel)!=_raw) return State::Stop;
			if(Mix_Playing(_channel)) return State::Play;
			return State::Pause;
		}
	private:
		GameSE (const GameSE &);
		GameSE & operator = (const GameSE &);
		int _channel;
		Mix_Chunk * _raw;
	};
};