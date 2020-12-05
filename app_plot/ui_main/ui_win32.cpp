#include<string>

#include"ui_win32.h"//interface of this module
#include"../include/Errlog.h"
#include"interpreter.h"

#include "atlbase.h"
#include "atlstr.h"
#include <atlconv.h>

using namespace std;

namespace module_UI 
{
	UI_win32::UI_win32()
		:hDC(NULL), hMainwindow(NULL)
	{}

	void UI_win32::_DrawPixel(unsigned long x, unsigned long y, const PixelAttibute& attr)
	{
		COLORREF wincolor = RGB(attr.red, attr.green, attr.blue);

		//���ò�ͬ��API��������ͬ�Ļ��Ʒ����õ���һ���߼����ص�����Ӳ�ͬ
		SetPixel(hDC, x, y, wincolor);
		for (int sizeH = attr.size; sizeH > 0; --sizeH)
			for (int sizew = attr.size; sizew > 0; --sizew)
			{
				SetPixel(hDC, x + sizew, y + sizeH, wincolor);
			}
	}
	void UI_win32::_ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
		const char* msg)
	{
		MessageBox(hMainwindow, msg, winName.c_str(),
			MB_OK |
			((flag == 0) ? MB_ICONINFORMATION : MB_ICONERROR));
	}

	int UI_win32::prepare(const char* file_name)
	{
		if (UI::prepare(file_name) != 0)
			return 1;

		winName = ("X0DL Interpreter(C++/wsh)--");

		string::size_type p1 = SrcFilePath.rfind('/');
		string::size_type p2 = SrcFilePath.rfind('\\');

		if (p1 == string::npos && p2 == string::npos)
			winName += SrcFilePath;
		else if (p1 != string::npos && p2 != string::npos)
		{
			p1 = (p1 < p2) ? p1 : p2;
			winName += SrcFilePath.substr(p1 + 1);
		}
		else if (p1 == string::npos)
			winName += SrcFilePath.substr(p1 + 1);
		else
			winName += SrcFilePath.substr(p2 + 1);

		//��ʼ�����ڡ�
		if (Preparewindow(GetModuleHandle(NULL)) == 0)
		{
			_ShowMessage(1, "���ڳ�ʼ��ʧ�ܣ�");
			return 1;
		}

		hDC = GetDC(hMainwindow);

		ShowWindow(hMainwindow, SW_SHOWNORMAL);

		return 0;
	}

	void UI_win32::run()
	{
		//Updatewindow(hMainwindow);

		std::string msg("\n����ִ�в��������ļ�:\n");
		msg += LOGFILE_PATH;
		msg += "�洢 �﷨������������ı�\n";
		msg += ERRFILE_PATH;
		msg += "�洢 ����Ĵ�����Ϣ\n\n";

		_ShowMessage(0, msg.c_str());

		//����window��Ϣѭ��
		MSG Msg;
		while (GetMessage(&Msg, NULL, 0, 0))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		return;
	}

//����������ִ���ļ�
	void UI_win32::ProcessFile()
	{
		_ShowMessage(0, "��Ҫ����ִ����...\n");
		if (1 == FDL_interpreter::getInstance()->redoInterpret())
		{
			_ShowMessage(1, "��Ȼ�޷���ȡ�ļ�,��Ҫ�����ˣ�\n");
			PostQuitMessage(0);
		}
	}

	int UI_win32::Preparewindow(HINSTANCE hInst)
	{
		WNDCLASS W;

		int x, y;//�������Ͻǵ�λ��
		int width_screen = GetSystemMetrics(SM_CXSCREEN);
		int height_screen = GetSystemMetrics(SM_CYSCREEN);
		#define MY_WINRATIO 3/5 //2/3

		int width = width_screen * MY_WINRATIO;//���ڵĿ��
		int height = height_screen * MY_WINRATIO;//���ڵĸ߶�
		if (width < (310 << 1))
			width = 310 << 1;
		if (height < (250 << 1))
			height = 250 << 1;

		x = (width_screen - width) / 2;
		y = (height_screen - height) / 2;

		memset(&W, 0, sizeof(WNDCLASS));
		W.style = CS_HREDRAW | CS_VREDRAW;
		W.lpfnWndProc = UI_win32::process_message;
		W.hInstance = hInst;
		W.hCursor = LoadCursor(NULL, IDC_ARROW);
		W.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		W.lpszClassName = winName.c_str();
		RegisterClass(&W);
		hMainwindow = CreateWindow(winName.c_str(), winName.c_str(),
			WS_OVERLAPPEDWINDOW,
			x, y, width, height,
			NULL, NULL, hInst, NULL);
		if (hMainwindow == NULL)
			return 0;

		SetCursor(LoadCursor(hInst, IDC_ARROW));

		return 1;
	}

	LRESULT CALLBACK UI_win32::WndProc(
		HWND hwin,
		UINT Message,
		WPARAM wParam,
		LPARAM lParam)
	{
		switch (Message)
		{
		case WM_DESTROY:
			ReleaseDC(hwin, hDC);
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT pt;
			BeginPaint(hwin, &pt);
			//ÿ����Ҫ���»��ƴ���ʱ,���ٴν���ִ��
			ProcessFile();
			EndPaint(hwin, &pt);
			return 0;
		}
		default:
			break;
		}
		return DefWindowProc(hwin, Message, wParam, lParam);
	}

	/*static*/LRESULT CALLBACK UI_win32::process_message(
		HWND hwin,
		UINT Message,
		WPARAM WParam,
		LPARAM lParam)
	{
		UI_win32* pUI = (UI_win32*)(UI_wrap::getUIObject());
		return pUI->WndProc(hwin, Message, WParam, lParam);
	}

	std::string UI_win32::selectFile()
	{
		OPENFILENAME ofn;
		char szFileName[10 * MAX_PATH];
		char* p;
		int nLen = 0;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		// WCharToMByte(ofn.lpstrFile, szFileName);
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = sizeof(szFileName);
		ofn.lpstrFile[0] = '\0';
		//USES_CONVERSION;
		//ofn.lpstrFile = A2W(szFileName);
		ofn.lpstrFilter = "������ͼ����Դ�ļ�(*,TXT;*.DL)\0*.TXT;*.DL\0�����ļ�(*,*)\0*,*\0\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrTitle = "ѡ��������ĺ�����ͼ����Դ�ļ�";

		if (GetOpenFileName(&ofn))
		{
			//_ShowMessage(e,ofn.1pstrFile);
			/*
			string ret;
			WCharToMByte(ofn.lpstrFile, ret);
			return ret;
			*/
			return ofn.lpstrFile;
		}
		else
		{
			return "";
		}
	}

}


std::wstring  StringToWString(const std::string& s)
{
	std::wstring wszStr;

	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	wszStr = lpwszStr;
	delete[] lpwszStr;
	return wszStr;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR    lpCmdLine,
	_In_ int       nCmdShow)

	/*
	int APIENTRY wWinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,//���������ļ�·��
		int nCmdShow)
		*/
{
	module_UI::UI* pUI = new module_UI::UI_win32;
	/*
	return FDL_interpreter::getInstance()->run(
		lpCmdLine == NULL ? "" : lpCmdLine,
		FDL_interpreter :: USE_UI_WINDOW, pUI);
		*/

	if (lpCmdLine == NULL)
		return FDL_interpreter::getInstance()->run(
			"",
			FDL_interpreter::USE_UI_WINDOW, pUI);
	else
	{
		USES_CONVERSION;
		return FDL_interpreter::getInstance()->run(
			lpCmdLine,
			FDL_interpreter::USE_UI_WINDOW, pUI);
	}
}