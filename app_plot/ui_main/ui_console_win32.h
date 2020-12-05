#pragma once
#ifndef _UI_CONSOLE_WIN32_HPP
#define _UI_CONSOLE_WIN32_HPP
#include"ui_win32.h"
namespace module_UI
{ 
	//基于控制台的界面类型
	class UI_console_win32:public UI_win32
	{
	public:
		UI_console_win32();
	~UI_console_win32();

	/*重置基类的public方法*/
	public:
	//界面准备
	/*Override*/virtual int prepare(const char* file_name);
	/*Override*/virtual bool callInterpreterByUI()const 
	{ return false; }
	//绘制一个点
	/*Override*/virtual void _DrawPixel(
		unsigned long x, unsigned long y,//点在窗口中的坐标
		const PixelAttibute& attr);
	//显示文本
	/*Override*/virtual void _ShowMessage(int flag,//常规类信息=e,错误类信息=1
	const char* msg);

	/*重置基类UI_win32的内部方法*/
	protected:
	/*Override*/virtual LRESULT CALLBACK WndProc(HWND hwin,UINT Message,WPARAM WParam,LPARAM lParam);
	/*Override*/virtual void ProcessFile();
	//针对控制台窗口的操作
		private:
		void AttachMyConsole();//关联一个控制台窗口
		void DetachMyConsole();//销毁控制台窗口
		void ClearConsole();
		void prepareMemoryobject();//准备内存缓冲区
	/*数据成员的一部分继承自UI_win32,此处仅需定义新增的*/
		private:
		HDC dcBuffer;//双缓冲需要的内存绘图环境
		HBITMAP memBM;//双缓冲需要的内存图像
	};
}//namespace module_UI
#endif//#ifndef_UI_CONSOLE_WIN32_HPP