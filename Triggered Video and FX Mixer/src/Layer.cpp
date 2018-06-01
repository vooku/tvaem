#include "Layer.h"

Layer::Layer(int id)
    : id_(id), paused_(true)
{
    player_.setPixelFormat(OF_PIXELS_BGRA);
    player_.setLoopState(OF_LOOP_NORMAL);
}

Layer::Layer(int id, const std::string& filename)
    : Layer(id)
{
    reload(filename);
}

Layer::~Layer()
{
    player_.getTexture().unbind(id_);
    player_.closeMovie();
}

bool Layer::reload(const std::string& filename)
{
    player_.closeMovie();

    if (!player_.load(filename)) {
        ofLog(OF_LOG_ERROR, "Cannot load %s.", filename);
        return false;
    }

    return true;
}

void Layer::bind() {
    player_.getTexture().bindAsImage(id_, GL_READ_ONLY);
}

void Layer::play()
{
    paused_ = false;
    player_.setPaused(paused_);
}

void Layer::pause()
{
    paused_ = true;
    player_.setPaused(paused_);
}

void Layer::playPause()
{
    paused_ = !paused_;
    player_.setPaused(paused_);
}

bool Layer::isFrameNew()
{
    player_.update();
    return player_.isFrameNew(); 
}
