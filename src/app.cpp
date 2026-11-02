#include "app.h"

App::App()
	: menu_screen_(scores_),
	settings_screen_(settings_),
	game_screen_(settings_, scores_),
	scores_screen_(scores_) {
	settings_.load(kSettingsFile); // fine if the file doesn't exist yet
	scores_.load(kHighScoreFile);
	switch_to(ScreenId::MainMenu);
}

void App::switch_to(ScreenId id) {
	switch (id) {
	case ScreenId::MainMenu:   current_ = &menu_screen_; break;
	case ScreenId::Settings:   current_ = &settings_screen_; break;
	case ScreenId::Game:	   current_ = &game_screen_; break;
	case ScreenId::HighScores: current_ = &scores_screen_; break;
	default: return;
	}
	current_->enter();
}

bool App::update(const Input& in, double dt) {
	const ScreenId next = current_->update(in, dt);
	if (next == ScreenId::Quit) return false;
	if (next != ScreenId::None) switch_to(next);
	return true;
}

void App::draw(Canvas& canvas) const {
	current_->draw(canvas);
}
