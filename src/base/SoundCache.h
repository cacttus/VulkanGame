/**
*  @file SoundCache.h
*  @date January 19, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SOUNDCACHE_1484887033955700376_H__
#define __SOUNDCACHE_1484887033955700376_H__

#include "../base/BaseHeader.h"

namespace BR2 {
class SoundSpec : public VirtualMemory {
public:
  SoundSpec(const string_t& sFile);
  virtual ~SoundSpec() override;

  void update();
  void load(const string_t& file);
  std::shared_ptr<SoundInst> play(const SoundPlayInfo& inf);
  std::vector<std::shared_ptr<SoundInst>>& getInstances() { return _vecInstances; }
private:
  std::vector<std::shared_ptr<SoundInst>> _vecInstances;
  string_t _sFilePath;
  //    Mix_Music* _pMusic = nullptr;
  //Mix_Chunk* _pChunk = nullptr;
  int32_t _iChannels = 0;
  int32_t _nSamples = 0;
  int32_t _iSampleRate = 0;
  int16_t* _pSoundData = nullptr;
  int32_t _iSoundDataLenBytes = 0;
  // SDL_AudioSpec _audioSpec ;
  LoadState::e _eLoadState = LoadState::e::NotLoaded;
};
class SoundInst : public VirtualMemory {
public:
  SoundInst(int16_t* pos, int32_t len);
  virtual ~SoundInst() override;

  void play(const SoundPlayInfo& inf);
  void mixIntoBuffer(uint8_t* buf, int buflen);

public:
  SoundPlayInfo _playInfo;
  PlayState::e _ePlayState = PlayState::e::Stopped;
  bool _bError = false;
  //SDL_AudioSpec _desired;
  // SDL_AudioSpec _have ;
  std::shared_ptr<SoundSpec> _pSoundSpec = nullptr;

  int8_t* audio_pos = nullptr; // **THIS MUST BE A BYTE BECAUSE WE OFFSET IT BY BYTES!!
  int32_t audio_len = 0; // remaining length of the sample we have to play
                         // SDL_AudioDeviceID _iSDLAudioDevice = 0;

private:
  int8_t* start_pos = nullptr;
  int32_t start_len = 0;
  void checkPlayback();
};
class SoundCache_Internal;
class SoundCache : public VirtualMemory {
public:
  SoundCache();
  virtual ~SoundCache() override;

  void init();
  std::shared_ptr<SoundSpec> getOrLoad(const string_t& file);
  void update();
  std::shared_ptr<SoundInst> tryPlay(const string_t& file);
  std::shared_ptr<SoundInst> tryPlay(const string_t& file, const SoundPlayInfo& inf);
  void mixSamplesAsync(uint8_t* stream, int len);

private:
  std::unique_ptr<SoundCache_Internal> _pint;
};

}//ns br2



#endif
