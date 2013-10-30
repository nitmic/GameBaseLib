#include "Music.h"

#include <vector>
#include <algorithm>
#include <Singleton.hpp>
#include <SDLWrapper.h>

namespace SDLAdapter{
	
	static ITune * g_LoadedMusic = nullptr;
	Tune::Tune(TUL::tString path){
		bgm = GetSingleton<TUL::SDLSound>()->loadMusic(path);
	}

	Tune::~Tune(){
		if(this!=g_LoadedMusic) return;
		Mix_HaltMusic();
		g_LoadedMusic = nullptr;
	}

	void Tune::play(){
		if(this==g_LoadedMusic){
			// 継続時
			if(playing) return;
			// silentから戻ってきた時
			Mix_ResumeMusic();
			playing = true;
			return;
		}

		// 初回 or 一度再生->何か他の曲を再生(強制Halt)->もう一度再生
		Mix_PlayMusic(bgm, -1);
		g_LoadedMusic = this;
		playing = true;
	}

	void Tune::pause(){
		if(this!=g_LoadedMusic) return;
		Mix_PauseMusic();
		playing = false;
	}
	
	// ---------------------------

	static std::vector<std::shared_ptr<ITune>> g_MusicStack;
	static std::shared_ptr<ITune> g_CurrentMusic = nullptr;

	void ReserveTune(std::shared_ptr<ITune> bgm){
		// あったらそっちを消して新しく追加の方がよい？
		if(std::find(g_MusicStack.begin(), g_MusicStack.end(), bgm)==g_MusicStack.end()){
			g_MusicStack.push_back(bgm);
		}
	}
	
	void PlayTune(){
		if(g_MusicStack.empty()){
			g_CurrentMusic = nullptr;
			return;
		}

		if(g_CurrentMusic!=g_MusicStack.back()){
			// 前回と曲が違う場合
			if(g_CurrentMusic!=nullptr) g_CurrentMusic->pause();
		}

		g_CurrentMusic = g_MusicStack.back(); // stop();動く場合あり
		g_CurrentMusic->play();
		g_MusicStack.pop_back();
		return;
	}
	

	// ---------------------------


	void GameSE::ring(TUL::tString path){
		freeze();
		_raw = GetSingleton<TUL::SDLSound>()->loadSound(path);
		_channel = Mix_PlayChannel(-1, _raw, 0);
	}

	void GameSE::freeze(){
		if(getState()==State::UnInit) return;
		if(getState()==State::Stop) return;
		Mix_HaltChannel(_channel);
		_channel = -1;
	}

	GameSE::State GameSE::getState(){
		if(_raw==nullptr) return State::UnInit;
		if(_channel==-1) return State::Stop;
		if(Mix_GetChunk(_channel)!=_raw) return State::Stop;
		if(Mix_Playing(_channel)) return State::Play;
		return State::Pause;
	}
};