// �C���N���[�h ----------------------------------------------------------------------------------
#include "framework.h"
#include "input.h"
#include "scene_manager.h"
#include "title.h"

// �֐� ----------------------------------------------------------------------------------------
void Title::init(void)
{

}

void Title::update(void)
{
    if (pInputManager->inputKeyState(DIK_SPACE))
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