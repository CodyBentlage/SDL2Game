#include "common.h"
#include "input.h"

extern App app;
static void doKeyDown(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 1;
	}
}

static void doKeyUp(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 0;
	}
}

void doButtonDown(SDL_JoyButtonEvent *event)
{
	if (event->button < MAX_JOYSTICK_BUTTONS)
	{
		app.joystickButtons[event->button] = 1;
	}
}

void doButtonUp(SDL_JoyButtonEvent *event)
{
	if (event->button < MAX_JOYSTICK_BUTTONS)
	{
		app.joystickButtons[event->button] = 0;
	}
}

void doInput(void)
{
	SDL_Event event;

	memset(app.inputText, '\0', MAX_LINE_LENGTH);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;

		case SDL_KEYDOWN:
			doKeyDown(&event.key);
			break;

		case SDL_KEYUP:
			doKeyUp(&event.key);
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button < MAX_MOUSE_BUTTONS)
			{
				app.mouse_buttons[event.button.button] = 1;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button < MAX_MOUSE_BUTTONS)
			{
				app.mouse_buttons[event.button.button] = 0;
			}
			break;

		case SDL_MOUSEMOTION:
			app.mouse_x = event.motion.x;
			app.mouse_y = event.motion.y;
			break;

		case SDL_TEXTINPUT:
			STRNCPY(app.inputText, event.text.text, MAX_LINE_LENGTH);
			break;

		// Game controller code!!!
		case SDL_CONTROLLERBUTTONDOWN:
			if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) // Adjust this to your correct SDL_CONTROLLER_BUTTON for R1
			{
				doButtonDown(&event.jbutton);
			}
			break;

		case SDL_CONTROLLERBUTTONUP:
			if (event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) // Adjust this to your correct SDL_CONTROLLER_BUTTON for R1
			{
				doButtonUp(&event.jbutton);
			}
			break;

		default:
			break;
		}
	}
}