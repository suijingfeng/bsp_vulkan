#ifndef Q3BSPSTATSUI_INCLUDED
#define Q3BSPSTATSUI_INCLUDED

#include "../StatsUI.hpp"
#include "../renderer/Font.hpp"

/*
 *  Quake III BSP stats display
 */

class Q3StatsUI : public StatsUI
{
public:
    Q3StatsUI(BspMap *map);

    ~Q3StatsUI()
    {
        delete m_font;
    }

    void OnRender();
    void OnWindowChanged();
private:
    Font *m_font = nullptr;
};

#endif
