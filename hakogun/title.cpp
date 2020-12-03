// インクルード ----------------------------------------------------------------------------------
#include "framework.h"
#include "scene_manager.h"
#include "title.h"

// 関数 ----------------------------------------------------------------------------------------
void Title::init(void)
{

}

void Title::update(void)
{
    if (GetAsyncKeyState(VK_SPACE) & 1)
    {
        pSceneManager.playChoice = true;

        pSceneManager.setChangeScene(state_game);
    }
}

void Title::render(void)
{

}

void Title::end(void)
{

}