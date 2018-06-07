#pragma once

#include "OfxMidi.h"
#include "meta.h"
#include <unordered_map>

class Scene {
public:
    static const int maxLayers = 5;

    Scene(const SceneDescription& description);

    void bindTextures();
    bool isFrameNew();
    bool hasActiveFX() const;
    void newMidiMessage(ofxMidiMessage & msg);
    void playPauseLayer(int idx);

    void setupUniforms(ofShader& shader) const;

private:
    struct Uniforms {
        int nLayers;
        int active[maxLayers];
        glm::ivec2 dimensions[maxLayers];
        int blendingModes[maxLayers];
        bool inverse;
        bool reducePalette;
        int colorShift;
    } mutable uniforms_;

    std::vector<std::unique_ptr<Layer>> layers_;
    std::unordered_map<Effect::Type, Effect> effects_;

};

