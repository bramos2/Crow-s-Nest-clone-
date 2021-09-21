#pragma once

#include <liblava/lava.hpp>

#include <iostream>

#define NOMINMAX
#include "../soloud/include/soloud.h"
#include "../soloud/include/soloud_wav.h"
#include "../soloud/include/soloud_wavstream.h"

#define NUM_BGM 1
#define NUM_SFX 2
// distance (squared) that sounds can furthest be heard from, used in the 3d
// audio formula
#define VOLUME_MAXDIST_SQUARED 10000
// distance (NOT squared) that sounds will most pan to
#define PAN_MAXDIST 150
// maximum volume sounds will generally play at
#define SOUND_MAX_VOLUME 1.5f

namespace crow::audio {
// enums
// each sound effect or bgm will have an id that's referenced by one of these
// two enums use the enums instead of the raw id to make playing sounds that
// much more intuitive.

// readable names for every bgm object in the game
enum BGM : int { TITLE = 1, DETECTED = 2 };
// readable names for every sfx object in the game
enum SFX : int { PLAYER_HIT = 1, BEAKER_BREAK = 2 };

// must be defined in order to play audio
extern SoLoud::Soloud soloud;

// holds every background sound object in the game
// the WavStream object doesn't hold the entire sound in memory, and instead
// streams the data on demand
extern SoLoud::WavStream bgm[NUM_BGM];

// holds every sound effect object in the game
extern SoLoud::Wav sfx[NUM_SFX];

// set to true when all sounds are loaded, should be false otherwise
extern bool sound_loaded;

// initialize everything necessary for audio
void initialize();

// de-initialize everything used by audio at shutdown
void cleanup();

// load every sound and background track in the game
void load_all_sounds();

// load into this id, the sfx at this path
bool load_sfx(std::string& path, int i);

// load into this id, the bgm at this path
bool load_bgm(std::string& path, int i);

// ************ playing functions ************ //

// play background audio
int play_bgm(int id);

// stop playing the bgm represented by the handle
void stop_bgm(int handle);

// a static, barebones play sound function
int play_sfx(int id);

// plays the sound with proper panning and volume
// glm::mat4 const sfx_pos = position matrix of the object emitting this sound
// lava::camera& const camera = currently in-use camera object
// float max_volume = multiplier for the maximum volume this sound can be heard
// at, defaults to SOUND_MAX_VOLUME
int play_sfx3d(int id, glm::mat4& sfx_pos, lava::camera& camera,
               float max_volume = SOUND_MAX_VOLUME);

}  // namespace crow::audio
