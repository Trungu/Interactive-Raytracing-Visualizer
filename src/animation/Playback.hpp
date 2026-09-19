#pragma once

class Playback
{
private:
    double animationTime = 0.0;
    bool paused = false;

public:
    void advance(double deltaTime);
    void togglePause();
    void restart();

    double getTime() const;
};