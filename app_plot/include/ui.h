#pragma once
//文件:ui.hpp
//内容:人机界面操作接口声明(C++面向对象版).
//作者:WXQ#2019,2018

#ifndef _UI HPP
#define _UI_HPP

#include<string>//for file path 

namespace module_UI {

	//人机界面类型的顶层基类。
	//定义了所有界面对象的公共接口。
	class UI
	{
		public:
			struct PixelAttibute//像素点的属性类型
		{
			int size;//像素点的显示尺寸,缺省为1
				unsigned char red;//颜色之红色分量
				unsigned char green;//颜色之绿色分量
				unsigned char blue;//颜色之蓝色分量

				PixelAttibute(unsigned char r = 255,
				unsigned char g = 0,
				unsigned char b = 0,
				int sz = 1)
				:red(r),green(g),blue(b),size(sz)
			{
			}
			void setColor(unsigned char r,unsigned char g,unsigned char b)
			{
				red = r,green = g;blue = b;
			}
				void setSize(int sz)
			{
				size = sz;
			}
		};

	public://所有种类的界面对象均定义各自的操作实现
		//界面准备。准备好之后就可以绘图了。
		//若准备成功则返回0,否则返回非0。
		virtual int prepare(const char* file_name);

			//“解释执行”任务是否由UI对象自主调用.
			//若是,则主控不调用“解释执行”。
			virtual bool callInterpreterByUI()const
		{
		return false;
		}

			//将主控权交由界面对象后,该对象维护/维持窗口状态直到关闭.
			virtual void run() = 0;

			//绘制一个点
			virtual void _DrawPixel(
			unsigned long x,unsigned long y,//点在窗口中的坐标
			const PixelAttibute & attr) = 0;

			//显示文本信息。
			virtual void _ShowMessage(int flag,//常规类信息=e,错误类信息=1
			const char* msg) = 0;
			//选择一个待分析的函数绘图语言源文件.
			//***当命令行未指定文件时,程序主控调用该方法。
			//Return:
			//若未选择文件则返回空串,否则
			//返回所选择文件的路径。
			virtual std::string selectFile() = 0;
			virtual~UI(){}

			protected:
			std::string SrcFilePath;//用于存放源程序文件路径
	};

	//人机界面对象的包装类型.
	//定义该类以便于外部调用相关操作。
	//**若没有UI对象,则等同于在控制台显示信息,不能绘图的。
	class UI_wrap
	{
	public://为方面其他模块调用,全定义为静态方法
	//设置起作用的UI对象
		static void setUIObject(UI* p_obj);
		//返回起作用的UI对象
		static UI* getUIObject();

		//绘制一个点
		static void DrawPixel(
			unsigned long x, unsigned long y,//点在窗口中的坐标
			const UI::PixelAttibute& attr);

		//显示文本
		static void ShowMessage(int flag,//常规类信息=0,错误类信息=1
			const char* msg);

	private:
		static UI* uiObj;//全局唯一的人机界面对象

		UI_wrap(int);//disable create any instance
	};
}
#endif