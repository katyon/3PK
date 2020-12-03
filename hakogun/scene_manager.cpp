// インクルード ----------------------------------------------------------------------------------
#include "scene_manager.h"

#include "clear.h"
#include "game.h"
#include "title.h"

// 関数 ----------------------------------------------------------------------------------------
void SceneManager::init(void)
{
    pSceneTitle.init();
    change_scene = keep;
    scene_state = state_title;
}

void SceneManager::update(ID3D11Device* device)
{
    if (change_scene != keep)
    {
        pSceneManager.changeSceneStateInit(device,change_scene);
    }

    switch (scene_state)
    {
    case state_title:
        pSceneTitle.update();
        break;
    case state_game:
        pSceneGame.update(device);
        break;
    case state_clear:
        pSceneClear.update();
        break;
    default: break;
    }
}

void SceneManager::render(ID3D11DeviceContext* context)
{
    switch (scene_state)
    {
    case state_title:
        pSceneTitle.render();
        break;
    case state_game:
        pSceneGame.render(context);
        break;
    case state_clear:
        pSceneClear.render();
        break;
    default: break;
    }
}

void SceneManager::changeSceneStateInit(ID3D11Device* device,SCENES _next_scene)
{
    // 現在のシーンの終了処理
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
    default:
        break;
    }

    // シーン遷移時に初期化
    switch (_next_scene)
    {
    case state_title:
        pSceneTitle.init();
        break;
    case state_game:
        pSceneGame.init(device);
        break;
    case state_clear:
        pSceneClear.init();
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
void SceneGame::init(ID3D11Device* device)
{
    pGame.Initialize(device);
    timer = 0;
}

void SceneGame::update(ID3D11Device* device)
{

    pGame.Update(device);
    timer++;
}

void SceneGame::render(ID3D11DeviceContext* context)
{
    pGame.Render(context);
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