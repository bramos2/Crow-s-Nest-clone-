#include "../hpp/credits.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
	std::vector<credit> credits;

	void init_credits() {
		const float blank_value = 0.08f;

		credits.push_back(credit(credit::credit_type::IMAGE, game_manager::texture_list::GUI_LOGO, 0.5f, 0.2f, "Crow's Nest")); // logo goes here
		credits.push_back(credit(credit::credit_type::TITLE, -1, 0, 0, "PROGRAMMING"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Julio Delgado Abreu"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Brian Ramos"));

		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, blank_value, ""));
		credits.push_back(credit(credit::credit_type::TITLE, -1, 0, 0, "SOUND"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Andrew Rocafort"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Cave Theme\" by Brandon75689 licensed OGA-BY 3.0 or CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/cave-theme"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Ambient main menu\" by Brandon75689 licensed OGA-BY 3.0 or CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/tragic-ambient-main-menu"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Credits/ Game Over\" by Umplix licensed CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/credits-game-over"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Intense Synth\" by Alex McCulloch licensed CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/intense-synth"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Breathe (Under The Sea)\" by Ilker Yalciner licensed CC-BY 4.0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/breathe-under-the-sea"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"Interface Sounds\" by Kenny licensed CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/interface-sounds"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "\"50 RPG Sound Effects\" by Kenny licensed CC0"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/content/50-rpg-sound-effects"));
		
		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, blank_value, ""));
		credits.push_back(credit(credit::credit_type::TITLE, -1, 0, 0, "MODELS AND TEXTURES"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Logos by Jess Mosholder https://jess-mosholder.myportfolio.com/"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Office chair by tonnynevel https://www.cgtrader.com/tonnynevel"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Servers free by sobul https://www.cgtrader.com/sobul"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Lab Device by alexsychov https://www.cgtrader.com/alexsychov"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Sofa Collection by DChacal https://www.cgtrader.com/dchacal"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Army bed by protatip1234555 https://www.cgtrader.com/protatip1234555"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "AIConsole.fbx by Joshua Blue"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Sci-Fi Construction Kit (Modular) by SickHead Games"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://assetstore.unity.com/packages/3d/environments/"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "sci-fi/sci-fi-construction-kit-modular-159280"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "SciFi Crates and Barrels Set by Thomas Colmenares"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "SciFi Console / Mystical Shrine by Knittel"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://opengameart.org/users/knittel"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Sci-fi GUI skin by 3d.rina"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "https://assetstore.unity.com/packages/2d/gui/sci-fi-gui-skin-15606"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Animations from mixamo.com"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "GP Games Asset Repository"));
		
		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, blank_value, ""));
		credits.push_back(credit(credit::credit_type::TITLE, -1, 0, 0, "SPECIAL THANKS"));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "William Gooch"));

		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, blank_value * 4, ""));
		credits.push_back(credit(credit::credit_type::IMAGE, game_manager::texture_list::SPLASH_FS, 0.13f, 0.08125f * 1.6f, ""));
		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, 0.12f, ""));
		credits.push_back(credit(credit::credit_type::IMAGE, game_manager::texture_list::SPLASH_GD, 0.5f, 0.33f * 1.6f, ""));
		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, 0.12f, ""));
		credits.push_back(credit(credit::credit_type::IMAGE, game_manager::texture_list::SPLASH_LV, 0.49f, 0.29f * 1.6f, ""));

		credits.push_back(credit(credit::credit_type::BLANK, -1, 0, 0.8f, ""));
		credits.push_back(credit(credit::credit_type::TEXT, -1, 0, 0, "Thank you for playing!"));
	}

	void deinit_credits() {
		credits.clear();
	}
}