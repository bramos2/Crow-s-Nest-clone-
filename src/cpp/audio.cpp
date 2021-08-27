#include "../hpp/audio.hpp"

#include "../hpp/cross.hpp"

// TODO: There are so many clang-tidy warnings.

namespace crow::audio {

SoLoud::Soloud soloud;
SoLoud::WavStream bgm[NUM_BGM];
SoLoud::Wav sfx[NUM_SFX];
bool sound_loaded;

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
  sound_path.append("../../res/sfx/slap.wav");
  load_sfx(sound_path, 0);

  sound_path = crow::get_exe_path();
  sound_path.append("../../res/sfx/slap.wav");
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

}  // namespace crow::audio
