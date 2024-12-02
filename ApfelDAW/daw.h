#ifndef DAW_H
#define DAW_H

#include <vector>
#include <string>

struct Track {
    std::string fileName;
    int sampleRate;
    int channels;
    double duration;
    std::vector<float> audioData;
};

class DAW {
public:
    void addTrack(const std::string& filePath);
    void removeTrack(int index);
    std::vector<Track>& getTracks();

    void play();
    void playAll();
    void stop();
    bool isPlaying() const;

private:
    std::vector<Track> tracks;
    bool playing = false;

    void loadAudio(const std::string& filePath, Track& track);
};

#endif