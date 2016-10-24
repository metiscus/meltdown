#include "inputsystem.h"

#include <cassert>
#include <mutex>

#include <SDL.h>

static std::once_flag create_instance;
static InputSystem *g_system = nullptr;

InputSystem* InputSystem::get_instance()
{
	std::call_once(create_instance, [](){
		g_system = new InputSystem();
		fprintf(stderr, "[InputSystem] : Initialized!\n");
	});
	assert(g_system);
	return g_system;
}

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{

}

void InputSystem::update(double dt)
{
	SDL_PumpEvents();

	keyboard_.text = "";

	while(SDL_PollEvent(nullptr) == 1)
	{
	    SDL_Event event;
    	SDL_PollEvent(&event);
		
		switch(event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				SDL_KeyboardEvent& key_event = event.key;
				
				if(key_event.state == SDL_PRESSED)
				{
					if(key_event.repeat != 0)
					{
						keyboard_.keys[key_event.keysym.sym] = Held;
					}
					else
					{
						keyboard_.keys[key_event.keysym.sym] = Down;
					}
				}
				else
				{
					keyboard_.keys[key_event.keysym.sym] = Up;
				}
				break;
			}

			case SDL_TEXTINPUT:
			{
				keyboard_.text = event.text.text;
				break;
			}

			case SDL_MOUSEMOTION:
			{
				SDL_MouseMotionEvent& motion_event = event.motion;
				mouse_.x = motion_event.x;
				mouse_.y = motion_event.y;
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				SDL_MouseButtonEvent& button_event = event.button;
				auto &button = mouse_.buttons[button_event.button];
				if(button_event.state == SDL_PRESSED)
				{
					
					if(button == Down) { button = Held; }
					else { button = Down; }
				}
				else
				{
					button = Up;
				}

				mouse_.x = button_event.x;
				mouse_.y = button_event.y;
				break;
			}

			case SDL_QUIT:
			{
				exit(0);
				break;
			}
		}
	}
}