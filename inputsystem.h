#pragma once

#include <array>
#include <cstring>
#include <string>
#include <map>

enum ButtonState
{
	Up,
	Down,
	Held
};

typedef struct Mouse
{
	int x;
	int y;
	static constexpr int buttons_max = 3;
	std::array<ButtonState, buttons_max> buttons;
	Mouse()
	{
		x = 0;
		y = 0;
		memset(&buttons[0], 0, sizeof(ButtonState)*buttons_max);
	}
} Mouse;

typedef struct Keyboard
{
	static constexpr int keys_max = 258;
	std::map<uint32_t, ButtonState> keys;
	Keyboard()
	{
		//memset(&keys[0], 0, sizeof(ButtonState)*keys_max);
	}

	std::string text;
} Keyboard;

class InputSystem 
{
	InputSystem();
	virtual ~InputSystem();

private:
	Mouse mouse_;
	Keyboard keyboard_;

public:
	static InputSystem* get_instance();
	virtual void update(double dt);
	const Mouse& get_mouse() const { return mouse_; }
	const Keyboard& get_keyboard() const { return keyboard_; }
};