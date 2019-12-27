#pragma once
#include <cstdint>

namespace skinny {

typedef uint8_t midiNote;

static const int MAX_LAYERS = 8;
static const int MAX_EFFECTS = 5;
static const float MAX_7BITF = 127.0f;
static constexpr const char * VERSION = "0.6.2-alpha";
static const midiNote DEFAULT_MASTER_ALPHA_CONTROL = 16;

} // namespace skinny
