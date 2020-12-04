// インクルード ----------------------------------------------------------------------------------
#include "framework.h"
#include "input.h"
#include "scene_manager.h"
#include "clear.h"

// 関数 ----------------------------------------------------------------------------------------
void Clear::init(void)
{

}

void Clear::update(void)
{
    if (pInputManager->inputKeyState(DIK_SPACE))
    {
        pSceneManager.playChoice = true;

        pSceneManager.setChangeScene(state_title);
    }
}


void Clear::render(void)
{

}

void Clear::end(void)
{

}