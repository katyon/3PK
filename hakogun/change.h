#pragma once

// 定数 ----------------------------------------------------------------

// クラス --------------------------------------------------------------
#define pChange Change::getInstance()
class Change
{
public:
    static  inline Change& getInstance()
    {
        static Change instance;
        return instance;
    }

    void init(void);
    void update(void);
    void render(void);
    void end(void);

private:
};