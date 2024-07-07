#pragma once

/**
 * @file SoundPlayer.hpp
 *
 * @brief Definition of a class for playing sounds
 */


#include <AL/al.h>
#include <AL/alc.h>

#include <cstdio> // Included for libaudio
#include <audio/wave.h>

#include <string>
#include <utility>
#include <map>


class SoundPlayer
{
public:

  SoundPlayer();
  ~SoundPlayer();

  /**
   * @brief Load a wave file, and register the sound by name.
   *
   * @param fileName The name of the wave file to be loaded
   * @param soundName The name against which the sound will be registered
   */
  void load(std::string fileName, std::string soundName = "");

  /**
   * @brief Play a loaded sound. Throw an exception if the sound name has
   *   not been registered.
   *
   * @pram soundName The name against which the desired sound has been registerd
   */
  void play(std::string soundName);

  /**
   * @brief Return true if a sound is currently playing. Useful, for example,
   *   when wating waiting for a sound to finish playing.
   *
   * @return true if a sound is currently playing, false otherwise.
   */
  bool isPlaying();

private:

  ALCdevice *m_pDevice;
  ALCcontext *m_pContext;
  ALuint m_source;

  std::string m_currentSound;
  std::map<std::string, std::pair<ALuint, char *>> m_buffers;

  /**
   * @brief Free resources acquired by constructor. Called in the body of the
   *   destructor.
   */
  void destroy();
};
