#pragma once

#include <iostream>

#include "../hpp/game_manager.hpp"
#include "../hpp/view.hpp"


#include "../../ext/soloud/include/soloud.h"
#include "../../ext/soloud/include/soloud_wav.h"
#include "../../ext/soloud/include/soloud_wavstream.h"

// distance (squared) that sounds can furthest be heard from, used in the 3d
// audio formula
#define VOLUME_MAXDIST_SQUARED 10000.f
// distance (NOT squared) that sounds will most pan to
#define PAN_MAXDIST 55.f
// maximum volume sounds will generally play at
#define SOUND_MAX_VOLUME 1.5f

namespace crow {
    class game_manager;

namespace audio {
// enums
// each sound effect or bgm will have an id that's referenced by one of these
// two enums use the enums instead of the raw id to make playing sounds that
// much more intuitive.

// a struct that holds data for an object that plays audio at specific intervals
struct timed_audio {
  // function that when returns true, causes this instance of timed_audio to
  // self destruct
  // note that if nullptr is passed here then the object will always immediately
  // self-destruct
  bool (*escape_clause)(crow::game_manager* state);
  // position of the audio in world space.
  // if the audio is not to be played as 3d audio, set this to nullptr
  float4x4_a* position;
  // sound to play
  int sound;
  // every time_frame ms, play the sound associated
  float time_frame;
  // amount of times to play this sound. set to -1 to play indefinitely
  int loops_remaining;
  // timer to keep track of time elapsed
  float timer;

  // simple constructor that sets all of the variables
  timed_audio(bool (*_escape_clause)(crow::game_manager* state),
              float4x4_a* _position, int _sound, float _time_frame,
              int _loops_remaining) {
    escape_clause = _escape_clause;
    position = _position;
    sound = _sound;
    time_frame = _time_frame;
    loops_remaining = _loops_remaining;
    timer = _time_frame;
  }
};

// readable names for every bgm object in the game
enum BGM : int { NORMAL, TITLE, DETECTED, GAME_WIN, GAME_OVER, LEVEL_CLEAR, ESCAPE, BGM_COUNT };
// readable names for every sfx object in the game
enum SFX : int { 
    FOOTSTEP_WORKER,
    MENU_OK,
    INTERACT,
    DOOR_LOCK,
    DOOR_UNLOCK,
    DOOR_LOCKED,
    CONSOLE_BROKEN,
    CONSOLE_WORKING,
    ENEMY_FOOTSTEP,
    ENEMY_APPEAR,
    ENEMY_ATTACK,
    ALARM,
    SFX_COUNT
};

// must be defined in order to play audio
extern SoLoud::Soloud soloud;

// holds every background sound object in the game
// the WavStream object doesn't hold the entire sound in memory, and instead
// streams the data on demand
extern SoLoud::WavStream bgm[BGM::BGM_COUNT];

// holds every sound effect object in the game
extern SoLoud::Wav sfx[SFX::SFX_COUNT];

// set to true when all sounds are loaded, should be false otherwise
extern bool sound_loaded;

extern std::vector<timed_audio> audio_timers;

extern int bgm_handle;
extern int bgs_handle;
extern float all_volume;
extern float bgm_volume;
extern float sfx_volume;

// initialize everything necessary for audio
void initialize();

// de-initialize everything used by audio at shutdown
void cleanup();

// load every sound and background track in the game
void load_all_sounds();

// load into this id, the sfx at this path
bool load_sfx(char* path, int i);

// load into this id, the bgm at this path
bool load_bgm(char* path, int i);
// load into this id, the bgs at this path
// note that it loads it as a sfx, with bgm parameters
bool load_bgs(char* path, int i);

// ************ playing functions ************ //

// play background music
int play_bgm(int id);
// plau background sound
int play_bgs(int id, bool reduce = true);

// stop playing the bgm represented by the handle
void stop_bgm();
void stop_bgm(int handle);
// stops playing the bgs 
void stop_bgs();

// a static, barebones play sound function
int play_sfx(int id);

// plays the sound with proper panning and volume
// glm::mat4 const sfx_pos = position matrix of the object emitting this sound
// lava::camera& const camera = currently in-use camera object
// float max_volume = multiplier for the maximum volume this sound can be heard
// at, defaults to SOUND_MAX_VOLUME
int play_sfx3d(int id, float4x4_a& sfx_pos, view_t& camera,
               float max_volume = SOUND_MAX_VOLUME);

// updates the volume metadata
void update_volume();

void add_footstep_sound(float4x4_a* worker_position, float interval);
void add_footstep_sound_e(float4x4_a* enemy_position, float interval);

// checks if an audio timer with the passed in escape clause exists
// returns true if it exists
// returns false otherwise
bool audio_timers_includes(bool (*_escape_clause)(crow::game_manager* state));
// same as above, but returns the index of such
int audio_timers_index(bool (*_escape_clause)(crow::game_manager* state));

void clear_audio_timers();

// updates every single instance of an audio timer in the audio timer vector
void update_audio_timers(crow::game_manager* state, float dt);

// simply returns true if the worker isnt moving, and false if he is
bool worker_isnt_moving(crow::game_manager* state);
// ditto but for the ai
bool enemy_isnt_moving(crow::game_manager* state);

}  // namespace audio
}  // namespace crow
