#pragma once

#include <vector>
#include <string>

namespace crow {
	struct credit {
		enum class credit_type : unsigned char {
			TEXT = 0, TITLE, IMAGE, BLANK
		} type = credit_type::TEXT;

		int image_id = 0;

		// ratio of texture to screen size, not absolute size of texture
		float image_w = 0;
		float image_h = 0;

		std::string text = "";

		credit(credit_type _t, int _id, float _w, float _h, std::string _text) :
				type(_t), image_id(_id), image_w(_w), image_h(_h), text(_text) { }
	};

	extern std::vector<credit> credits;

	void init_credits();
	void deinit_credits();
}