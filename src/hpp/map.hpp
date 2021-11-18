#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "entities.hpp"
#include "interactible.hpp"
#include "message.hpp"
#include "search_theta.hpp"
#include "tile.hpp"

namespace crow {
    class game_manager;

    struct event_trigger {
        float2e top_left;
        float2e bottom_right;
        void (crow::game_manager::* _event)();

        event_trigger(float2e _tl, float2e _br, void (crow::game_manager::* _e)())
                : top_left(_tl), bottom_right(_br), _event(_e) { }

        bool within_bounds(DirectX::XMFLOAT3 pos) {
            if (pos.x > top_left.x && pos.z > top_left.y &&
                pos.x < bottom_right.x && pos.z < bottom_right.y) {
                return true;
            }
            return false;
        }
    };

struct room {
  // how much to scale the width and length variables of the room when drawing
  // the minimap
  const float minimap_scale = 3.3f;
  unsigned int width = 25;
  unsigned int length = 15;
  unsigned int height = 50;
  bool has_player = false;
  bool has_ai = false;
  // a list of the live entities in this room
  std::vector<int> live_entities;
  float2e minimap_pos = {0.f, 0.f};
  // seconds of oxygen left in the room
  float oxygen = 50;
  // max oxygen in the room
  float oxygen_max = 50;
  int id = 0;
  crow::tile_map tiles;
  crow::theta_star pather;

  /*float3e cam_pos = float3e(0.f, 20.f, -2.f);
  float3e cam_rotation = float3e(-85.f, 0.f, 0.f);*/

  // contains all events that may be in this level
  std::vector<crow::event_trigger> event_triggers;

  // contains the entity index of every object that is in this room
  std::vector<size_t> object_indices;

  // the tile location and type of every object in this room
  // std::unordered_map<glm::uvec2, object_type> room_objects;
  std::vector<crow::interactible*> objects;

  // used in level generation only.
  // this vector is emptied while meshes are being generated for the level
  std::vector<DirectX::XMMATRIX> furniture_matrices; // matrix for the location of the meshes to be used
  std::vector<int> furniture_meshes; // mesh indices to be used
  std::vector<int> furniture_textures; // textures to be used for the meshes


  // may not be needed as doors will handle ajacencies
  // std::vector<std::shared_ptr<room>> neighbors;

  void generate_debug_collision_display();
  void load_entities(game_manager& state);
  void generate_tilemap();
  void initialize_pather();

  void update_room_doors(std::vector<ID3D11ShaderResourceView*>& textures, entities& entities);

  float2e get_tile_wpos(unsigned int const x, unsigned int const y);
  float2e get_tile_wpos(tile* const tile);
  tile* get_tile_at(float2e const pos);
  std::vector<float2e> get_path(float2e start, float2e goal);
  // checks if a room has a broken console
  // used to play the ambient noise of a broken console
  //        returns:
  //             0 = no console found
  //            -1 = working console found
  //             1 = broken console found
  int room::has_broken_console();
};

struct level {
  unsigned int x = 0;
  unsigned int y = 0;
  unsigned int id = 0;

  unsigned int room_length = 15;
  unsigned int room_width = 25;
  unsigned int room_height = 50;

  unsigned int starting_room = 0;
  unsigned int exit_room = 0;
  bool found_ai = false;
  // the amount of np units in this level to be processed 0 - 3
  int units = 0;
  
  player_interact* p_inter = nullptr;
  crow::room* selected_room = nullptr;
  // all valid rooms must have a valid ID that is greater than 0
  std::vector<std::vector<room>> rooms;
  std::vector<door> doors;
  // message to display on the screen. will be read by the game manager to
  // display a message on the top of the screen
  crow::message msg;
  // pointer to object that you are currently interacting with; only used for
  // objects that take time to interact with. do NOT use with anything instant
  crow::interactible* interacting;
  // oxygen console in this floor. if there is no console, or if the console is
  // not broken, then oxygen will not decrease
  crow::oxygen_console* oxygen_console;
  // pressure console in this floor. if there is no console, or if the consle is
  // not broken, then pressure will not decrease (increase?)
  crow::pressure_console* pressure_console;

  // loads a level from file
  void load_level(std::string filepath);

  // loads the hardcoded level with the specified id
  void load_level(crow::game_manager* state, int lv);

  void clean_level();

  // locates the default room and sets it
  // call this when loading a new level AFTER loading entities and BEFORE
  // updating the camera
  void select_default_room();

  // helper functions for level loading:
  // return is the index of the object in the respective room's object array

    int place_couch(room& r, float2e position, char orientation, int number);
    int place_table(room& r, float2e position, char orientation, int number);
    int place_crate1(room& r, float2e position, char orientation); // don't use .5
    // don't use .5 for position. takes up 3x3 space
    int place_crate2(room& r, float2e position, float rotation); 

    int place_crate3(room& r, float2e position, char orientation); // don't use .5
    int place_lightbox(room& r, float2e position, float rotation); // don't use .5
  
    // don't use .5 for position.
    // valid orientations: v (1x1), d/u (1x2), l/r (2x1)
    int place_barrel(room& r, float2e position, char orientation, int number);
    int place_serverbox(room& r, float2e position, char orientation);
    int place_electricbox(room& r, float2e position, char orientation);
    int place_bed(room& r, float2e position, char orientation);
    int place_chair(room& r, float2e position, char orientation); // don't use .5
};

}  // namespace crow