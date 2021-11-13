#include "../hpp/view.hpp"
#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
	void level::load_level(std::string filepath) {}

	//    up door position (default):  7, 14
	//  left door position (default):  0, 7
	//  down door position (default):  7, 0
	// right door position (default): 14, 7

	void level::load_level(crow::game_manager* state, int lv) {
		oxygen_console = nullptr;
		pressure_console = nullptr;
		/*pressure = pressure_max = 230;*/
		// TODO MAKE LEVEL TILE MAP BASED ON THE LEVEL'S ROOM DIMMENSIONS AND REMOVE EXTRA DATA FROM ROOM STRUCT
		switch (lv) {
		case 1: {
			x = 4;
			y = 3;
			id = 1;
			exit_room = 8;
			rooms.resize(y);
			for (auto& row : rooms) {
				row.resize(x);
			}
        // TODO MAKE LEVEL TILE MAP BASED ON THE LEVEL'S ROOM DIMMENSIONS AND REMOVE EXTRA DATA FROM ROOM STRUCT
			rooms[2][1].id = 1;
			crow::door* lv1_door1_u = new door(this);
			crow::door* lv1_door1_l = new door(this);
			crow::door* lv1_door1_r = new door(this, false);
			crow::door_panel* lv1_doorp1_r = new door_panel(this);
			{
				lv1_door1_l->set_tile('l');
				lv1_door1_u->set_tile('u');
				lv1_door1_r->set_tile('r');
				lv1_doorp1_r->set_tile('r'); lv1_doorp1_r->y += 2;
				lv1_door1_l->roomptr = lv1_door1_u->roomptr = lv1_door1_r->roomptr =
					&rooms[2][1];
				rooms[2][1].objects.push_back(lv1_door1_l);
				rooms[2][1].objects.push_back(lv1_door1_u);
				rooms[2][1].objects.push_back(lv1_door1_r);
				rooms[2][1].objects.push_back(lv1_doorp1_r);


				// door panel
				lv1_doorp1_r->door = lv1_door1_r;

				rooms[2][1].generate_tilemap();
				
				place_table(rooms[2][1], { 12.5f, 7.5f }, 'u', 1);
				place_table(rooms[2][1], { 16.5f, 7.5f }, 'u', 1);
				place_table(rooms[2][1], { 8.5f, 7.5f }, 'u', 1);

				// tutorial
				rooms[2][1].event_triggers.push_back(event_trigger({ -10, -7}, { 1, 7}, &game_manager::t_locked_door_message));
			}

			rooms[1][1].id = 2;
			crow::door* lv1_door2_d = new door(this, true);
			crow::door_panel* lv1_doorp2_d = new door_panel(this);
			{
				lv1_door2_d->set_tile('d');
				lv1_doorp2_d->set_tile('d'); lv1_doorp2_d->x += 2;
				lv1_door2_d->roomptr = &rooms[1][1];

				// neighbors
				lv1_door2_d->neighbor = lv1_door1_u;
				lv1_door1_u->neighbor = lv1_door2_d;

				lv1_doorp2_d->door = lv1_door2_d;

				rooms[1][1].objects.push_back(lv1_door2_d);
				rooms[1][1].objects.push_back(lv1_doorp2_d);

				rooms[1][1].generate_tilemap();

				// i hope you like boxes w
				place_lightbox(rooms[1][1], { 1, 14 }, 0);
				place_lightbox(rooms[1][1], { 10, 14 }, 0);
				place_lightbox(rooms[1][1], { 11, 14 }, 30);
				int b1 = place_lightbox(rooms[1][1], { 11, 14 }, 0);

				place_lightbox(rooms[1][1], { 14, 7 }, 50);

				place_lightbox(rooms[1][1], { 24, 14 }, 0);
				place_lightbox(rooms[1][1], { 23, 14 }, 0);
				place_lightbox(rooms[1][1], { 24, 13 }, 0);
				place_lightbox(rooms[1][1], { 3, 2 }, 0);

				place_lightbox(rooms[1][1], { 22, 4 }, 0);
				place_lightbox(rooms[1][1], { 22, 5 }, 0);
				place_lightbox(rooms[1][1], { 21, 4 }, 15);

				// move this thing up
				float4x4_a* _b1 = (float4x4_a*)&rooms[1][1].furniture_matrices[b1];
				(*_b1)[3][1] += 1;
				(*_b1)[3][2] -= 0.5f;
			}

			rooms[2][0].id = 3;
			crow::door* lv1_door3_r = new door(this);
			{
				lv1_door3_r->set_tile('r');
				lv1_door3_r->roomptr = &rooms[2][0];

				// neighbors
				lv1_door3_r->neighbor = lv1_door1_l;
				lv1_door1_l->neighbor = lv1_door3_r;

				rooms[2][0].objects.push_back(lv1_door3_r);

				// spawn the worker here
				rooms[2][0].object_indices.push_back(0);

				rooms[2][0].generate_tilemap();
				
				place_couch(rooms[2][0], { 9.5f, 7.5f }, 'r', 1);

				place_table(rooms[2][0], { 22.5f, 2.5f }, 'u', 1);
				place_table(rooms[2][0], { 20.5f, 7.5f }, 'r', 1);
				place_table(rooms[2][0], { 22.5f, 10.5f }, 'u', 1);

				// tutorial
				rooms[2][0].event_triggers.push_back(event_trigger({ -1, -1}, { 1, 1}, &game_manager::t_first_control_message));
			}

			rooms[2][2].id = 4;
			crow::door* lv1_door4_l = new door(this);
			crow::door* lv1_door4_u = new door(this);
			crow::door* lv1_door4_r = new door(this);
			{
				lv1_door4_l->set_tile('l');
				lv1_door4_u->set_tile('u');
				lv1_door4_r->set_tile('r');
				lv1_door4_l->roomptr = lv1_door4_u->roomptr = lv1_door4_r->roomptr =
					&rooms[2][2];

				// neighbors
				lv1_door4_l->neighbor = lv1_door1_r;
				lv1_door1_r->neighbor = lv1_door4_l;

				rooms[2][2].objects.push_back(lv1_door4_l);
				rooms[2][2].objects.push_back(lv1_door4_u);
				rooms[2][2].objects.push_back(lv1_door4_r);

				rooms[2][2].generate_tilemap();

				place_table(rooms[2][2], { 14.5f, 3.5f }, 'd', 0);
				place_table(rooms[2][2], { 17.5f, 4.5f }, 'r', 0);
				place_table(rooms[2][2], { 17.5f, 8.5f }, 'r', 1);

				place_chair(rooms[2][2], { 14, 2 }, 'u');
				place_chair(rooms[2][2], { 19, 4 }, 'l');

				place_chair(rooms[2][2], { 21, 14 }, 'r');
			}

			rooms[2][3].id = 5;
			crow::door* lv1_door5_l = new door(this);
			crow::door* lv1_door5_u = new door(this);
			{
				lv1_door5_l->set_tile('l');
				lv1_door5_u->set_tile('u');
				lv1_door5_l->roomptr = lv1_door5_u->roomptr = &rooms[2][3];

				// neighbors
				lv1_door5_l->neighbor = lv1_door4_r;
				lv1_door4_r->neighbor = lv1_door5_l;

				rooms[2][3].objects.push_back(lv1_door5_l);
				rooms[2][3].objects.push_back(lv1_door5_u);

				rooms[2][3].generate_tilemap();

				place_electricbox(rooms[2][3], { 23.5f, 13.5f }, 'd');
				place_serverbox(rooms[2][3], { 3.5f, 13.5f }, 'd');
				place_serverbox(rooms[2][3], { 7.5f, 13.5f }, 'd');

				place_serverbox(rooms[2][3], { 17.5f, 13.5f }, 'd');
				place_serverbox(rooms[2][3], { 21.5f, 13.5f }, 'd');

				place_table(rooms[2][3], { 19.5f, 4.5f }, 'u', 1);
				place_chair(rooms[2][3], { 21, 3 }, 'u');
				place_chair(rooms[2][3], { 18, 6 }, 'l');

			}

			rooms[1][3].id = 6;
			crow::door* lv1_door6_l = new door(this);
			crow::door* lv1_door6_u = new door(this);
			crow::door* lv1_door6_d = new door(this);
			{
				lv1_door6_l->set_tile('l');
				lv1_door6_u->set_tile('u');
				lv1_door6_d->set_tile('d');
				lv1_door6_l->roomptr = lv1_door6_u->roomptr = lv1_door6_d->roomptr =
					&rooms[1][3];

				// neighbors
				lv1_door6_d->neighbor = lv1_door5_u;
				lv1_door5_u->neighbor = lv1_door6_d;

				rooms[1][3].objects.push_back(lv1_door6_l);
				rooms[1][3].objects.push_back(lv1_door6_u);
				rooms[1][3].objects.push_back(lv1_door6_d);

				rooms[1][3].generate_tilemap();

				
				place_table(rooms[1][3], { 5.5f, 2.5f }, 'u', 1);
				place_table(rooms[1][3], { 9.5f, 2.5f }, 'u', 1);
				place_table(rooms[1][3], { 13.5f, 2.5f }, 'u', 1);
				place_table(rooms[1][3], { 17.5f, 2.5f }, 'u', 1);

				place_table(rooms[1][3], { 5.5f, 11.5f }, 'u', 1);
				place_table(rooms[1][3], { 9.5f, 11.5f }, 'u', 1);
				place_table(rooms[1][3], { 13.5f, 11.5f }, 'u', 1);
				place_table(rooms[1][3], { 17.5f, 11.5f }, 'u', 1);

				int b1 = place_table(rooms[1][3], { 4.5f, 5.5f }, 'r', 1);
				int b2 = place_table(rooms[1][3], { 4.5f, 9.5f }, 'l', 1);
				
				// squish these so that it looks slightly nicer
				float4x4_a* _b1 = (float4x4_a*)&rooms[1][3].furniture_matrices[b1];
				float4x4_a* _b2 = (float4x4_a*)&rooms[1][3].furniture_matrices[b2];
				
				(*_b1)[2][2] *= 0.875f;
				(*_b2)[2][2] *= 0.875f;
				(*_b1)[3][2] -= 0.25f;
				(*_b2)[3][2] -= 0.675f;
			}

			rooms[0][3].id = 7;
			crow::door* lv1_door7_d = new door(this);
			crow::door_panel* lv1_doorp9_l = new door_panel(this);
			{
				lv1_door7_d->set_tile('d');
				lv1_door7_d->roomptr = &rooms[0][3];
				lv1_doorp9_l->set_tile('u'); lv1_doorp9_l->x += 8;

				// neighbors
				lv1_door7_d->neighbor = lv1_door6_u;
				lv1_door6_u->neighbor = lv1_door7_d;

				rooms[0][3].objects.push_back(lv1_door7_d);
				rooms[0][3].objects.push_back(lv1_doorp9_l);

				// spawn the "sphynx" here
				rooms[0][3].object_indices.push_back(1);

				rooms[0][3].generate_tilemap();

				place_crate1(rooms[0][3], { 12, 12 }, 'h');

			}

			rooms[1][2].id = 8;
			crow::door* lv1_door8_u = new door(this);
			crow::door* lv1_door8_r = new door(this);
			crow::door* lv1_door8_d = new door(this);
			{
				lv1_door8_u->set_tile('u');
				lv1_door8_r->set_tile('r');
				lv1_door8_d->set_tile('d');
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

				rooms[1][2].generate_tilemap();

				place_crate2(rooms[1][2], { 18, 11 }, 0);
				place_crate2(rooms[1][2], {  6, 11 }, 0);
				place_crate2(rooms[1][2], { 18,  3 }, 0);
				place_crate2(rooms[1][2], {  6,  3 }, 0);

			}

			rooms[0][2].id = 9;
			crow::door* lv1_door9_l = new door(this, false);
			crow::door* lv1_door9_d = new door(this);
			{
				lv1_door9_l->set_tile('l');
				lv1_door9_d->set_tile('d');
				lv1_door9_l->roomptr = lv1_door9_d->roomptr = &rooms[0][2];

				// neighbors
				lv1_door9_d->neighbor = lv1_door8_u;
				lv1_door8_u->neighbor = lv1_door9_d;

				// ???!???
				lv1_doorp9_l->door = lv1_door9_l;

				rooms[0][2].objects.push_back(lv1_door9_l);
				rooms[0][2].objects.push_back(lv1_door9_d);

				rooms[0][2].generate_tilemap();
				
				place_table(rooms[0][2], { 10.5f, 5.5f }, 'u', 1);
				place_table(rooms[0][2], { 14.5f, 5.5f }, 'u', 1);
				place_table(rooms[0][2], { 10.5f, 7.5f }, 'd', 1);
				place_table(rooms[0][2], { 14.5f, 7.5f }, 'd', 1);
				
				place_chair(rooms[0][2], { 8, 6 }, 'r');
				place_chair(rooms[0][2], { 8, 7 }, 'r');

				place_chair(rooms[0][2], { 17, 6 }, 'l');
				place_chair(rooms[0][2], { 17, 7 }, 'l');
				
				place_chair(rooms[0][2], {  9, 4 }, 'u');
				place_chair(rooms[0][2], { 11, 4 }, 'u');
				place_chair(rooms[0][2], { 13, 4 }, 'u');
				place_chair(rooms[0][2], { 15, 4 }, 'u');

				place_chair(rooms[0][2], { 10, 9 }, 'd');
				place_chair(rooms[0][2], { 12, 9 }, 'd');
				place_chair(rooms[0][2], { 14, 9 }, 'd');
				place_chair(rooms[0][2], { 16, 9 }, 'd');



			}

			rooms[0][1].id = 10;
			crow::door* lv1_door10_l = new door(this);
			crow::door* lv1_door10_r = new door(this);
			{
				lv1_door10_l->set_tile('l');
				lv1_door10_r->set_tile('r');
				lv1_door10_l->roomptr = lv1_door10_r->roomptr = &rooms[0][1];

				// neighbors
				lv1_door10_r->neighbor = lv1_door9_l;
				lv1_door9_l->neighbor = lv1_door10_r;

				rooms[0][1].objects.push_back(lv1_door10_l);
				rooms[0][1].objects.push_back(lv1_door10_r);

				rooms[0][1].generate_tilemap();
				
				place_chair(rooms[0][1], {  0, 14 }, 'd');
				place_chair(rooms[0][1], {  6, 14 }, 'r');
				place_chair(rooms[0][1], {  7, 14 }, 'u');
				place_chair(rooms[0][1], { 12, 14 }, 'r');
				place_chair(rooms[0][1], { 10, 14 }, 'd');
				place_chair(rooms[0][1], { 18, 14 }, 'd');
				place_chair(rooms[0][1], { 24, 14 }, 'l');

				place_chair(rooms[0][1], {  6, 0 }, 'u');
				place_chair(rooms[0][1], {  9, 0 }, 'r');
				place_chair(rooms[0][1], { 16, 0 }, 'r');
				place_chair(rooms[0][1], { 17, 0 }, 'r');
				place_chair(rooms[0][1], { 19, 0 }, 'r');
				place_chair(rooms[0][1], { 20, 0 }, 'l');
				place_chair(rooms[0][1], { 21, 0 }, 'd');
				place_chair(rooms[0][1], { 25, 0 }, 'l');
			}

			rooms[0][0].id = 11;
			crow::door* lv1_door11_r = new door(this);
			crow::door* lv1_door11_d = new door(this);
			{
				lv1_door11_r->set_tile('r');
				lv1_door11_d->set_tile('d');
				lv1_door11_r->roomptr = lv1_door11_d->roomptr = &rooms[0][0];

				// neighbors
				lv1_door11_r->neighbor = lv1_door10_l;
				lv1_door10_l->neighbor = lv1_door11_r;

				rooms[0][0].objects.push_back(lv1_door11_r);
				rooms[0][0].objects.push_back(lv1_door11_d);

				rooms[0][0].generate_tilemap();
				
				place_serverbox(rooms[0][0], {  3.5f, 13.5f }, 'd');
				place_serverbox(rooms[0][0], {  7.5f, 13.5f }, 'd');
				place_serverbox(rooms[0][0], { 11.5f, 13.5f }, 'd');
				place_serverbox(rooms[0][0], { 15.5f, 13.5f }, 'd');
				place_serverbox(rooms[0][0], { 19.5f, 13.5f }, 'd');
				place_serverbox(rooms[0][0], { 23.5f, 13.5f }, 'd');

				place_serverbox(rooms[0][0], { 0.5f, 11.5f }, 'r');
				place_serverbox(rooms[0][0], { 0.5f, 7.5f }, 'r');
				place_serverbox(rooms[0][0], { 0.5f, 3.5f }, 'r');

				place_electricbox(rooms[0][0], { 11.5f, 8.5f }, 'd');
				place_electricbox(rooms[0][0], { 13.5f, 8.5f }, 'd');
			}

			rooms[1][0].id = 12;
			crow::door* lv1_door12_u = new door(this);
			crow::exit* floor1_exit = new exit(state, this, lv);
			{
				lv1_door12_u->set_tile('u');
				lv1_door12_u->roomptr = &rooms[1][0];

				// neighbors
				lv1_door12_u->neighbor = lv1_door11_d;
				lv1_door11_d->neighbor = lv1_door12_u;

				rooms[1][0].objects.push_back(lv1_door12_u);

				floor1_exit->set_tile('l');
				rooms[1][0].objects.push_back(floor1_exit);

				rooms[1][0].generate_tilemap();
				
				place_couch(rooms[1][0], { 16.5f, 7.5f }, 'l', 1);

				place_crate2(rooms[1][0], { 16, 1 }, 0);

				place_barrel(rooms[1][0], { 12, 4 }, 'v', 1);
				place_barrel(rooms[1][0], { 10, 4 }, 'l', 0);
				place_crate2(rooms[1][0], {  7, 12 }, 12);
				place_crate2(rooms[1][0], { 22, 2 }, 30);

				place_crate2(rooms[1][0], { 23,  9 }, 0);
				place_crate2(rooms[1][0], { 23, 12 }, 0);
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
			crow::door* door1 = new door(this, false);
			crow::door_panel* door1p = new door_panel(this);
			//crow::oxygen_console* oxy = new crow::oxygen_console(this);
			crow::pressure_console* press = new crow::pressure_console(this);
			{
				door1->set_tile('r');
				door1p->set_tile(24, 9);
				door1->roomptr = &rooms[0][0];
				rooms[0][0].objects.push_back(door1p);
				rooms[0][0].objects.push_back(door1);

				door1p->door = door1;
				//door1->panel = door1p;

				//rooms[3][1].objects.push_back(oxy);
				//oxygen_console = oxy;
				//oxy->set_tile('d');

				// worker start position
				rooms[0][0].object_indices.push_back(0);

				// here is the damn issue
				rooms[0][0].generate_tilemap();
			}

			rooms[0][1].id = 2;
			crow::door* door2 = new door(false);
			crow::door* door2_2 = new door();
			{
				door2->set_tile('l');
				door2_2->set_tile('d');

				door2->neighbor = door1;
				door1->neighbor = door2;

				door2->roomptr = door2_2->roomptr = &rooms[0][1];
				rooms[0][1].objects.push_back(door2);
				rooms[0][1].objects.push_back(door2_2);

				rooms[0][1].generate_tilemap();

				// build level design (invisible)
				for (int i = 5; i < 10; i++) {
					//rooms[0][1].tiles.map[i][7]->is_open = false;
					rooms[0][1].tiles.map[7][i]->is_open = false;
				}
				// rooms[0][1].initialize_pather();
			}

			rooms[1][1].id = 3;
			crow::door* door3 = new door();
			crow::door* door3_2 = new door();
			{
				door3->set_tile('u');
				door3_2->set_tile('d');

				door3->neighbor = door2_2;
				door2_2->neighbor = door3;

				door3->roomptr = door3_2->roomptr = &rooms[1][1];
				rooms[1][1].objects.push_back(door3);
				rooms[1][1].objects.push_back(door3_2);

				rooms[1][1].generate_tilemap();

				// build level design (invisible)
				for (int i = 4; i < 11; i++) {
					rooms[1][1].tiles.map[i][9]->is_open = false;
				}
			}

			rooms[2][1].id = 4;
			crow::door* door4 = new door();
			crow::door* door4_2 = new door();
			crow::door* door4_3 = new door();
			crow::door* door4_4 = new door();
			{
				door4->set_tile('u');
				door4_2->set_tile('l');
				door4_3->set_tile('d');
				door4_4->set_tile('r');

				door4->neighbor = door3_2;
				door3_2->neighbor = door4;

				door4->roomptr = door4_2->roomptr = door4_3->roomptr =
					door4_4->roomptr = &rooms[2][1];
				rooms[2][1].objects.push_back(door4);
				rooms[2][1].objects.push_back(door4_2);
				rooms[2][1].objects.push_back(door4_3);
				rooms[2][1].objects.push_back(door4_4);

				rooms[2][1].generate_tilemap();
			}

			rooms[2][0].id = 5;
			crow::door* door5 = new door();
			{
				door5->set_tile('r');

				door5->neighbor = door4_2;
				door4_2->neighbor = door5;

				door5->roomptr = &rooms[2][0];
				rooms[2][0].objects.push_back(door5);
				// pushing ai index for demostrations
				rooms[2][0].object_indices.push_back(1);

				rooms[2][0].generate_tilemap();
			}

			rooms[3][1].id = 6;
			crow::door* door6 = new door();
			{
				press->roomptr = &rooms[3][1];
				rooms[3][1].objects.push_back(press);
				pressure_console = press;
				press->set_tile('d');

				door6->set_tile('u');

				door6->neighbor = door4_3;
				door4_3->neighbor = door6;

				door6->roomptr = &rooms[3][1];
				rooms[3][1].objects.push_back(door6);
				rooms[3][1].generate_tilemap();
			}

			rooms[2][2].id = 7;
			crow::door* door7 = new door();
			crow::door* door7_2 = new door();
			{
				door7->set_tile('l');
				door7_2->set_tile('r');

				door7->neighbor = door4_4;
				door4_4->neighbor = door7;

				door7->roomptr = door7_2->roomptr = &rooms[2][2];
				rooms[2][2].objects.push_back(door7);
				rooms[2][2].objects.push_back(door7_2);

				rooms[2][2].generate_tilemap();

				// build level design (invisible)
				for (int i = 8; i < 17; i++) {
					rooms[2][2].tiles.map[7][i]->is_open = false;
				}
			}

			rooms[2][3].id = 8;
			crow::door* door8 = new door();
			crow::exit* floor0_exit = new exit(state, this, lv);
			{
				door8->set_tile('l');

				door8->neighbor = door7_2;
				door7_2->neighbor = door8;

				door8->roomptr = &rooms[2][3];
				rooms[2][3].objects.push_back(door8);

				floor0_exit->set_tile('r');
				rooms[2][3].objects.push_back(floor0_exit);

				rooms[2][3].generate_tilemap();
			}
			break;
		}

		// setting the player bool on the starting room, should be done when loading
		// any level
		found_ai = false;
		for (auto& i : rooms) {
			for (auto& j : i) {
				// automatically locks all doors connected to locked doors
				for (int k = 0; k < j.objects.size(); k++) {
					if (j.objects[k]->type == crow::object_type::DOOR) {
						crow::door* d = (crow::door*)(j.objects[k]);

						if (!d->neighbor) {
							printf("error! Found an incorrectly configured door!");
							continue;
						}

						if (!d->is_active) d->neighbor->is_active = false;
					}
				}

				// properly set the starting room variables
				for (int k = 0; k < j.object_indices.size(); k++) {
					if (j.object_indices[k] == 0) {
						this->starting_room = j.id;
						j.has_player = true;
						break;
					}
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
				if (j.id != 0) {
					j.initialize_pather();
				}
			}
		}
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

	void level::clean_level() {
		for (auto& i : rooms) {
			for (auto& j : i) {
				// de-allocate dynamic memory
				for (int k = 0; k < j.objects.size(); k++) {
					delete j.objects[k];
				}

				// no objects here
				j.objects.resize(0);
				j.object_indices.resize(0);
			}
			i.resize(0);
		}

		rooms.resize(0);
	}

}  // namespace crow
