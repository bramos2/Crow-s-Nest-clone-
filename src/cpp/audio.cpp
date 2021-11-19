#include "../hpp/audio.hpp"

#include "../hpp/entities.hpp"

namespace crow {
    namespace audio {
        // float to multiply bgm by when bgs is playing
        const float bgs_mute_coefficient = 0.2f;
        // float to multiply bgs volume by since the sounds are very loud
        const float bgs_volume_coefficient = 0.4f;

        SoLoud::Soloud soloud;
        SoLoud::WavStream bgm[BGM::BGM_COUNT];
        SoLoud::Wav sfx[SFX::SFX_COUNT];
        bool sound_loaded;
        std::vector<timed_audio> audio_timers;
        // handle of currently playing bgm
        int bgm_handle;
        // handle of currently playing bgs
        int bgs_handle;
        // volume of ALL sounds
        float all_volume;
        // volume of bgm ONLY (stacks with all_volume)
        float bgm_volume;
        // volume of sfx ONLY (stacks with all_volume)
        float sfx_volume;
        int bgm_id;

        // all audio-related initialization here
        void initialize() {
            all_volume = 1;
            bgm_volume = 1;
            sfx_volume = 1;

            bgm_handle = -1;
            bgm_id = -1;
            bgs_handle = -1;
            soloud.init();
            load_all_sounds();
        }
        // all audio-related cleanup called here
        void cleanup() { soloud.deinit(); }
        
        int play_bgm(int id) {
            // handle duplicate bgm play calls
            if (bgm_id != id) {
                bgm_id = id;
            } else return bgm_handle;
            // stop previous song
            if (bgm_handle != -1) stop_bgm();
            // play song
            bgm_handle = soloud.play(bgm[id], bgm_volume);
            // prevents the bgm from dying just because there's too many sfx
            soloud.setProtectVoice(bgm_handle, 1);
            return bgm_handle;
        }

        int play_bgs(int id, bool reduce) {
            bgs_handle = soloud.play(sfx[id], sfx_volume * bgs_volume_coefficient);
            // prevents the bgm from dying just because there's too many sfx
            soloud.setProtectVoice(bgs_handle, 1);

            // decreases the volume of the bgm while bgs is playing
            if (reduce && bgm_handle != -1) {
                soloud.setVolume(bgm_handle, bgm_volume * bgs_mute_coefficient);
            }
            return bgs_handle;
        }

        void stop_bgm() {
            soloud.stop(bgm_handle);
            bgm_handle = -1;
            bgm_id = -1;
        }
        void stop_bgm(int handle) { soloud.stop(handle); }

        void stop_bgs() {
            soloud.stop(bgs_handle);
            bgs_handle = -1;
            // revert the bgm volume
            soloud.setVolume(bgm_handle, bgm_volume);
        }

        int play_sfx(int id) {
          int voice = soloud.play(sfx[id], sfx_volume);
          return voice;
        }

        int play_sfx3d(int id, float4x4_a& sfx_pos, view_t& camera,
                       float max_volume) {
          // figure out the distance between the camera and object
          float dist_x = sfx_pos[3][0] - camera.view_mat[3].x;
          float dist_y = sfx_pos[3][1] - camera.view_mat[3].y;
          float dist_z = sfx_pos[3][2] - camera.view_mat[3].z;
          float volume = (dist_x * dist_x) + (dist_y * dist_y) + (dist_z * dist_z);
          volume = crow::clampf(VOLUME_MAXDIST_SQUARED / volume, 0.0f, max_volume) * sfx_volume;
          float pan = crow::clampf(dist_x / PAN_MAXDIST, -1.0f, 1.0f);
          // finally, play the sound
          int voice = soloud.play(sfx[id], volume, pan);
          // int voice = soloud.play3d(sfx[id], dist_x, dist_y, dist_z, 0, 0, 0,
          // volume);

          return voice;
        }

        void load_all_sounds() {
          // loading sounds one by one
          load_sfx("res/sfx/footstep00.ogg", SFX::FOOTSTEP_WORKER);
          load_sfx("res/sfx/Menu_Select.wav", SFX::MENU_OK);
          load_sfx("res/sfx/Keyboard_Typing.wav", SFX::INTERACT);
          load_sfx("res/sfx/Opening_door.wav", SFX::DOOR_LOCK); // we could probably get an additional sound
          load_sfx("res/sfx/Opening_door.wav", SFX::DOOR_UNLOCK);
          load_sfx("res/sfx/Locking_Door.wav", SFX::DOOR_LOCKED); 
          load_bgs("res/sfx/Busted_electronics.wav", SFX::CONSOLE_BROKEN);
          load_bgs("res/sfx/Laboratory_digital_equipment.wav", SFX::CONSOLE_WORKING);
          load_sfx("res/sfx/Monster_Step_3.wav", SFX::ENEMY_FOOTSTEP);
          load_sfx("res/sfx/Monster_Scream.wav", SFX::ENEMY_APPEAR);
          load_sfx("res/sfx/Door_Bang.wav", SFX::ENEMY_ATTACK);
          load_sfx("res/sfx/Alarm_Blaring.wav", SFX::ALARM);

          // loading bgm
          load_bgm("res/bgm/cavethemeb4.ogg", BGM::NORMAL);
          load_bgm("res/bgm/ambientmain_0.ogg", BGM::TITLE);
          load_bgm("res/bgm/synth_wave_alex.mp3", BGM::DETECTED);
          load_bgm("res/bgm/breathe.mp3", BGM::GAME_OVER);
          load_bgm("res/bgm/game_overcredits.wav", BGM::GAME_WIN);

          sound_loaded = true;
        }

