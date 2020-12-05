//文件:ui_wrap.cpp
//内容:人机界面包装类UI_wrap的实现(C++语言版)
//作者:WXQ#2019,2018

#include <cstdio>//fprintf(),stdout

#include"../include/ui.h"
#include"./ui_console.h"

using namespace std;
namespace module_UI
{
	int UI::prepare(const char* file_name)
	{
		SrcFilePath = file_name;
		return 0;
	}
	UI* UI_wrap::uiObj = NULL;//人机界面对象

	void UI_wrap::setUIObject(UI* p_obj)
	{
		uiObj = p_obj;
	}
	UI* UI_wrap ::getUIObject() { return uiObj; }

	void UI_wrap::DrawPixel(
		unsigned long x, unsigned long y,//点在窗口中的坐标
		const UI::PixelAttibute& attr)
	{
		if (uiObj == NULL)
			uiObj = new UI_console();

		uiObj->_DrawPixel(x, y, attr);
	}

	//显示文本
	void UI_wrap::ShowMessage(int flag,//常规类信息=e,错误类信息=1
		const char* msg)
	{
		if (uiObj == NULL)
		{
			uiObj = new UI_console();
		}
		uiObj->_ShowMessage(flag, msg);
	}
}//namespace module_UI