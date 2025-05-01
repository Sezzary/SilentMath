#include "Framework.h"
#include "Engine/Input/Input.h"

#include "Engine/Input/Keys.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Input
{
    void InputManager::Initialize()
    {
        _rawInput.KeyStates.resize((int)KeyId::Count);
    }

    void InputManager::Deinitialize()
    {
        
    }

    void InputManager::Update()
    {
        // Update key states.
        int keyStateIdx = 0;
        ReadKeyboard(keyStateIdx);
        ReadMouse(keyStateIdx);
        ReadController(keyStateIdx);
    }

    void InputManager::ReadKeyboard(int& keyStateIdx)
    {
        int         keyboardStateCount = 0;
        const bool* keyboardState      = SDL_GetKeyboardState(&keyboardStateCount);
        SDL_Keymod  modState           = SDL_GetModState();

        // Set key states.
        for (int scanCode : VALID_KEYBOARD_SCAN_CODES)
        {
            if (keyboardStateCount <= 0)
            {
                break;
            }
            else if (scanCode >= keyboardStateCount)
            {
                continue;
            }

            _rawInput.KeyStates[keyStateIdx] = keyboardState[scanCode];
            keyStateIdx++;
        }

        // Set modifier key states.
        for (int modCode : VALID_KEYBOARD_MOD_CODES)
        {
            _rawInput.KeyStates[keyStateIdx] = modState & modCode;
            keyStateIdx++;
        }
    }

    void InputManager::ReadMouse(int& keyStateIdx)
    {
        auto pos      = Vector2::Zero;
        auto butState = SDL_GetMouseState(&pos.x, &pos.y);
        
        // Set button states.
        for (int butCode : VALID_MOUSE_BUT_CODES)
        {
            _rawInput.KeyStates[keyStateIdx] = butState & SDL_BUTTON_MASK(butCode);
            keyStateIdx++;
        }

        // Set axis states.
        _rawInput.MousePosition = pos;
    }

    void InputManager::ReadController(int& keyStateIdx)
    {
        // TODO
        /*SDL_GameController* controller = SDL_GameControllerOpen(0);

        if (controller != nullptr)
        {
            if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
            {
                // A button is pressed
            }
        
            float leftAxis = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
            if (leftAxis < -0.1f)
            {

            }
        }
        
        SDL_GameControllerClose(controller);*/
    }
}
