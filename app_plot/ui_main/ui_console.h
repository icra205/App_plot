#pragma once
//文件：ui_console.hpp
//内容：基于控制台的界面类型定义（C++语言版）
//作者：WXQ#2019，2818
#ifndef _UI_CONSOLE_HPP
#define _UI_CONSOLE HPP

#include"../include/ui.h"

namespace module_UI
{
	
	//基于控制台的界面类型
	class UI_console :public UI
	{
	public:
		//将主控权交由界面对象后，该对象执行程序的主控逻辑。
		/*Override*/virtual void run();
		//绘制一个点
		/*Override*/virtual void _DrawPixel(
			unsigned long x, unsigned long y,//点在窗口中的坐标
			const PixelAttibute& attr);
		//显示文本
		/*Override*/virtual void _ShowMessage(int flag,//常规类信息=0，错误类信息=1
			const char* msg);
	protected:
		/*Override*/virtual std::string selectFile();
	};
}//namespace module_UI
#endif//#ifndef_UI_CONSOLE_HPP