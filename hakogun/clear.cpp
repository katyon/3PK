// インクルード ----------------------------------------------------------------------------------
#include "framework.h"
#include "scene_manager.h"
#include "clear.h"

// 関数 ----------------------------------------------------------------------------------------
void Clear::init(void)
{

}

void Clear::update(void)
{
    if (GetAsyncKeyState(VK_SPACE) & 1)
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