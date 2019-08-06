#include "Show.h"
#include "Status.h"

Show::Show(int width, int height) :
    width_(width),
    height_(height),
    currentScene_(std::make_shared<Scene>())
{
#ifndef NDEBUG
    const auto shaderFile = "../../src/shaders/shader";
#else
    const auto shaderFile = "shaders/shader";
#endif

    shader_.load(shaderFile);
    if (!shader_.isLoaded()) {
        ofLog(OF_LOG_FATAL_ERROR, "Could not load shaders.");
        Status::instance().exit = true;
        return;
    }
}

void Show::draw()
{
    if (currentScene_->isFrameNew()) {
        shader_.begin();
        setupUniforms();
        currentScene_->bind();
        ofDrawRectangle(0, 0, width_, height_);
        currentScene_->unbind();
        shader_.end();
    }
}

Scene::FoundMappables Show::newMidiMessage(ofxMidiMessage& msg)
{
    auto result = currentScene_->newMidiMessage(msg);

    const auto noteOn = msg.status == MIDI_NOTE_ON;
    const auto noteOff = msg.status == MIDI_NOTE_OFF;
    const auto note = msg.pitch;
    const auto cc = msg.status == MIDI_CONTROL_CHANGE;

    if (cc && masterAlphaControl_ == msg.control) {
        uniforms_.masterAlpha_ = msg.value / 127.0f;
        return {};
    }

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (effects_[i]->getNote() == note) {
            if (noteOn) {
                effects_[i]->play();
                result.effects.emplace_back(i, true);
            }
            else if (noteOff) {
                effects_[i]->pause();
                result.effects.emplace_back(i, false);
            }
        }
    }

    return result;
}

bool Show::reload(const ShowDescription& description)
{
    masterAlphaControl_ = description.getAlphaControl();

    currentScene_->bind();

    shader_.begin();
    currentScene_->reload(description.currentScene());

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        const auto& effect = description.getEffects()[i];
        effects_[i].reset(new Effect(effect.type, effect.note));
    }

    shader_.end();
    currentScene_->unbind();

    return currentScene_->isValid();
}

void Show::playPauseEffect(int i)
{
    effects_[i]->playPause();
}

ScenePtr Show::getCurrentScene() const
{
    return currentScene_;
}

const Show::Effects& Show::getEffects() const
{
    return effects_;
}


midiNote Show::getAlphaControl() const
{
    return masterAlphaControl_;
}

float Show::getAlpha() const
{
    return uniforms_.masterAlpha_;
}

void Show::setAlphaControl(const midiNote & control)
{
    masterAlphaControl_ = control;
}

void Show::setupUniforms() const
{
    currentScene_->setupUniforms(shader_);

    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        uniforms_.fxTypes[i] = static_cast<int>(effects_[i]->type);
        uniforms_.fxPlaying[i] = effects_[i]->isPlaying();
    }

    shader_.setUniform1f("masterAlpha", uniforms_.masterAlpha_);
    shader_.setUniform2iv("screenSize", reinterpret_cast<int*>(&glm::ivec2{ width_, height_ }));

    shader_.setUniform1iv("fxTypes", uniforms_.fxTypes, MAX_EFFECTS);
    shader_.setUniform1iv("fxPlaying", uniforms_.fxPlaying, MAX_EFFECTS);
}

bool Show::hasActiveFX() const
{
    for (auto i = 0; i < MAX_EFFECTS; ++i) {
        if (uniforms_.fxPlaying[i]) {
            return true;
        }
    }

    return false;
}