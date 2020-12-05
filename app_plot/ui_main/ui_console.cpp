//文件：ui_console.cpp
//内容：基于控制台的界面实现（C++语言版）
//作者：WXQ#2019，2018
/*本文代码基于C/C++的库函数实现*/

#include<cstdio>//fprintf（），stdout
#include<string>
#include<iostream>

#include"ui_console.h"//interface of this module
#include"../include/Errlog.h"

using namespace std;
namespace module_UI
{
	void UI_console::_DrawPixel(unsigned long x, unsigned long y, const PixelAttibute &attr)
	{
		fprintf(stdout, "......DrawPixel（%1d，%1d）with color"
			"[%02X，%02X，%02X] and size %d\n",
			x, y, attr.red, attr.green, attr.blue, attr.size);
	}

	void UI_console::_ShowMessage(int flag, const char* msg)
	{
		if (flag == 0)
			fprintf(stdout, msg);
		else
			fprintf(stderr, msg);
	}

	void UI_console::run()
	{
		std::string msg("\n本次执行产生两个文件：\n");
		msg += LOGFILE_PATH;
		msg += "存储语法分析器的输出文本\n";
		msg += ERRFILE_PATH;
		msg += "存储输出的错误信息\n\n";

		_ShowMessage(0, msg.c_str());
		return;
	}
	std::string UI_console::selectFile()
	{
		cout << "请输入待分析的文件路径并按下回车键：" << endl;
		string yourFile;
		getline(cin, yourFile);
		return yourFile;
	}
}