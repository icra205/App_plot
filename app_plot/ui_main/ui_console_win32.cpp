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

/*ע��Selectobject:ͬһ��GDI�����ܱ����DCѡ��*/
	void UI_console_win32::_DrawPixel(unsigned long x, unsigned long y, const PixelAttibute& attr)
	{
		COLORREF wincolor =/*win32API*/RGB(attr.red, attr.green, attr.blue);
		HPEN oldPen, thePen;//WIN32 ����
		thePen =/*win32API*/::CreatePen(PS_SOLID, 2, wincolor);
		HBRUSH oldBrush, theBrush;//WIN32��ˢ,�������
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

		SHOW_PIXEL(hDC);//��ʾ������,���ͼ�νϸ���,����Թ۲쵽����Ч��
		SHOW_PIXEL(dcBuffer);//Ҳ���Ƶ��ڴ�ͼ����,�������»���ʱ������˸
		
		#undef SHOW_PIXEL

		/*win32API*/::DeleteObject(thePen);
		/*win32API*/::DeleteObject(theBrush);
	};//namespace module_UI 

	void UI_console_win32::_ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
		const char* msg)
	{
		fprintf(flag == 0 ? stdout : stderr, msg);
		if (flag == 1)//���ڴ�����Ϣ,��Ҫ�õ����Ի��򴰿���ʾ��
		{
			UI_win32::_ShowMessage(flag, msg);
		}
	}

	void UI_console_win32::AttachMyConsole()
	{
		/*win32API*/::AllocConsole();//Call WINAPI,create the console
		/*win32API*/::SetConsoleTitle("������ͼ���Խ�����(����̨) - wXQ");
		/*win32API*/::freopen("CONOUT$", "w", stdout);
		/*win32API*/::freopen("CONOUT$", "w", stderr);
		printf("\nThis console attachs a GUI window.\n"
			"\n�������ֻ���ļ�����һ�ν���ִ�С�\n");
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

		//׼����ͼ����
		hDC =/*win32API*/::GetDC(hMainwindow);
		RECT rect;
		/*win32API*/::GetClientRect(hMainwindow, &rect);
		int width_screen =/*win32API*/::GetSystemMetrics(SM_CXSCREEN);
		int height_screen =/*win32API*/::GetSystemMetrics(SM_CYSCREEN);

		//�����ڴ��ͼ�豸���ڴ�ͼ��,�ߴ�ͬ��Ļ�ֱ���
		dcBuffer =/*win32API*/::CreateCompatibleDC(hDC);
		memBM =/*win32API*/::CreateCompatibleBitmap(hDC, width_screen, height_screen);

		/*win32API*/::SelectObject(dcBuffer, memBM);//����DC������λͼ

		RECT areaMemBM = { 0,0,width_screen,height_screen };

		//���ڴ�ͼ��ı������Ϊ��ɫ
		::FillRect(dcBuffer, &areaMemBM,
			(HBRUSH)(GetStockObject(WHITE_BRUSH)));
		//�����ڱ������Ϊ��ɫ
		::FillRect(hDC, &areaMemBM,
			(HBRUSH)(GetStockObject(WHITE_BRUSH)));
	};

	int UI_console_win32::prepare(const char* file_name)
	{
		AttachMyConsole();

		//׼�����ڲ���ʾ
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
		case WM_PAINT://���踴�ƻ�������ͼ��,�ɱ��ⴰ����˸
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
		case WM_RBUTTONUP://����Ҽ�-->���½���ִ��
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
			"������\n\n����Ҽ�,\n\n�Ƿ��ٴν���ִ�У�",
			"XDDL��������ʾ",
			MB_YESNO | MB_ICONINFORMATION);
		if (ok != IDYES)
			return;
		prepareMemoryobject();//clear current image 
		UI_win32::ProcessFile();
	}
}//namespace module_UI
