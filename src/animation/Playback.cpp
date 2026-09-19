#include "Playback.hpp"

void Playback::advance(double deltaTime)
{
    if (!paused)
    {
        animationTime += deltaTime;
    }
}

void Playback::togglePause()
{
    paused = !paused;
}

void Playback::restart()
{
    animationTime = 0.0;
    paused = false;
}

double Playback::getTime() const
{
    return animationTime;
}
