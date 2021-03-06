#include "../hpp/behavior_tree.hpp"

#include <functional>

namespace crow {

	void behavior_tree::composite_node::add_child(node* n) {
		children.push_back(n);
	}

	const std::vector<behavior_tree::node*>&
		behavior_tree::composite_node::get_children() const {
		return children;
	}

	status behavior_tree::selector_node::run(float dt, crow::ai_manager& m) {
		for (auto child : get_children()) {
			status current = child->run(dt, m);
			if (current == status::PASSED || current == status::RUNNING) {
				return current;
			}
		}
		return status::FAILED;
	}

	status behavior_tree::sequence_node::run(float dt, crow::ai_manager& m) {
		for (auto child : get_children()) {
			status current = child->run(dt, m);
			if (current == status::FAILED || current == status::RUNNING) {
				return current;
			}
		}
		return status::PASSED;
	}

	behavior_tree::node* behavior_tree::decorator_node::get_child() const {
		return child;
	}

	void behavior_tree::decorator_node::set_child(node* n) { child = n; }

	status behavior_tree::root_node::run(float dt, crow::ai_manager& m) {
		if (get_child()) {
			return get_child()->run(dt, m);
		}
		return status::FAILED;
	}

	status behavior_tree::inverter_node::run(float dt, crow::ai_manager& m) {
		const status temp = get_child()->run(dt, m);
		if (temp == status::RUNNING) {
			return temp;
		}

		if (temp == status::PASSED) {
			return status::FAILED;
		}

		return status::PASSED;
	}

	behavior_tree::behavior_tree() : root(new root_node) {}

	behavior_tree::~behavior_tree() { delete root; }

