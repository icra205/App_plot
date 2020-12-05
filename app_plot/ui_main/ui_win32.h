#pragma once
#ifndef _UI_WINDOW32_HPP
#define _UI_WINDOW32_HPP
/*本模块基于Windows API实现,也定义了
“Win32Application”型应用的主函数WinMain()
*/

#include <windows.h>//windows API
#include <wingdi.h>
//#include <winuser.h>//include windows.h

#include<string>
#include"../include/ui.h"

namespace module_UI
{ 
//基于控制台的界面类型
	class UI_win32 :public UI
	{
	public:
		//界面准备
		/*override*/virtual int prepare(const char* file_name);
		/*Override*/virtual bool callInterpreterByUI()const
		{
			return true;
		}

		//将主控权交由界面对象后,该对象执行程序的主控逻辑,
		/*Override*/virtual void run();
		//绘制一个点
		/*Override*/virtual void _DrawPixel(
			unsigned long x, unsigned long y,//点在窗口中的坐标
			const PixelAttibute& attr);
		//显示文本
		/*override*/virtual void _ShowMessage(int flag,//常规类信息=e,错误类信息=1
			const char* msg);

	public:
		UI_win32();

	protected:
		//初始化窗口
		virtual int Preparewindow(HINSTANCE hInst);

		//窗口处理函数
		virtual LRESULT CALLBACK WndProc(HWND hWin, UINT Message, WPARAM wParam, LPARAM lParam);
		//分析并解释执行文件
		virtual void ProcessFile();

		//消息处理函数
		static LRESULT CALLBACK
			process_message(
				HWND hwin,
				UINT Message,
				WPARAM wParam,
				LPARAM lParam);

	protected:
		/*Override*/virtual std::string selectFile();

	protected:
		HDC hDC;//handle of display device context 
		HWND hMainwindow;//handle of main window 

		std::string winName;//窗口名
	};
}//namespace module_UI
#endif//#ifndef_UI_WINDOW32_HPP