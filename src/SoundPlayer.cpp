/**
 * @file SoundPlayer.cpp
 *
 * @brief Implemenation of a class for playing sounds
 */


#include "SoundPlayer.hpp"

#include <stdexcept>
#include <array>

#include <cstdint>


SoundPlayer::SoundPlayer()
: m_pDevice(nullptr)
, m_pContext(nullptr)
, m_source(0)
, m_currentSound()
, m_buffers()
{
  // Open default audio device

  m_pDevice = alcOpenDevice(nullptr);

  if (m_pDevice == nullptr)
  {
    throw std::runtime_error("Failed to open default audio device");
  }

  // Create context over device

  m_pContext = alcCreateContext(m_pDevice, nullptr);

  if (m_pContext == nullptr)
  {
    throw std::runtime_error("Failed to create context over default audio device");
  }

  alcMakeContextCurrent(m_pContext);

  // Set default listener position, velocity, and orientation

  alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set listener position");
  }

  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 1.0f);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set listener velocity");
  }

  std::array<ALfloat, 6> listenerOrientation = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
  alListenerfv(AL_ORIENTATION, listenerOrientation.data());
  alListener3f(AL_VELOCITY, 0.0f, 0.0f, 1.0f);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set listener orientation");
  }

  // Create an OpenAL source

  alGenSources(static_cast<ALuint>(1), &m_source);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to create source");
  }

  // Set source position, velocity, pitch, gain, and looping flag

  // Position
  alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set source position");
  }

  // Velocity
  alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set source velocity");
  }

  // Pitch
  alSourcef(m_source, AL_PITCH, 1.0);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set source pitch");
  };

  // Gain
  alSourcef(m_source, AL_GAIN, 1.0);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set source gain");
  }

  // Looping flag
  alSourcei(m_source, AL_LOOPING, AL_FALSE);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to set source looping flag");
  }
}

SoundPlayer::~SoundPlayer()
{
  destroy();
}


void SoundPlayer::destroy()
{
  // Free audio assets
  for (auto &[name, buffer]: m_buffers)
  {
    alDeleteBuffers(1, &(buffer.first));
    delete[] buffer.second;
  }

  // Free source
  alDeleteSources(1, &m_source);

  // Free audio context
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_pContext);

  // Close default audio device
  alcCloseDevice(m_pDevice);
}


void SoundPlayer::load(std::string fileName, std::string soundName)
{
  // Validate input

  if (soundName == "")
  {
    soundName = fileName;
  }

  if (m_buffers.find(soundName) != m_buffers.end())
  {
    throw std::runtime_error("The sound " + std::string(soundName) + "has already been loaded");
  }

  // Generate OpenAL audio buffer

  ALuint buffer = 0;
  alGenBuffers(1, &buffer);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to generate OpenAL audio buffer");
  }

  // Open wave file

  WaveInfo *pWaveFileInfo = nullptr;

  if ((pWaveFileInfo = WaveOpenFileForReading(fileName.c_str())) == nullptr)
  {
    throw std::runtime_error("Failed to open wave file");
  }

  // Seek to start of wave file

  if (WaveSeekFile(0, pWaveFileInfo) != 0)
  {
    throw std::runtime_error("Failed to seek to start of wave file");
  }

  // Read PCM samples from wave file

  uint32_t &dataSize = pWaveFileInfo->dataSize;

  char *pData = new char[static_cast<size_t>(dataSize)];
  if (pData == nullptr)
  {
    throw std::runtime_error("Failed to allocate memory for PCM samples");
  }

  if (WaveReadFile(pData, dataSize, pWaveFileInfo) != dataSize)
  {
    throw std::runtime_error("Failed to read wave file");
  }

  // Get audio format

  ALenum format = AL_NONE;
  
  short &numChannels = pWaveFileInfo->channels;
  short &bitDepth = pWaveFileInfo->bitsPerSample;

  bool invalidChannelNumber = false;
  switch (bitDepth)
  {
  case 8: {
    if (numChannels == 1)
    {
      format = AL_FORMAT_MONO8;
    }
    else if (numChannels == 2)
    {
      format = AL_FORMAT_STEREO8;
    }
    else
    {
      invalidChannelNumber = true;
    }

    break;
  }

  case 16: {
    if (numChannels == 1)
    {
      format = AL_FORMAT_MONO16;
    }
    else if (numChannels == 2)
    {
      format = AL_FORMAT_STEREO16;
    }
    else
    {
      invalidChannelNumber = true;
    }

    break;
  }

  default: {
    throw std::domain_error("Invalid bit depth");
    break;
  }
  }

  if (invalidChannelNumber)
  {
    throw std::domain_error("Invalid number of channels");
  }

  // Load PCM samples into OpenAL buffer

  int32_t &sampleRate = pWaveFileInfo->sampleRate;

  alBufferData(buffer, format, pData, dataSize, sampleRate);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to load PCM samples into OpenAL buffer");
  }

  // Close wave file

  WaveCloseFile(pWaveFileInfo);

  // Store information in class instance buffer
  
  m_buffers[soundName] = std::make_pair(buffer, pData);
}

void SoundPlayer::play(std::string soundName)
{
  // Validate input

  if (m_buffers.find(soundName) == m_buffers.end())
  {
    throw std::runtime_error("Sound not found");
  }

  // Load buffer into source 

  if (m_currentSound != soundName)
  {
    alSourcei(m_source, AL_BUFFER, m_buffers[soundName].first);
    if (alGetError() != AL_NO_ERROR)
    {
      throw std::runtime_error("Failed to load buffer into source");
    }
  }

  // Play the source

  alSourcePlay(m_source);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to play OpenAL source");
  }
}

bool SoundPlayer::isPlaying()
{
  ALint sourceState = 0;

  // TODO: Ensure player is never left in invalid state
  static_cast<void>(alGetError());

  alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
  if (alGetError() != AL_NO_ERROR)
  {
    throw std::runtime_error("Failed to get source state");
  }

  return (sourceState == AL_PLAYING);
}
