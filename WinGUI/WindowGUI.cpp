#include "WindowGUI.h"

void WindowGUI::WindowLoop()
{
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

WindowGUI::WindowGUI(int height, int width, int startx, int starty, LPWSTR ClassName, LPWSTR title, HINSTANCE HInst)
{
    hInst = HInst;
    windowWidth = width;
    windowHeight = height;
    Startx = startx;
    Starty = starty;
    className=ClassName;
    Title=title;
}

bool WindowGUI::Init(LRESULT(CALLBACK* proc)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp))
{
    Proc = proc;
    WNDCLASSW wc = { 0 };

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"GUI";
    wc.lpfnWndProc = *Proc;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(wc.lpszClassName, Title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, Startx, Starty, windowWidth, windowHeight, NULL, NULL, NULL, NULL);

    //WindowLoopThread = thread(&WindowGUI::WindowLoop,this);
}

void WindowGUI::AddMenu(string MenuName)
{
    Menu tmp;
    tmp.MenuID = MenuName;
    Menus.push_back(tmp);
}

HMENU WindowGUI::GetMenu(string Name)
{
    for (int i = 0; i < Menus.size(); i++)
    {
        if (Name == Menus[i].MenuID) {
            return Menus[i].hMenu;
        }
    }
    return HMENU();
}

void WindowGUI::AddMenuItem(string ParentMenu, UINT type, UINT_PTR msgID, LPCWSTR Text)
{
    MenuItem tmp;
    tmp.ParentMenu = ParentMenu;
    tmp.type = type;
    tmp.msgID = msgID;
    tmp.Text = Text;
    MenuItems.push_back(tmp);
}

void WindowGUI::AddControl(string controlid, LPWSTR classname, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height,  HMENU function)
{
    Control tmp;
    tmp.ControlID = controlid;
    tmp.ClassName = classname;
    tmp.InsideText = insidetext;
    tmp.Style = style;
    tmp.PosX = posx;
    tmp.PosY = posy;
    tmp.Width = width;
    tmp.Height = height;
    tmp.Function = function;
    Controls.push_back(tmp);
}

void WindowGUI::AddButton(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height, HMENU function)
{
    Control tmp;
    tmp.ControlID = controlid;
    tmp.ClassName = (LPWSTR)L"Button";
    tmp.InsideText = insidetext;
    tmp.Style = style;
    tmp.PosX = posx;
    tmp.PosY = posy;
    tmp.Width = width;
    tmp.Height = height;
    tmp.Function = function;
    Controls.push_back(tmp);
}

void WindowGUI::AddLabel(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height)
{
    Control tmp;
    tmp.ControlID = controlid;
    tmp.ClassName = (LPWSTR)L"Static";
    tmp.InsideText = insidetext;
    tmp.Style = style;
    tmp.PosX = posx;
    tmp.PosY = posy;
    tmp.Width = width;
    tmp.Height = height;
    tmp.Function = NULL;
    Controls.push_back(tmp);
}

void WindowGUI::AddEdit(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height)
{
    Control tmp;
    tmp.ControlID = controlid;
    tmp.ClassName = (LPWSTR)L"Edit";
    tmp.InsideText = insidetext;
    tmp.Style = style;
    tmp.PosX = posx;
    tmp.PosY = posy;
    tmp.Width = width;
    tmp.Height = height;
    tmp.Function = NULL;
    Controls.push_back(tmp);
}

Vec2 WindowGUI::GetWindowCenter()
{
    Vec2 tmp;
    tmp.x = floor(windowWidth / 2);
    tmp.y = floor(windowHeight / 2);

    return tmp;
}

HWND WindowGUI::GetControl(string Name)
{
    for (int i = 0; i < Controls.size(); i++)
    {
        if (Name == Controls[i].ControlID) {
            return Controls[i].CONTROL;
        }
    }
    return HWND();
}

WindowGUI::~WindowGUI()
{
    //WindowLoopThread.join();
}

void WindowGUI::AddMenus(HWND hWnd)
{
    for (int i = 0; i < Menus.size(); i++)
    {
        Menus[i].hMenu = CreateMenu();
        for (int j = 0; j < MenuItems.size(); j++)
        {
            if (MenuItems[j].ParentMenu == Menus[i].MenuID) {
                AppendMenuW(Menus[i].hMenu, MenuItems[j].type, MenuItems[j].msgID, MenuItems[j].Text);
            }
        }
    }

    if (!Menus.empty()) {
        SetMenu(hWnd, Menus[0].hMenu);
    }
}

void WindowGUI::AddControls(HWND hWnd)
{
    for (int i = 0; i < Controls.size(); i++)
    {
        Controls[i].CONTROL=CreateWindowW(Controls[i].ClassName, Controls[i].InsideText, Controls[i].Style, Controls[i].PosX, Controls[i].PosY, Controls[i].Width, Controls[i].Height, hWnd, Controls[i].Function, NULL, NULL);
    }
}
