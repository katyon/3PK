#pragma once

// ’è” ----------------------------------------------------------------

// ƒNƒ‰ƒX --------------------------------------------------------------
#define pTitle Title::getInstance()
class Title
{
public:
    static  inline Title& getInstance()
    {
        static Title instance;
        return instance;
    }

    void init(void);
    void update(void);
    void render(void);
    void end(void);

private:
};