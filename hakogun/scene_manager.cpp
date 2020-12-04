// �C���N���[�h ----------------------------------------------------------------------------------
#include "scene_manager.h"

#include "change.h"
#include "clear.h"
#include "game.h"
#include "title.h"

// �֐� ----------------------------------------------------------------------------------------
void SceneManager::init(void)
{
    pSceneTitle.init();
    change_scene = keep;
    scene_state = state_title;
}

void SceneManager::update()
{
    if (change_scene != keep)
    {
        pSceneManager.changeSceneStateInit(change_scene);
    }

    switch (scene_state)
    {
    case state_title:
        pSceneTitle.update();
        break;
    case state_game:
        pSceneGame.update();
        break;
    case state_clear:
        pSceneClear.update();
        break;
    case state_change:
        pSceneChange.update();
        break;
    default: break;
    }
}

void SceneManager::render()
{
    switch (scene_state)
    {
    case state_title:
        pSceneTitle.render();
        break;
    case state_game:
        pSceneGame.render();
        break;
    case state_clear:
        pSceneClear.render();
        break;
    case state_change:
        pSceneChange.render();
        break;
    default: break;
    }
}

void SceneManager::changeSceneStateInit(SCENES _next_scene)
{
    // ���݂̃V�[���̏I������
    switch (getSceneState())
    {
    case state_title:
        pSceneTitle.end();
        break;
    case state_game:
        pSceneGame.end();
        break;
    case state_clear:
        pSceneClear.end();
        break;
    case state_change:
        pSceneChange.end();
        break;
    default:
        break;
    }

    // �V�[���J�ڎ��ɏ�����
    switch (_next_scene)
    {
    case state_title:
        pSceneTitle.init();
        break;
    case state_game:
        pSceneGame.init();
        break;
    case state_clear:
        pSceneClear.init();
        break;
    case state_change:
        pSceneChange.init();
        break;
    default:
        break;
    }

    setChangeScene(keep);
    setSceneState(_next_scene);
}

void SceneManager::setSceneState(SCENES _scene_state)
{
    scene_state = _scene_state;
}
void SceneManager::setChangeScene(SCENES _change_scene)
{
    change_scene = _change_scene;
}
int SceneManager::getSceneState()
{
    return scene_state;
}
int SceneManager::getChangeScene()
{
    return change_scene;
}

// Title
void SceneTitle::init(void)
{
    pTitle.init();
    pSceneManager.playBGM = true;
    timer = 0;
}

void SceneTitle::update(void)
{
    pTitle.update();
    timer++;
}

void SceneTitle::render(void)
{
    pTitle.render();
}

void SceneTitle::end(void)
{
    pTitle.end();
}

// Game
void SceneGame::init()
{
    pGame.Initialize();
    timer = 0;
}

void SceneGame::update()
{

    pGame.Update();
    timer++;
}

void SceneGame::render()
{
    pGame.Render();
}

void SceneGame::end(void)
{
    pGame.Release();
}

// Clear
void SceneClear::init(void)
{
    pClear.init();
    timer = 0;
}

void SceneClear::update(void)
{

    pClear.update();
    timer++;
}

void SceneClear::render(void)
{
    pClear.render();
}

void SceneClear::end(void)
{
    pClear.end();
}

// Change
void SceneChange::init(void)
{
    pChange.init();
    timer = 0;
}

void SceneChange::update(void)
{

    pChange.update();
    timer++;
}

void SceneChange::render(void)
{
    pChange.render();
}

void SceneChange::end(void)
{
    pChange.end();
}