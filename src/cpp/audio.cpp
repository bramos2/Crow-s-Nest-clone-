#include "../hpp/audio.hpp"

#include "../hpp/cross.hpp"
#include "../hpp/entities.hpp"

// TODO: There are so many clang-tidy warnings.

namespace crow::audio {

SoLoud::Soloud soloud;
SoLoud::WavStream bgm[NUM_BGM];
SoLoud::Wav sfx[NUM_SFX];
bool sound_loaded;
std::vector<timed_audio> audio_timers;

// all audio-related initialization here
void initialize() {
  soloud.init();
  load_all_sounds();
}
// all audio-related cleanup called here
void cleanup() { soloud.deinit(); }

int play_bgm(int id) {
  int voice = soloud.play(bgm[id]);
  // prevents the bgm from dying just because there's too many sfx
  soloud.setProtectVoice(voice, 1);
  return voice;
}

void stop_bgm(int handle) { soloud.stop(handle); }

int play_sfx(int id) {
  int voice = soloud.play(sfx[id]);
  return voice;
}

int play_sfx3d(int id, glm::mat4& sfx_pos, lava::camera& camera,
               float max_volume) {
  // figure out the distance between the camera and object
  float dist_x = sfx_pos[3][0] - camera.position.x;
  float dist_y = sfx_pos[3][1] - camera.position.y;
  float dist_z = sfx_pos[3][2] - camera.position.z;
  float volume = (dist_x * dist_x) + (dist_y * dist_y) + (dist_z * dist_z);
  volume = std::clamp(VOLUME_MAXDIST_SQUARED / volume, 0.0f, max_volume);
  float pan = std::clamp(dist_x / PAN_MAXDIST, -1.0f, 1.0f);
  // finally, play the sound
  int voice = soloud.play(sfx[id], volume, pan);
  // int voice = soloud.play3d(sfx[id], dist_x, dist_y, dist_z, 0, 0, 0,
  // volume);

  return voice;
}

void load_all_sounds() {
  // initialize the string that we use to load sfx
  std::string sound_path;

  // loading sounds one by one
  sound_path = crow::get_exe_path();
  sound_path.append("../../res/sfx/footstep00.ogg");
  load_sfx(sound_path, 0);

  sound_path = crow::get_exe_path();
  sound_path.append("../../res/sfx/bong_001.ogg");
  load_sfx(sound_path, 1);

  // loading bgm one by one (doesn't exist)

  printf("sounds loaded!\n");
  sound_loaded = true;
}

bool load_sfx(std::string& path, int i) {
  sfx[i].load(path.c_str());
  sfx[i].setLooping(0);
  sfx[i].setInaudibleBehavior(false, true);
  // successfully loaded a sound, return true
  if (sfx[i].mSampleCount) return true;
  // sound is null, return false
  return false;
}

bool load_bgm(std::string& path, int i) {
  bgm[i].load(path.c_str());
  bgm[i].setLooping(1);
  bgm[i].setInaudibleBehavior(true, false);
  // successfully loaded a sound, return true
  if (bgm[i].mSampleCount) return true;
  // sound is null, return false
  return false;
}

void add_footstep_sound(glm::mat4* worker_position, float interval) {
  int index = crow::audio::audio_timers_index(crow::audio::worker_isnt_moving);

  if (index != -1) {
    crow::audio::audio_timers.erase(crow::audio::audio_timers.begin() + index);
  }

  crow::audio::audio_timers.push_back(
      crow::audio::timed_audio(crow::audio::worker_isnt_moving, worker_position,
                               crow::audio::FOOTSTEP_WORKER, interval, -1));
}

bool audio_timers_includes(bool (*_escape_clause)(crow::game_manager* state)) {
  for (int i = 0; i < audio_timers.size(); i++) {
    if (audio_timers[i].escape_clause == _escape_clause) return true;
  }
  return false;
}

int audio_timers_index(bool (*_escape_clause)(crow::game_manager* state)) {
  for (int i = 0; i < audio_timers.size(); i++) {
    if (audio_timers[i].escape_clause == _escape_clause) return i;
  }
  return -1;
}

void update_audio_timers(crow::game_manager* state, lava::delta dt) {
  for (int i = 0; i < audio_timers.size(); i++) {
    // check for exit clause
    // will destroy this object if:
    //    1) audio has no properly defined self-destruct method
    //    2) audio has ran out of loops
    //    3) escape clause returns true
    if (!audio_timers[i].escape_clause ||
        audio_timers[i].loops_remaining == 0 ||
        audio_timers[i].escape_clause(state)) {
      audio_timers.erase(audio_timers.begin() + i);
      continue;
    }

    // inrement timer of this timer
    audio_timers[i].timer += dt;
    // check to see if we should play a sound because we hit the time threshold
    if (audio_timers[i].timer >= audio_timers[i].time_frame) {
      // plays a 3d sound if the sound has a position, plays a regular (not-3d)
      // sound otherwise
      if (audio_timers[i].position)
        play_sfx3d(audio_timers[i].sound, *audio_timers[i].position,
                   state->app->camera);
      else
        play_sfx(audio_timers[i].sound);
      audio_timers[i].timer -= audio_timers[i].time_frame;
      audio_timers[i].loops_remaining--;
    }
  }
}

bool worker_isnt_moving(crow::game_manager* state) {
  if (state->entities.velocities[static_cast<size_t>(crow::entity::WORKER)].x ==
          0 &&
      state->entities.velocities[static_cast<size_t>(crow::entity::WORKER)].z ==
          0)
    return true;
  return false;
}

}  // namespace crow::audio
