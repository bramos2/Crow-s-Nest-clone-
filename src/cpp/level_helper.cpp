#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
	int level::place_couch(room& r, float2e position, char orientation, int number) {
		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'u': // up
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 6; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'd': // down
			furn_matrix = YrotationM(furn_matrix, 180);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 6; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, 90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 6; i++) {
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, -90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 6; i++) {
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::SOFA1 + number);
		r.furniture_textures.push_back(game_manager::texture_list::SOFA1 + number);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_table(room& r, float2e position, char orientation, int number) {
		const float d_scale = 0.2f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'u': // up
			furn_matrix = YrotationM(furn_matrix, -90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'd': // down
			furn_matrix = YrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'l': // left
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, 180);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::DESK1 + number);
		r.furniture_textures.push_back(game_manager::texture_list::DESK12 + number / 2);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_crate1(room& r, float2e position, char orientation) {
		const float d_scale = 0.02f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'v': // vertical
			furn_matrix = YrotationM(furn_matrix, -90);
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 8; j++) {
					r.tiles.map[clampf(j + position.y - 0.5f - 3, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
				}
			}
			
			// position correction
			furn_matrix[3][2] -= 0.5f;
			break;
		case 'h': // horizontal
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 8; j++) {
					r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(j + position.x - 0.5f - 3, 0, r.width - 1)]->is_open = false;
				}
			}
			
			// position correction
			furn_matrix[3][0] -= 0.5f;
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] += position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] += position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::CRATE1);
		r.furniture_textures.push_back(game_manager::texture_list::CRATE_BARREL);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_crate2(room& r, float2e position, float rotation) {
		const float d_scale = 0.02f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		furn_matrix = YrotationM(furn_matrix, rotation);
		
		for (int i = 0; i < 3; i++) {
			r.tiles.map[clampf(position.y - 1, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y + 1, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::CRATE2);
		r.furniture_textures.push_back(game_manager::texture_list::CRATE_BARREL);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_crate3(room& r, float2e position, char orientation) {
		const float d_scale = 0.02f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		
		switch (orientation) {
		case 'v': // vertical
			furn_matrix = ZrotationM(furn_matrix, -90);
			r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;

			// position correction
			furn_matrix[3][0] += 0.5f;
			furn_matrix[3][1] += 1;
			break;
		case 'u': // up
			furn_matrix = YrotationM(furn_matrix, 90);
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y + 1, 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;

			// position correction
			furn_matrix[3][2] += 0.5f;
			break;
		case 'd': // down
			furn_matrix = YrotationM(furn_matrix, -90);
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y + 1, 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;

			// position correction
			furn_matrix[3][2] += 0.5f;
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, 180);
			r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x + 1, 0, r.width - 1)]->is_open = false;

			// position correction
			furn_matrix[3][0] += 0.5f;
			break;
		case 'r': // right
			r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x + 1, 0, r.width - 1)]->is_open = false;

			// position correction
			furn_matrix[3][0] += 0.5f;
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] += position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] += position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::CRATE3);
		r.furniture_textures.push_back(game_manager::texture_list::CRATE_BARREL);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_lightbox(room& r, float2e position, float rotation) {
		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		furn_matrix = YrotationM(furn_matrix, rotation);
		
		r.tiles.map[position.y][position.x]->is_open = false;

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::LIGHT_BOX);
		r.furniture_textures.push_back(game_manager::texture_list::LIGHT_BOX);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_serverbox(room& r, float2e position, char orientation) {
		const float d_scale = 0.2f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		furn_matrix[3][1] += 2;

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'u': // up
			furn_matrix = YrotationM(furn_matrix, 180);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
			}

			// position correction
			furn_matrix[3][0] -= 2;
			break;
		case 'd': // down
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 2, 0, r.width - 1)]->is_open = false;
			}
			furn_matrix[3][0] -= 0.5f;
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, -90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}

			// position correction
			furn_matrix[3][2] -= 2.6f;
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, 90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 2, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			furn_matrix[3][2] -= 0.5f;
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] += position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] += position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::SERVER_BOX1);
		r.furniture_textures.push_back(game_manager::texture_list::SERVER_BOX1);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_electricbox(room& r, float2e position, char orientation) {
		const float d_scale = 0.03f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);

		// this switch case handles the rotation of the matrix of the object only
		switch (orientation) {
		case 'u': // up
			furn_matrix = YrotationM(furn_matrix, 180);
			furn_matrix = XrotationM(furn_matrix, 90);
			break;
		case 'd': // down
			furn_matrix = XrotationM(furn_matrix, 90);
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, -90);
			furn_matrix = XrotationM(furn_matrix, 90);
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, 90);
			furn_matrix = XrotationM(furn_matrix, 90);
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// collision is the same for all orientations
		for (int i = 0; i < 2; i++) {
			r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x, 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x, 0, r.width - 1)]->is_open = false;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::ELECTRIC_BOX1);
		r.furniture_textures.push_back(game_manager::texture_list::ELECTRIC_BOX1);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_barrel(room& r, float2e position, char orientation, int number) {
		const float d_scale = 0.02f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		furn_matrix[3][1] = 0.5f;

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'v': // standing / vertical
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			break;
		case 'd': // down
		case 'u': // up
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y + 1, 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			furn_matrix = ZrotationM(furn_matrix, 90);
			furn_matrix = XrotationM(furn_matrix, -90);

			furn_matrix[3][2] = -0.5f;
			break;
		case 'l': // left
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x - 1, 0, r.width - 1)]->is_open = false;
			furn_matrix = ZrotationM(furn_matrix, -90);

			furn_matrix[3][0] = 0.5f;
			break;
		case 'r': // right
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x    , 0, r.width - 1)]->is_open = false;
			r.tiles.map[clampf(position.y    , 0, r.length - 1)][clampf(position.x + 1, 0, r.width - 1)]->is_open = false;
			furn_matrix = ZrotationM(furn_matrix, 90);

			furn_matrix[3][0] = -0.5f;
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] += position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] += position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::BARREL1 + number);
		r.furniture_textures.push_back(game_manager::texture_list::CRATE_BARREL);

		return r.furniture_matrices.size() - 1;
	}
	int level::place_bed(room& r, float2e position, char orientation) {
		const float d_scale = 4.f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		furn_matrix[3][1] = 1.f;

		// this switch case handles the rotation of the matrix of the object in addition to its collision
		switch (orientation) {
		case 'u': // up
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'd': // down
			furn_matrix = YrotationM(furn_matrix, 180);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x - 0.5f, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(i + position.y - 1, 0, r.length - 1)][clampf(position.x + 0.5f, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, 90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			}
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, -90);
			furn_matrix = XrotationM(furn_matrix, 90);
			for (int i = 0; i < 4; i++) {
				r.tiles.map[clampf(position.y - 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
				r.tiles.map[clampf(position.y + 0.5f, 0, r.length - 1)][clampf(i + position.x - 1, 0, r.width - 1)]->is_open = false;
			}
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// correctly set the furniture's position on the map
		furn_matrix[3][0] = position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] = position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::BED1);
		r.furniture_textures.push_back(game_manager::texture_list::BED1);

		return r.furniture_matrices.size() - 1;
	}

	int level::place_chair(room& r, float2e position, char orientation) {
		const float d_scale = 0.006f;

		// create the matrix for the object in question
		float4x4_a furn_matrix = IdentityM_a();
		scale_matrix(furn_matrix, d_scale, d_scale, d_scale);
		furn_matrix[3][1] = 1;

		// this switch case handles the rotation of the matrix of the object only
		switch (orientation) {
		case 'u': // up
			furn_matrix = YrotationM(furn_matrix, 180);
			furn_matrix[3][2] -= 0.4f;
			break;
		case 'd': // down
			furn_matrix[3][2] = 0.4f;
			break;
		case 'l': // left
			furn_matrix = YrotationM(furn_matrix, -90);
			furn_matrix[3][0] = 0.4f;
			break;
		case 'r': // right
			furn_matrix = YrotationM(furn_matrix, 90);
			furn_matrix[3][0] -= 0.4f;
			break;
		default:
			printf("error! a spawned object wasn't given a valid orientation");
			break;
		}

		// collision is the same for all orientations
		r.tiles.map[clampf(position.y, 0, r.length - 1)][clampf(position.x, 0, r.width - 1)]->is_open = false;

		// correctly set the furniture's position on the map
		furn_matrix[3][0] += position.x - r.width * 0.5f + 0.5f; furn_matrix[3][2] += position.y - r.length * 0.5f + 0.5f;
		
		// actually add the furniture to the room
		r.furniture_matrices.push_back((DirectX::XMMATRIX&)furn_matrix);
		r.furniture_meshes.push_back(game_manager::mesh_types::CHAIR1);
		r.furniture_textures.push_back(game_manager::texture_list::CHAIR1);

		return r.furniture_matrices.size() - 1;
	}
}
