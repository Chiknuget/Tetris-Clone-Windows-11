#pragma once

#include <string>

#include "game/settings.h"
#include "screen.h"

class SettingsScreen : public Screen {
public:
	explicit SettingsScreen(Settings& settings);

	void enter() override;
	ScreenId update(const Input& in, double dt) override;
	void draw(Canvas& canvas) const override;

private:
	static constexpr int kRowCount = 6;

	void adjust(int row, int dir);
	std::string value_text(int row) const;

	Settings& settings_;
	int selected_ = 0;
};
