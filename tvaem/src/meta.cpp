#include "meta.h"

const uint8_t MappableDescription::invalid_midi = 255;
const std::filesystem::path LayerDescription::invalid_path = {};

bool LayerDescription::fromXml(ofxXmlSettings & config) {
    path = config.getValue("path", invalid_path.string());
    blendMode = static_cast<Layer::BlendMode>(config.getValue("blendMode", static_cast<int>(Layer::BlendMode::Invalid)));
    for (int i = 0; i < config.getNumTags("midi"); i++) {
        midiMap.insert(config.getValue("midi", invalid_midi, i));
    }

    if (path.empty() || blendMode == Layer::BlendMode::Invalid || midiMap.count(invalid_midi)) {
        ofLog(OF_LOG_ERROR, "Layer description contains invalid values and will be skipped.");
        return false;
    }

    return true;
}

void LayerDescription::toXml(ofxXmlSettings& config) const {
    config.setValue("path", path.string());
    config.setValue("blendMode", static_cast<int>(blendMode));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}

bool EffectDescription::fromXml(ofxXmlSettings & config) {
    type = static_cast<Effect::Type>(config.getValue("type", static_cast<int>(Effect::Type::Invalid)));
    for (int i = 0; i < config.getNumTags("midi"); i++) {
        midiMap.insert(config.getValue("midi", invalid_midi, i));
    }

    if (type == Effect::Type::Invalid || midiMap.count(invalid_midi)) {
        ofLog(OF_LOG_ERROR, "Effect description contains invalid values and will be skipped.");
        return false;
    }

    return true;
}

void EffectDescription::toXml(ofxXmlSettings & config) const {
    config.setValue("type", static_cast<int>(type));
    for (auto midiNote : midiMap)
        config.addValue("midi", midiNote);
}


void SceneDescription::fromXml(ofxXmlSettings & config) {
    name = config.getValue("name", "");

    for (int i = 0; i < config.getNumTags("layer"); i++) {
        config.pushTag("layer", i);
        LayerDescription layer;
        if (layer.fromXml(config))
            layers.push_back(std::move(layer));
        config.popTag(); // layer
    }

    for (int i = 0; i < config.getNumTags("effect"); i++) {
        config.pushTag("effect", i);
        EffectDescription effect;
        if (effect.fromXml(config))
            effects.push_back(std::move(effect));
        config.popTag(); // effect
    }
}

void SceneDescription::toXml(ofxXmlSettings & config) const {
    config.addValue("name", name);

    for (int i = 0; i < layers.size(); i++) {
        config.addTag("layer");
        config.pushTag("layer", i);
        layers[i].toXml(config);
        config.popTag(); // layer
    }

    for (int i = 0; i < effects.size(); i++) {
        config.addTag("effect");
        config.pushTag("effect", i);
        effects[i].toXml(config);
        config.popTag(); // effect
    }
}

void ShowDescription::fromXml(ofxXmlSettings & config) {
    for (int i = 0; i < config.getNumTags("scene"); i++) {
        config.pushTag("scene", i);
        SceneDescription scene;
        scene.fromXml(config);
        scenes_.push_back(std::move(scene));
        config.popTag(); // scene
    }
    currentIdx_ = 0;
    if (!scenes_.empty())
        nextIdx_ = (currentIdx_ + 1) % scenes_.size();
    else
        nextIdx_ = 0;
}

void ShowDescription::toXml(ofxXmlSettings & config) const {
    for (int i = 0; i < scenes_.size(); i++) {
        config.addTag("scene");
        config.pushTag("scene", i);
        scenes_[i].toXml(config);
        config.popTag(); // scene
    }
}

ShowDescription & ShowDescription::operator++() {
    currentIdx_ = ++currentIdx_ % scenes_.size();
    nextIdx_ = ++nextIdx_ % scenes_.size();
    return *this;
}