        bool load_sfx(char* path, int i) {
          sfx[i].load(path);
          sfx[i].setLooping(0);
          sfx[i].setInaudibleBehavior(false, true);
          // successfully loaded a sound, return true
          if (sfx[i].mSampleCount) return true;
          // sound is null, return false
          return false;
        }

        bool load_bgm(char* path, int i) {
          bgm[i].load(path);
          bgm[i].setLooping(1);
          bgm[i].setInaudibleBehavior(true, false);
          // successfully loaded a sound, return true
          if (bgm[i].mSampleCount) return true;
          // sound is null, return false
          return false;
        }

        bool load_bgs(char* path, int i) {
          sfx[i].load(path);
          sfx[i].setLooping(1);
          sfx[i].setInaudibleBehavior(true, false);
          // successfully loaded a sound, return true
          if (sfx[i].mSampleCount) return true;
          // sound is null, return false
          return false;
        }

        void update_volume() {
            soloud.setGlobalVolume(all_volume);
            if (bgm_handle != -1) soloud.setVolume(bgm_handle, bgm_volume);
            if (bgs_handle != -1) {
                soloud.setVolume(bgm_handle, bgm_volume * bgs_mute_coefficient);
                soloud.setVolume(bgs_handle, sfx_volume * bgs_volume_coefficient);
            }
        }

        void add_footstep_sound(float4x4_a* worker_position, float interval) {
          int index = crow::audio::audio_timers_index(crow::audio::worker_isnt_moving);

          if (index != -1) {
            crow::audio::audio_timers.erase(crow::audio::audio_timers.begin() + index);
          }

          crow::audio::audio_timers.push_back(
              crow::audio::timed_audio(crow::audio::worker_isnt_moving, worker_position,
                                       crow::audio::FOOTSTEP_WORKER, interval, -1));
        }

        void add_footstep_sound_e(float4x4_a* enemy_position, float interval) {
            // search for a previously existing instance of this sound
            int index = crow::audio::audio_timers_index(crow::audio::enemy_isnt_moving);

            // if found, give up, we are already playing the sound
            if (index != -1) {
                return;
            }

            // if not found, add new instance
            crow::audio::audio_timers.push_back(
                crow::audio::timed_audio(crow::audio::enemy_isnt_moving, enemy_position,
                                        crow::audio::ENEMY_FOOTSTEP, interval, -1));
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

        void clear_audio_timers() {
            while (audio_timers.size()) audio_timers.pop_back();
        }

        void update_audio_timers(crow::game_manager* state, float dt) {
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
              audio_timers[i].timer -= audio_timers[i].time_frame;
              audio_timers[i].loops_remaining--;

                // hotfix to prevent sounds from playing when the relevant actor is present
                // there are far more elegant ways to do this but there is no need for it
                if (audio_timers[i].escape_clause == &worker_isnt_moving) {
                    // check to see if the worker is in the room before playing sound
                    if (!state->current_level.selected_room->has_player) continue;
                }
                if (audio_timers[i].escape_clause == &enemy_isnt_moving) {
                    // check to see if the worker is in the room before playing sound
                    if (!state->current_level.selected_room->has_ai) continue;
                }

              // plays a 3d sound if the sound has a position, plays a regular (not-3d)
              // sound otherwise
              if (audio_timers[i].position)
                play_sfx3d(audio_timers[i].sound, *audio_timers[i].position,
                           state->view);
              else play_sfx(audio_timers[i].sound);
            }
          }
        }
        
        bool worker_isnt_moving(crow::game_manager* state) {
          if (state->entities.velocities[static_cast<size_t>(crow::entity::WORKER)].x == 0 &&
              state->entities.velocities[static_cast<size_t>(crow::entity::WORKER)].z == 0)
            return true;
          return false;
        }

        bool enemy_isnt_moving(crow::game_manager* state) {
          if (state->entities.velocities[static_cast<size_t>(crow::entity::SPHYNX)].x == 0 &&
              state->entities.velocities[static_cast<size_t>(crow::entity::SPHYNX)].z == 0)
            return true;
          return false;
        }

    }  // namespace audio
}  // namespace crow
