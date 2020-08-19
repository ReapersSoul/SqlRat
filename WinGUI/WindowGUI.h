#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
using namespace std;

/* example proc
LRESULT CALLBACK Proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		wind->AddMenus(hWnd);
		wind->AddControls(hWnd);
		break;
	case WM_COMMAND:
		switch (wp)
		{
		case 5:
			DestroyWindow(hWnd);
			break;
		case 1:
			MessageBeep(MB_OK);
			break;
		case 2:
			MessageBoxW(hWnd, L"You so fucking cute!", L"Happy Message!", MB_OK);
			break;
		case 3:
			MessageBoxW(hWnd, L"Ugly Bitch!", L"Angry Message!", MB_OK);
			break;
		//case 4:
		//    wchar_t str(100);
		//    GetWindowText(hEdit,&str,150);
		//    SetWindowTextW(hWnd, &str);
		//    break;
		case 6:
			MessageBoxW(hWnd, L"You Pressed The Button!", L"BUTTON!", MB_OK);
			break;
		case 7:

		}
		break;
	default:
		return DefWindowProcW(hWnd,msg,wp,lp);
		break;
	}
}
*/

struct Menu
{
	HMENU hMenu;
	string MenuID;
};

struct MenuItem
{
	string ParentMenu;
	UINT type;
	UINT_PTR msgID;
	LPCWSTR Text;
};

struct Control
{
	string ControlID;

	LPWSTR ClassName;
	LPWSTR InsideText;
	UINT Style;
	int PosX;
	int PosY;
	int Width;
	int Height;
	HMENU Function;

	HWND CONTROL;
};

struct Vec2 {
	int x;
	int y;
};

class WindowGUI
{
	WNDCLASSW wc;
	int windowWidth;
	int windowHeight;
	int Startx;
	int Starty;
	LPWSTR className;
	LPWSTR Title;

	HINSTANCE hInst;

	vector<Menu> Menus;
	vector<MenuItem> MenuItems;
	vector<Control> Controls;

	



	LRESULT (CALLBACK *Proc)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	

public:
	void WindowLoop();

	//thread WindowLoopThread;

	void AddMenus(HWND hWnd);

	void AddControls(HWND hWnd);

	WindowGUI(int height, int width, int startx, int starty, LPWSTR ClassName, LPWSTR Title, HINSTANCE HInst);

	bool Init(LRESULT(CALLBACK* proc)(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp));

	void AddMenu(string Menu);
	HMENU GetMenu(string Name);
	void AddMenuItem(string ParentMenu, UINT type, UINT_PTR msgID, LPCWSTR Text);

	void AddControl(string controlid,LPWSTR classname, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height, HMENU function);
	HWND GetControl(string controlid);

	void AddButton(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height, HMENU function);
	void AddLabel(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height);
	void AddEdit(string controlid, LPWSTR insidetext, UINT style, int posx, int posy, int width, int height);

	Vec2 GetWindowCenter();
	
	~WindowGUI();
};

