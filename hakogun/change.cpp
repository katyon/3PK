// �C���N���[�h ----------------------------------------------------------------------------------
#include "framework.h"
#include "input.h"
#include "scene_manager.h"
#include "change.h"

// �֐� ----------------------------------------------------------------------------------------
void Change::init(void)
{

}

void Change::update(void)
{
    if (pInputManager->inputKeyTrigger(DIK_SPACE))
    {
        pSceneManager.playChoice = true;

        pSceneManager.setChangeScene(state_game);
    }
}

void Change::render(void)
{

}

void Change::end(void)
{

}