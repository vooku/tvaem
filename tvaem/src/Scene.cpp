#include "Scene.h"

void Scene::reload(const SceneDescription & description)
{
    name_ = description.name;
    valid_ = true;

    layers_.resize(description.layers.size());

    for (int i = 0; i < layers_.size() && i < MAX_LAYERS; ++i) {
        bool reloadLayer = description.layers[i].valid; // do not load invalid description
        reloadLayer = reloadLayer && (!layers_[i] || description.layers[i].path.filename() != layers_[i]->getName());
        if (reloadLayer) {
            auto newLayer = std::make_unique<Layer>(i, description.layers[i].path, description.layers[i].midiMap);
            newLayer->setBlendMode(description.layers[i].blendMode);
            if (newLayer->isValid())
                layers_[i].reset(newLayer.release());
            else
                valid_ = false;
        }
    }

    for (const auto& effect : description.effects) {
        effects_.emplace(effect.type, effect.midiMap);
    }
}

void Scene::bindTextures()
{
    for (auto& layer : layers_) {
         if (layer)
            layer->bind();
    }
}

bool Scene::isFrameNew()
{
    bool newFrame = false;
    for (auto& layer : layers_) {
        if (layer)
            newFrame |= layer->isFrameNew();
    }
    return newFrame;
}

bool Scene::hasActiveFX() const
{
    return uniforms_.inverse || uniforms_.reducePalette || uniforms_.colorShift;
}

Scene::FoundMappables Scene::newMidiMessage(ofxMidiMessage & msg) {
    auto noteOn = msg.status == MIDI_NOTE_ON;
    auto noteOff = msg.status == MIDI_NOTE_OFF;
    int note = msg.pitch;

    if (!noteOn && !noteOff)
        return {};

    FoundMappables result;
    for (auto& layer : layers_) {
        if (layer && layer->containsMidiNote(note)) {
            if (noteOn) {
                layer->play();
                result.layers.insert({ layer->getId(), true });
            }
            else if (noteOff) {
                layer->pause();
                result.layers.insert({ layer->getId(), false });
            }
        }
    }

    for (auto& effect : effects_) {
        if (effect.second.containsMidiNote(note)) {
            if (noteOn) {
                effect.second.play();
                result.effects.insert({ effect.first, true });
            }
            else if (noteOff) {
                effect.second.pause();
                result.effects.insert({ effect.first, false });
            }
        }
    }

    return result;
}

void Scene::playPauseLayer(int idx) 
{
    if (idx < layers_.size() && layers_[idx])
        layers_[idx]->playPause();
}

void Scene::playPauseEffect(Effect::Type type)
{
    effects_.at(type).playPause();
}

void Scene::setupUniforms(ofShader& shader) const 
{
    uniforms_.nLayers = layers_.size();
    for (int i = 0; i < layers_.size(); ++i) {
        if (layers_[i]) {
            uniforms_.playing[i] = layers_[i]->isPlaying();
            uniforms_.dimensions[i] = { layers_[i]->getWidth(), layers_[i]->getHeight() };
            uniforms_.blendingModes[i] = static_cast<int>(layers_[i]->getBlendMode());
        }
        else {
            uniforms_.playing[i] = false;
        }
    }

    shader.setUniform1i("nLayers", uniforms_.nLayers);
    shader.setUniform1iv("playing", uniforms_.playing, uniforms_.nLayers);
    shader.setUniform2iv("dimensions", reinterpret_cast<int*>(uniforms_.dimensions), uniforms_.nLayers);
    shader.setUniform1iv("blendingModes", uniforms_.blendingModes, uniforms_.nLayers);

    auto it = effects_.find(Effect::Type::Inverse);
    if (it != effects_.end())
        uniforms_.inverse = it->second.isPlaying();
    it = effects_.find(Effect::Type::ReducePalette);
    if (it != effects_.end())
        uniforms_.reducePalette = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift);
    if (it != effects_.end())
        uniforms_.colorShift = it->second.isPlaying();
    it = effects_.find(Effect::Type::ColorShift2);
    if (it != effects_.end())
        uniforms_.colorShift2 = it->second.isPlaying();

    shader.setUniform1i("inverse", uniforms_.inverse);
    shader.setUniform1i("reducePalette", uniforms_.reducePalette);
    shader.setUniform1i("colorShift", uniforms_.colorShift);
    shader.setUniform1i("colorShift2", uniforms_.colorShift2);
}
