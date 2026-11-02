#pragma once

#include "engine/canvas.h"
#include "engine/input.h"
#include "game/highscores.h"
#include "game/settings.h"
#include "screens/game_screen.h"
#include "screens/highscores_screen.h"
#include "screens/main_menu_screen.h"
#include "screens/settings_screen.h"

// Owns the shared data and the screens and switches between them.
class App {
public:
	App();

	bool update(const Input& in, double dt); // returns false to quit
	void draw(Canvas& canvas) const;

private:
	void switch_to(ScreenId id);

	//Declared before the screens, because the screens hold references to them.
	Settings settings_;
	HighScoreTable scores_;

	MainMenuScreen menu_screen_;
	SettingsScreen settings_screen_;
	GameScreen game_screen_;
	HighScoresScreen scores_screen_;

	Screen* current_ = nullptr;
};
