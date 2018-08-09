#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class Gui;

class Scene {
public:
    typedef std::array<std::unique_ptr<Layer>, MAX_LAYERS> Layers;
    typedef std::unordered_map<Effect::Type, Effect> Effects;

    friend class Gui;

    void reload(const SceneDescription& description);
    void bindTextures();
    bool isFrameNew();
    bool hasActiveFX() const;

    /**
     * Return which layers / effects were (de)activated.
     */
    struct FoundMappables {
        std::unordered_map<int, bool> layers;
        std::unordered_map<Effect::Type, bool> effects;
    };
    FoundMappables newMidiMessage(ofxMidiMessage & msg);

    void playPauseLayer(int idx);
    void playPauseEffect(Effect::Type type);

    void setupUniforms(ofShader& shader) const;

    const auto& getName() const { return name_; }
    const auto& getLayers() const { return layers_; }
    const auto& getEffects() const { return effects_; }
    bool isValid() const { return valid_; }

private:
    struct Uniforms {
        int nLayers;
        int playing[MAX_LAYERS];
        glm::ivec2 dimensions[MAX_LAYERS];
        int blendingModes[MAX_LAYERS];
        bool inverse = false;
        bool reducePalette = false;
        bool colorShift = false;
        bool colorShift2 = false;
    } mutable uniforms_;

    std::string name_;
    Layers layers_;
    Effects effects_;
    bool valid_;

};
