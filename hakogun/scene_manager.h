#pragma once
#include <d3d11.h>
#include	"directxmath.h"

// 定数 ----------------------------------------------------------------
enum SCENES
{
    keep,

    state_title,
    state_game,
    state_clear,
    state_over,
    state_change,
};

// クラス --------------------------------------------------------------
#define pSceneManager SceneManager::getInstance()
class SceneManager
{
public:
    static  inline SceneManager& getInstance()
    {
        static SceneManager instance;
        return instance;
    }


    void init();
    void update();
    void render();
    void changeSceneStateInit(SCENES _next_scene);    // シーン遷移処理

    void setChangeScene(SCENES _change_scene);  // シーン遷移時に使用
    void setSceneState(SCENES _scene_state);
    int getSceneState();
    int getChangeScene();

    bool playBGM = false;
    bool playChoice = false;
    bool playDeath = false;
    bool playReturn = false;

private:
    SCENES change_scene = keep;
    SCENES scene_state = state_title;
};

#define pSceneTitle SceneTitle::getInstance()

class SceneTitle
{
public:
    static  inline SceneTitle& getInstance()
    {
        static SceneTitle instance;
        return instance;
    }

    void init(void);
    void update(void);
    void render(void);
    void end(void);

    int timer = 0;
private:
};

#define pSceneGame SceneGame::getInstance()
class SceneGame
{
public:
    static  inline SceneGame& getInstance()
    {
        static SceneGame instance;
        return instance;
    }

    void init();
    void update();
    void render();
    void end(void);

    int timer = 0;
private:
};

#define pSceneClear SceneClear::getInstance()
class SceneClear
{
public:
    static  inline SceneClear& getInstance()
    {
        static SceneClear instance;
        return instance;
    }

    void init(void);
    void update(void);
    void render(void);
    void end(void);

    int timer = 0;
private:
};

#define pSceneChange SceneChange::getInstance()
class SceneChange
{
public:
    static  inline SceneChange& getInstance()
    {
        static SceneChange instance;
        return instance;
    }

    void init(void);
    void update(void);
    void render(void);
    void end(void);

    int timer = 0;
private:
};