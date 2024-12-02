#include "daw.h"
#include <sndfile.h>
#include <portaudio.h>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <string>

static int audioCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void* userData) {
    auto* track = reinterpret_cast<Track*>(userData);
    auto* out = static_cast<float*>(outputBuffer);

    static size_t position = 0;
    size_t remaining = track->audioData.size() - position;
    size_t toCopy = std::min<size_t>(framesPerBuffer * track->channels, remaining);

    std::copy(track->audioData.begin() + position, track->audioData.begin() + position + toCopy, out);
    position += toCopy;

    if (toCopy < framesPerBuffer * track->channels) {
        std::fill(out + toCopy, out + framesPerBuffer * track->channels, 0.0f);
        position = 0;
        return paComplete;
    }

    return paContinue;
}

void DAW::addTrack(const std::string& filePath) {
    Track track;
    loadAudio(filePath, track);
    tracks.push_back(track);
}

void DAW::removeTrack(int index) {
    if (index >= 0 && index < tracks.size()) {
        tracks.erase(tracks.begin() + index);
    }
}

std::vector<Track>& DAW::getTracks() {
    return tracks;
}

void DAW::loadAudio(const std::string& filePath, Track& track) {
    SF_INFO sfInfo;
    SNDFILE* sndFile = sf_open(filePath.c_str(), SFM_READ, &sfInfo);
    if (!sndFile) {
        throw std::runtime_error("Failed to open audio file: " + filePath);
    }

    track.fileName = filePath;
    track.sampleRate = sfInfo.samplerate;
    track.channels = sfInfo.channels;
    track.duration = static_cast<double>(sfInfo.frames) / sfInfo.samplerate;

    size_t totalFrames = sfInfo.frames * sfInfo.channels;
    track.audioData.resize(totalFrames);

    sf_read_float(sndFile, track.audioData.data(), totalFrames);
    sf_close(sndFile);
}

void DAW::play() {
    if (tracks.empty() || playing) return;

    playing = true;
    Pa_Initialize();

    auto& track = tracks[0];
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, track.channels, paFloat32, track.sampleRate,
        256, audioCallback, &track);
    Pa_StartStream(stream);

    std::thread playbackThread([stream]() {
        while (Pa_IsStreamActive(stream) == 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Pa_CloseStream(stream);
        Pa_Terminate();
        });
    playbackThread.detach();
}

static int mixedAudioCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void* userData) {
    static size_t position = 0;
    auto* mixedAudio = static_cast<std::vector<float>*>(userData);
    auto* out = static_cast<float*>(outputBuffer);

    size_t toCopy = std::min<size_t>(framesPerBuffer * 2, mixedAudio->size() - position);
    std::copy(mixedAudio->begin() + position, mixedAudio->begin() + position + toCopy, out);
    position += toCopy;

    if (position >= mixedAudio->size()) {
        position = 0;
        return paComplete;
    }
    return paContinue;
}

void DAW::playAll() {
    if (tracks.empty() || playing) return;

    playing = true;
    Pa_Initialize();

    size_t maxSamples = 0;
    for (const auto& track : tracks) {
        maxSamples = std::max(maxSamples, track.audioData.size());
    }
    std::vector<float> mixedAudio(maxSamples, 0.0f);

    for (const auto& track : tracks) {
        for (size_t i = 0; i < track.audioData.size(); ++i) {
            mixedAudio[i] += track.audioData[i] / tracks.size();
        }
    }

    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, tracks[0].sampleRate, 256,
        mixedAudioCallback, &mixedAudio);

    Pa_StartStream(stream);

    std::thread playbackThread([stream]() {
        while (Pa_IsStreamActive(stream) == 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        Pa_CloseStream(stream);
        Pa_Terminate();
        });
    playbackThread.detach();
}


void DAW::stop() {
    playing = false;
    Pa_Terminate();
}

bool DAW::isPlaying() const {
    return playing;
}