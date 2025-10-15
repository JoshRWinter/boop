#include "SoundManager.hpp"

SoundManager::SoundManager(win::AssetRoll &roll, float area_left, float area_right)
	: area_left(area_left)
	, area_right(area_right)
	, sounds(roll)
{
}

void SoundManager::play_music()
{
	const float musicvol = 0.4f;

	if (!muted && music == -1)
		music = sounds.play("sound/music1.ogg", 10, 0.0f, vol * musicvol, vol * musicvol, true, false);
}

void SoundManager::play_menu_click()
{
	if (!muted)
		sounds.play("sound/click.ogg", 0, 1.0f, vol, vol, false, true);
}

void SoundManager::play_ball_bounce(float x)
{
	float l, r;
	get_left_right(x, l, r);

	if (!muted)
		sounds.play("sound/hit1.ogg", 0, 1.0f, l * vol, r * vol, false, true);
}

void SoundManager::play_ball_paddle(float x)
{
	float l, r;
	get_left_right(x, l, r);

	if (!muted)
		sounds.play("sound/hit2.ogg", 0, 1.0f, l * vol, r * vol, false, true);
}

void SoundManager::play_yipee()
{
	if (!muted)
		sounds.play("sound/yipee.ogg", 0, 1.0f, vol, vol, false, true);
}

void SoundManager::play_wompwomp()
{
	if (!muted)
		sounds.play("sound/wompwomp.ogg", 0, 1.0f, vol, vol, false, true);
}

void SoundManager::play_hurray()
{
	if (!muted)
		sounds.play("sound/hurray.ogg", 0, 1.0f, vol, vol, false, true);
}

void SoundManager::play_yousuck()
{
	if (!muted)
		sounds.play("sound/yousuck.ogg", 0, 1.0f, vol, vol, false, true);
}

void SoundManager::mute(bool m)
{
	if (!muted && m)
		sounds.pause(music);
	else if (muted && !m)
		sounds.resume(music);

	sounds.save();

	muted = m;
}

void SoundManager::get_left_right(float x, float &left, float &right) const
{
	const float halfwidth = (area_right - area_left) / 2.0f;

	const float leftness = (halfwidth - (x - area_left)) / halfwidth;
	const float rightness = (halfwidth - (area_right - x)) / halfwidth;

	left = std::max(0.3f, std::min(0.4f, leftness));
	right = std::max(0.3f, std::min(0.4f, rightness));
}
