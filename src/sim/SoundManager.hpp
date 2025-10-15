#pragma once

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/sound/SoundEngine.hpp>

class SoundManager
{
	WIN_NO_COPY_MOVE(SoundManager);

	static constexpr auto vol = 0.2f;

public:
	SoundManager(win::AssetRoll &roll, float area_left, float area_right);

	void play_music();
	void play_menu_click();
	void play_ball_bounce(float x);
	void play_ball_paddle(float x);
	void play_yipee();
	void play_wompwomp();
	void play_hurray();
	void play_yousuck();

	void mute(bool m);

private:
	void get_left_right(float x, float &left, float &right) const;

	const float area_left, area_right;

	win::SoundEngine sounds;
	bool muted = false;
	std::uint32_t music = -1;
};