	void behavior_tree::build_tree(int type) {
		switch (type)
		{
		// passive ai
		case 1: {
			// allocating nodes for the tree
			lnodes.resize(10);
			for (auto& n : lnodes) {
				n = new leaf_node();
			}

			seqn.resize(2);
			for (auto& n : seqn) {
				n = new sequence_node();
			}

			seln.resize(4);
			for (auto& n : seln) {
				n = new selector_node();
			}

			invn.resize(1);
			for (auto& n : invn) {
				n = new inverter_node();
			}

			// assigning behaviors to leaf nodes
			{
				unsigned short c = 0;
				lnodes[c++]->r = &crow::passive_roam_check;
				lnodes[c++]->r = &crow::roam_path;
				lnodes[c++]->r = &crow::move;

				lnodes[c++]->r = &crow::passive_has_target;
				lnodes[c++]->r = &crow::target_door;

				lnodes[c++]->r = &crow::has_path;
				lnodes[c++]->r = &crow::get_path;

				lnodes[c++]->r = &crow::reached_target;
				lnodes[c++]->r = &crow::move;

				lnodes[c++]->r = &crow::handle_door;
			}

			{
				unsigned int c = 0;
				unsigned int sel = 0;
				unsigned int seq = 0;
				unsigned int inv = 0;

				// adding all selector nodes to main sequence node
				set_root_child(seqn[seq]);
				for (size_t i = 0; i < 4; i++) {
					seqn[seq]->add_child(seln[i]);
				}
				// adding the final behavior to the main sequence node
				seqn[seq]->add_child(lnodes.back());
				seq++;

				// 1st selector node
				invn[inv]->set_child(lnodes[c++]);
				seln[sel]->add_child(invn[inv]);

				for (size_t i = 0; i < 2; ++i){
					seqn[seq]->add_child(lnodes[c++]);
				}
				seln[sel]->add_child(seqn[seq++]);
				++sel;

				// 2nd selector node
				for (size_t i = 0; i < 2; i++){
					seln[sel]->add_child(lnodes[c++]);
				}
				++sel;

				// 3rd selector node
				for (size_t i = 0; i < 2; i++) {
					seln[sel]->add_child(lnodes[c++]);
				}
				++sel;

				// 4th selector node
				for (size_t i = 0; i < 2; i++) {
					seln[sel]->add_child(lnodes[c++]);
				}
				++sel;

			}

			break;
		}
		// enemy AI
		default: {
			// allocating nodes for the tree
			lnodes.resize(15);
			// 0 - 11 child nodes (12)
			for (auto& n : lnodes) {
				// lnodes[i]
				n = new leaf_node();
			}

			seqn.resize(4);
			// 12 - 14 sequence nodes (3)
			for (auto& n : seqn) {
				// seqn[i]
				n = new sequence_node();
			}

			seln.resize(5);
			// 15 - 18 selector nodes (4)
			for (auto& n : seln) {
				// seln[i]
				n = new selector_node();
			}

			invn.resize(1);
			for (auto& n : invn) {
				n = new inverter_node();
			}

			// assigning behaviors to leaf nodes
			{
				unsigned int c = 0;
				lnodes[c++]->r = &crow::roam_check;
				lnodes[c++]->r = &crow::roam_path;
				lnodes[c++]->r = &crow::move;

				lnodes[c++]->r = &crow::has_target;
				lnodes[c++]->r = &crow::target_player;
				lnodes[c++]->r = &crow::target_console;
				lnodes[c++]->r = &crow::target_door;

				lnodes[c++]->r = &crow::has_path;
				lnodes[c++]->r = &crow::is_path_currernt;
				lnodes[c++]->r = &crow::get_path;

				lnodes[c++]->r = &crow::reached_target;
				lnodes[c++]->r = &crow::move;

				lnodes[c++]->r = &crow::is_target_door;
				lnodes[c++]->r = &crow::handle_door;
				lnodes[c++]->r = &crow::destroy_target;
			}

			// connecting nodes to form tree
			// tree diagram
			// vector index = num - 1
			//           root
			//             |
			//            (16)seq
			//    |       |        |        |        |
			//  20sel   (21)sel  (22)sel  (23)sel  (24)sel
			//   | |    | | | |    |    |   | |    |      |
			// inv 17seq 4 5 6 7 (18)seq 10 11 12   (19)seq 15
			//  |  | |          | |                  | |
			//  1   2 3         8 9                 13 14

			{
				unsigned int c = 0;
				unsigned int sel = 0;
				unsigned int seq = 0;
				unsigned int inv = 0;

				set_root_child(seqn[seq]);
				for (size_t i = 0; i < 5; i++) {
					seqn[seq]->add_child(seln[i]);
				}
				seq++;

				// 1st selector
				seln[sel]->add_child(invn[inv]);
				invn[inv]->set_child(lnodes[c++]);
				for (size_t i = 0; i < 2; i++) {
					seqn[seq]->add_child(lnodes[c++]);
				}
				seln[sel]->add_child(seqn[seq++]);
				sel++;

				// 2nd selector
				for (size_t i = 0; i < 4; i++) {
					seln[sel]->add_child(lnodes[c++]);
				}
				sel++;

				// 3rd selector
				seln[sel]->add_child(seqn[seq]);
				for (size_t i = 0; i < 2; i++) {
					seqn[seq]->add_child(lnodes[c++]);
				}
				seq++;
				seln[sel++]->add_child(lnodes[c++]);

				// 4th selector
				for (size_t i = 0; i < 2; i++) {
					seln[sel]->add_child(lnodes[c++]);
				}
				sel++;

				// 5th selector
				seln[sel]->add_child(seqn[seq]);
				for (size_t i = 0; i < 2; i++) {
					seqn[seq]->add_child(lnodes[c++]);
				}
				seq++;
				seln[sel++]->add_child(lnodes[c++]);
			}
			break;
		}
		}

	}

	void behavior_tree::clean_tree() {
		for (auto& n : lnodes) {
			if (n) {
				delete n;
			}
		}

		for (auto& n : seln) {
			if (n) {
				delete n;
			}
		}

		for (auto& n : seqn) {
			if (n) {
				delete n;
			}
		}

		for (auto& n : invn) {
			if (n) {
				delete n;
			}
		}
	}

	void behavior_tree::set_root_child(node* n) const { root->set_child(n); }

	status behavior_tree::run(float dt) const {
		status result = status::FAILED;
		if (aim) {
			result = root->run(dt, *aim);
		}
		return result;
	}

	status behavior_tree::leaf_node::run(float dt, crow::ai_manager& m) {
		status result = status::FAILED;
		if (r) {
			result = std::invoke(r, dt, m);
		}
		return result;
	}

}  // namespace crow