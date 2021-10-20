#include "../hpp/view.hpp"
#include "../hpp/map.hpp"

namespace crow {
void level::load_level(std::string filepath) {}

//    up door position (default):  7, 0
//  left door position (default):  0, 7
//  down door position (default):  7, 14
// right door position (default): 14, 7

void level::load_level(lava::app* app, int lv) {
  switch (lv) {
    case 1: {
      x = 4;
      y = 3;
      id = 1;
      starting_room = 1;
      exit_room = 8;
      rooms.resize(y);
      for (auto& row : rooms) {
        row.resize(x);
      }

      rooms[2][1].id = 1;
      crow::door* lv1_door1_u = new door();
      crow::door* lv1_door1_l = new door();
      crow::door* lv1_door1_r = new door();
      {
        lv1_door1_l->set_tile(0, 7);
        lv1_door1_u->set_tile(7, 0);
        lv1_door1_r->set_tile(14, 7);
        lv1_door1_l->roomptr = lv1_door1_u->roomptr = lv1_door1_r->roomptr =
            &rooms[2][1];
        rooms[2][1].objects.push_back(lv1_door1_l);
        rooms[2][1].objects.push_back(lv1_door1_u);
        rooms[2][1].objects.push_back(lv1_door1_r);
        rooms[2][1].make_room_meshes(app);
        // spawn the worker here
        rooms[2][1].object_indices.push_back(0);

        // build level design (invisible)
        for (int i = 4; i < 11; i++) {
          rooms[2][1].tiles.map[i][4]->is_open = false;
          rooms[2][1].tiles.map[i][9]->is_open = false;
        }
      }

      rooms[1][1].id = 2;
      crow::door* lv1_door2_d = new door();
      {
        lv1_door2_d->set_tile(7, 14);
        lv1_door2_d->roomptr = &rooms[1][1];

        // neighbors
        lv1_door2_d->neighbor = lv1_door1_u;
        lv1_door1_u->neighbor = lv1_door2_d;

        rooms[1][1].objects.push_back(lv1_door2_d);
        rooms[1][1].make_room_meshes(app);
      }

      rooms[2][0].id = 3;
      crow::door* lv1_door3_r = new door();
      {
        lv1_door3_r->set_tile(7, 14);
        lv1_door3_r->roomptr = &rooms[2][0];

        // neighbors
        lv1_door3_r->neighbor = lv1_door1_l;
        lv1_door1_l->neighbor = lv1_door3_r;

        rooms[2][0].objects.push_back(lv1_door3_r);
        rooms[2][0].make_room_meshes(app);
      }

      rooms[2][2].id = 4;
      crow::door* lv1_door4_l = new door();
      crow::door* lv1_door4_u = new door();
      crow::door* lv1_door4_r = new door();
      {
        lv1_door4_l->set_tile(0, 7);
        lv1_door4_u->set_tile(7, 0);
        lv1_door4_r->set_tile(7, 14);
        lv1_door4_l->roomptr = lv1_door4_u->roomptr = lv1_door4_r->roomptr =
            &rooms[2][2];

        // neighbors
        lv1_door4_l->neighbor = lv1_door1_r;
        lv1_door1_r->neighbor = lv1_door4_l;

        rooms[2][2].objects.push_back(lv1_door4_l);
        rooms[2][2].objects.push_back(lv1_door4_u);
        rooms[2][2].objects.push_back(lv1_door4_r);
        rooms[2][2].make_room_meshes(app);
      }

      rooms[2][3].id = 5;
      crow::door* lv1_door5_l = new door();
      crow::door* lv1_door5_u = new door();
      {
        lv1_door5_l->set_tile(0, 7);
        lv1_door5_u->set_tile(7, 0);
        lv1_door5_l->roomptr = lv1_door5_u->roomptr = &rooms[2][3];

        // neighbors
        lv1_door5_l->neighbor = lv1_door4_r;
        lv1_door4_r->neighbor = lv1_door5_l;

        rooms[2][3].objects.push_back(lv1_door5_l);
        rooms[2][3].objects.push_back(lv1_door5_u);
        rooms[2][3].make_room_meshes(app);
      }

      rooms[1][3].id = 6;
      crow::door* lv1_door6_l = new door();
      crow::door* lv1_door6_u = new door();
      crow::door* lv1_door6_d = new door();
      {
        lv1_door6_l->set_tile(0, 7);
        lv1_door6_u->set_tile(7, 0);
        lv1_door6_d->set_tile(14, 7);
        lv1_door6_l->roomptr = lv1_door6_u->roomptr = lv1_door6_d->roomptr =
            &rooms[1][3];

        // neighbors
        lv1_door6_d->neighbor = lv1_door5_u;
        lv1_door5_u->neighbor = lv1_door6_d;

        rooms[1][3].objects.push_back(lv1_door6_l);
        rooms[1][3].objects.push_back(lv1_door6_u);
        rooms[1][3].objects.push_back(lv1_door6_d);
        rooms[1][3].make_room_meshes(app);
      }

      rooms[0][3].id = 7;
      crow::door* lv1_door7_d = new door();
      {
        lv1_door7_d->set_tile(14, 7);
        lv1_door7_d->roomptr = &rooms[0][3];

        // neighbors
        lv1_door7_d->neighbor = lv1_door6_u;
        lv1_door6_u->neighbor = lv1_door7_d;

        rooms[0][3].objects.push_back(lv1_door7_d);
        rooms[0][3].make_room_meshes(app);
      }

      rooms[1][2].id = 8;
      crow::door* lv1_door8_u = new door();
      crow::door* lv1_door8_r = new door();
      crow::door* lv1_door8_d = new door();
      {
        lv1_door8_u->set_tile(0, 7);
        lv1_door8_r->set_tile(14, 7);
        lv1_door8_d->set_tile(7, 14);
        lv1_door8_u->roomptr = lv1_door8_r->roomptr = lv1_door8_d->roomptr =
            &rooms[1][2];

        // neighbors
        lv1_door8_d->neighbor = lv1_door4_u;
        lv1_door4_u->neighbor = lv1_door8_d;
        lv1_door8_r->neighbor = lv1_door6_l;
        lv1_door6_l->neighbor = lv1_door8_r;

        rooms[1][2].objects.push_back(lv1_door8_u);
        rooms[1][2].objects.push_back(lv1_door8_r);
        rooms[1][2].objects.push_back(lv1_door8_d);
        rooms[1][2].make_room_meshes(app);
      }

      rooms[0][2].id = 9;
      crow::door* lv1_door9_l = new door();
      crow::door* lv1_door9_d = new door();
      {
        lv1_door9_l->set_tile(0, 7);
        lv1_door9_d->set_tile(7, 14);
        lv1_door9_l->roomptr = lv1_door9_d->roomptr = &rooms[0][2];

        // neighbors
        lv1_door9_d->neighbor = lv1_door8_u;
        lv1_door8_u->neighbor = lv1_door9_d;

        rooms[0][2].objects.push_back(lv1_door9_l);
        rooms[0][2].objects.push_back(lv1_door9_d);
        rooms[0][2].make_room_meshes(app);
      }

      rooms[0][1].id = 10;
      crow::door* lv1_door10_l = new door();
      crow::door* lv1_door10_r = new door();
      {
        lv1_door10_l->set_tile(0, 7);
        lv1_door10_r->set_tile(7, 14);
        lv1_door10_l->roomptr = lv1_door10_r->roomptr = &rooms[0][1];

        // neighbors
        lv1_door10_r->neighbor = lv1_door9_l;
        lv1_door9_l->neighbor = lv1_door10_r;

        rooms[0][1].objects.push_back(lv1_door10_l);
        rooms[0][1].objects.push_back(lv1_door10_r);
        rooms[0][1].make_room_meshes(app);
      }

      rooms[0][0].id = 11;
      crow::door* lv1_door11_r = new door();
      crow::door* lv1_door11_d = new door();
      {
        lv1_door11_r->set_tile(14, 7);
        lv1_door11_d->set_tile(7, 14);
        lv1_door11_r->roomptr = lv1_door11_d->roomptr = &rooms[0][0];

        // neighbors
        lv1_door11_r->neighbor = lv1_door10_l;
        lv1_door10_l->neighbor = lv1_door11_r;

        rooms[0][0].objects.push_back(lv1_door11_r);
        rooms[0][0].objects.push_back(lv1_door11_d);
        rooms[0][0].make_room_meshes(app);
      }

      rooms[1][0].id = 12;
      crow::door* lv1_door12_u = new door();
      crow::exit* floor1_exit = new exit(app, this, lv);
      {
        lv1_door12_u->set_tile(7, 0);
        lv1_door12_u->roomptr = &rooms[1][0];

        // neighbors
        lv1_door12_u->neighbor = lv1_door11_d;
        lv1_door11_d->neighbor = lv1_door12_u;

        rooms[1][0].objects.push_back(lv1_door12_u);
        floor1_exit->set_tile(0, 7);
        rooms[1][0].objects.push_back(floor1_exit);
        rooms[1][0].make_room_meshes(app);
      }

      break;
    }  // END OF FLOOR 1
    case 0:
    default:
      x = y = 5;
      id = 1;
      starting_room = 1;
      exit_room = 8;
      rooms.resize(y);
      for (auto& row : rooms) {
        row.resize(x);
      }

      rooms[0][0].id = 1;
      crow::door* door1 = new door(this);
      crow::door_panel* door1p = new door_panel(this);
      {
        door1->set_tile(14, 7);
        door1p->set_tile(14, 8);
        door1->roomptr = &rooms[0][0];
        rooms[0][0].objects.push_back(door1p);
        rooms[0][0].objects.push_back(door1);

        door1p->door = door1;
        door1->panel = door1p;

        rooms[0][0].make_room_meshes(app);
        // worker start position
        rooms[0][0].object_indices.push_back(0);

        // build level design (invisible)
        for (int i = 5; i < 14; i++) {
          rooms[0][0].tiles.map[i][0]->is_open = false;
          rooms[0][0].tiles.map[i][10]->is_open = false;
        }
      }

      rooms[0][1].id = 2;
      crow::door* door2 = new door();
      crow::door* door2_2 = new door();
      {
        door2->set_tile(0, 7);
        door2_2->set_tile(7, 14);

        door2->neighbor = door1;
        door1->neighbor = door2;

        door2->roomptr = door2_2->roomptr = &rooms[0][1];
        rooms[0][1].objects.push_back(door2);
        rooms[0][1].objects.push_back(door2_2);
        rooms[0][1].make_room_meshes(app);

        // build level design (invisible)
        for (int i = 5; i < 10; i++) {
          rooms[0][0].tiles.map[i][7]->is_open = false;
          rooms[0][0].tiles.map[7][i]->is_open = false;
        }
      }

      rooms[1][1].id = 3;
      crow::door* door3 = new door();
      crow::door* door3_2 = new door();
      {
        door3->set_tile(7, 0);
        door3_2->set_tile(7, 14);

        door3->neighbor = door2_2;
        door2_2->neighbor = door3;

        door3->roomptr = door3_2->roomptr = &rooms[1][1];
        rooms[1][1].objects.push_back(door3);
        rooms[1][1].objects.push_back(door3_2);
        rooms[1][1].make_room_meshes(app);

        // build level design (invisible)
        for (int i = 4; i < 11; i++) {
          rooms[1][1].tiles.map[i][4]->is_open = false;
          rooms[1][1].tiles.map[i][9]->is_open = false;
        }
      }

      rooms[2][1].id = 4;
      crow::door* door4 = new door();
      crow::door* door4_2 = new door();
      // crow::door* door4_3 = new door();
      crow::door* door4_4 = new door();
      {
        door4->set_tile(7, 0);
        door4_2->set_tile(0, 7);
        // door4_3->set_tile(7, 14);
        door4_4->set_tile(14, 7);

        door4->neighbor = door3_2;
        door3_2->neighbor = door4;

        door4->roomptr = door4_2->roomptr =  // door4_3->roomptr =
            door4_4->roomptr = &rooms[2][1];
        rooms[2][1].objects.push_back(door4);
        rooms[2][1].objects.push_back(door4_2);
        // rooms[2][1].objects.push_back(door4_3);
        rooms[2][1].objects.push_back(door4_4);
        rooms[2][1].make_room_meshes(app);
      }

      rooms[2][0].id = 5;
      crow::door* door5 = new door();
      {
        door5->set_tile(14, 7);

        door5->neighbor = door4_2;
        door4_2->neighbor = door5;

        door5->roomptr = &rooms[2][0];
        rooms[2][0].objects.push_back(door5);
        rooms[2][0].make_room_meshes(app);
        // pushing ai index for demostrations
        rooms[2][0].object_indices.push_back(1);
      }

      // rooms[3][1].id = 6;
      // crow::door* door6 = new door();
      //{
      //  door6->set_tile(7, 0);
      //
      //  door6->neighbor = door4_3;
      //  door4_3->neighbor = door6;
      //
      //  door6->roomptr = &rooms[3][1];
      //  rooms[3][1].objects.push_back(door6);
      //  rooms[3][1].make_room_meshes(app);
      //}

      rooms[2][2].id = 7;
      crow::door* door7 = new door();
      crow::door* door7_2 = new door();
      {
        door7->set_tile(0, 7);
        door7_2->set_tile(14, 7);

        door7->neighbor = door4_4;
        door4_4->neighbor = door7;

        door7->roomptr = door7_2->roomptr = &rooms[2][2];
        rooms[2][2].objects.push_back(door7);
        rooms[2][2].objects.push_back(door7_2);
        rooms[2][2].make_room_meshes(app);

        // build level design (invisible)
        for (int i = 2; i < 13; i++) {
          rooms[2][2].tiles.map[7][i]->is_open = false;
        }
      }

      rooms[2][3].id = 8;
      crow::door* door8 = new door();
      crow::exit* floor0_exit = new exit(app, this, lv);
      {
        door8->set_tile(0, 7);

        door8->neighbor = door7_2;
        door7_2->neighbor = door8;

        door8->roomptr = &rooms[2][3];
        rooms[2][3].objects.push_back(door8);

        floor0_exit->set_tile(14, 7);
        rooms[2][3].objects.push_back(floor0_exit);
        rooms[2][3].make_room_meshes(app);
      }

      break;
  }

  // setting the player bool on the starting room, should be done when loading
  // any level
  bool found_ai = false;
  for (auto& i : rooms) {
    for (auto& j : i) {
      if (static_cast<unsigned int>(j.id) == this->starting_room) {
        j.has_player = true;
        //return;
      }

      // initially will need to find where the ai is at
      if (!found_ai) {
        for (auto& inx : j.object_indices) {
          if (inx == static_cast<size_t>(crow::entity::SPHYNX)) {
            found_ai = true;
            j.has_ai = true;
          }
        }
      }
    }
  }
}

void level::change_level(lava::app* app, int lv) {
  // todo::most likely the stuff in this temp array has to be disposed of
  // somehow, so that will need to be taken care of.
  std::vector<lava::mesh::ptr> temp;
  // reset the level variable
  clean_level(temp);

  // after all clean up is done, it's time to start loading the next level
  if (lv - 1 == crow::final_level) {
    // we beat the game, so send us to the endgame sequence, whatever that may
    // be

    // todo::this
  } else {
    // load the next level
    load_level(app, lv);
  }
  // todo::call the load_entities method somehow (this must be before the lines
  // that load the camera)

  // auto-load the first room
  select_default_room();
  crow::update_room_cam(selected_room->cam_pos, selected_room->cam_rotation,
                        app->camera);
}

void level::select_default_room() {
  for (auto& i : rooms) {
    for (auto& j : i) {
      if (static_cast<unsigned int>(j.id) == this->starting_room) {
        selected_room = &j;
        return;
      }
    }
  }
}

}  // namespace crow