#include<cstdio>
#include<string>
#include<io.h>
#include<fcntl.h>
#include<cstdlib>

#include"ui_console_win32.h"//interface of this module
#include"interpreter.h"
#pragma warning(disable:4996)

using namespace std;

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

	return wcstring;
}

namespace module_UI 
{
	UI_console_win32::UI_console_win32() :dcBuffer(NULL), memBM(NULL)
	{};
	UI_console_win32::~UI_console_win32()
	{
		/*win32API*/::DeleteObject(memBM);
		/*win32API*/::DeleteDC(dcBuffer);
	};

/*注意Selectobject:同一个GDI对象不能被多个DC选择*/
	void UI_console_win32::_DrawPixel(unsigned long x, unsigned long y, const PixelAttibute& attr)
	{
		COLORREF wincolor =/*win32API*/RGB(attr.red, attr.green, attr.blue);
		HPEN oldPen, thePen;//WIN32 画笔
		thePen =/*win32API*/::CreatePen(PS_SOLID, 2, wincolor);
		HBRUSH oldBrush, theBrush;//WIN32画刷,用于填充
		theBrush =/*win32API*/::CreateSolidBrush(wincolor);

		#define SHOW_PIXEL(H)\
		do  \
		{ \
			oldPen = (HPEN)SelectObject(H,thePen);\
		oldBrush = (HBRUSH)SelectObject(H,theBrush);\
		Ellipse(H,x,y,x + attr.size,y + attr.size);\
		SelectObject(H,oldBrush);\
		SelectObject(H,oldPen);\
		}while(0)

		SHOW_PIXEL(hDC);//显示到窗口,如果图形较复杂,则可以观察到动画效果
		SHOW_PIXEL(dcBuffer);//也绘制到内存图像中,以在重新绘制时避免闪烁
		
		#undef SHOW_PIXEL

		/*win32API*/::DeleteObject(thePen);
		/*win32API*/::DeleteObject(theBrush);
	};//namespace module_UI 

	void UI_console_win32::_ShowMessage(int flag,//常规类信息=e,错误类信息=1
		const char* msg)
	{
		fprintf(flag == 0 ? stdout : stderr, msg);
		if (flag == 1)//对于错误信息,还要用弹出对话框窗口提示。
		{
			UI_win32::_ShowMessage(flag, msg);
		}
	}

	void UI_console_win32::AttachMyConsole()
	{
		/*win32API*/::AllocConsole();//Call WINAPI,create the console
		/*win32API*/::SetConsoleTitle("函数绘图语言解释器(控制台) - wXQ");
		/*win32API*/::freopen("CONOUT$", "w", stdout);
		/*win32API*/::freopen("CONOUT$", "w", stderr);
		printf("\nThis console attachs a GUI window.\n"
			"\n这个程序只对文件进行一次解释执行。\n");
	}

	void UI_console_win32::DetachMyConsole()
	{
		/*win32API*/::FreeConsole();
	}

	void UI_console_win32::ClearConsole()
	{}

	void UI_console_win32::prepareMemoryobject()
	{
		if (NULL != memBM)
		{
			::DeleteObject(memBM);/*win32API*/
			memBM = NULL;
		}
		if (NULL != dcBuffer)
		{
			::DeleteDC(dcBuffer);/*win32API*/
			dcBuffer = NULL;
		}

		//准备绘图环境
		hDC =/*win32API*/::GetDC(hMainwindow);
		RECT rect;
		/*win32API*/::GetClientRect(hMainwindow, &rect);
		int width_screen =/*win32API*/::GetSystemMetrics(SM_CXSCREEN);
		int height_screen =/*win32API*/::GetSystemMetrics(SM_CYSCREEN);

		//创建内存绘图设备及内存图像,尺寸同屏幕分辨率
		dcBuffer =/*win32API*/::CreateCompatibleDC(hDC);
		memBM =/*win32API*/::CreateCompatibleBitmap(hDC, width_screen, height_screen);

		/*win32API*/::SelectObject(dcBuffer, memBM);//设置DC关联的位图

		RECT areaMemBM = { 0,0,width_screen,height_screen };

		//将内存图像的背景填充为白色
		::FillRect(dcBuffer, &areaMemBM,
			(HBRUSH)(GetStockObject(WHITE_BRUSH)));
		//将窗口背景填充为白色
		::FillRect(hDC, &areaMemBM,
			(HBRUSH)(GetStockObject(WHITE_BRUSH)));
	};

	int UI_console_win32::prepare(const char* file_name)
	{
		AttachMyConsole();

		//准备窗口并显示
		if (UI_win32::prepare(file_name) != 0)
			return 1;

		prepareMemoryobject();

		return 0;
	};

	LRESULT CALLBACK UI_console_win32::WndProc(
		HWND hwin,
		UINT Message,
		WPARAM wParam,
		LPARAM lParam)
	{
		switch (Message)
		{
		case WM_PAINT://仅需复制缓冲区的图像,可避免窗口闪烁
		{
			if (NULL == dcBuffer)
				return 0;

			PAINTSTRUCT pt;
			HDC hDCPaint = BeginPaint(hwin, &pt);
			RECT rect = pt.rcPaint;
			BitBlt(hDCPaint, rect.left, rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top, dcBuffer,
				rect.left, rect.top, SRCCOPY);
			EndPaint(hwin, &pt);
			return 0;
		}
		case WM_RBUTTONUP://鼠标右键-->重新解释执行
			ProcessFile();
			return 0;

		case WM_DESTROY:
			DetachMyConsole();
			//and goto default:
		default:
			break;
		}

		return UI_win32::WndProc(hwin, Message, wParam, lParam);
	}

	void UI_console_win32::ProcessFile()
	{
		int ok = MessageBox(hMainwindow,
			"你点击了\n\n鼠标右键,\n\n是否再次解释执行？",
			"XDDL解释器提示",
			MB_YESNO | MB_ICONINFORMATION);
		if (ok != IDYES)
			return;
		prepareMemoryobject();//clear current image 
		UI_win32::ProcessFile();
	}
}//namespace module_UI
