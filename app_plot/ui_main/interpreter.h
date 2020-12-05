#ifndef _FDL INTERPRETER HPP
#define _FDL_INTERPRETER_HPP

#include<string>

#include"../include/ui.h"

/*本文定义的解释器类采用了设计模式之“singleton pattern”,约束了该类型的实例仅有一个。
*/
//函数绘图语言的解释器类(应用主控类).
class FDL_interpreter
{
public:
	static FDL_interpreter* getInstance();
	enum UI_Kind
	{
		USE_UI_UNKNOWN,
		USE_UI_DEFAULT,
		USE_UI_CONSOLE,//specified by key'con'
		USE_UI_WINDOW,//specified by key‘win'
		USE_UI_WIN_CONSOLE//specified by key'wincon','conwin'
	};

public:

	int run(std::string filePath, UI_Kind uk, module_UI::UI* pUI = NULL);
	//对文件进行(再次)解释执行。
	//这个方法应该可被UI对象多次调用。
	//Return value:
	//8-没有发现任何错误
	//1-文件不可读或读错误
	//2-存在语法错误和/或语义错误
	int redoInterpret();

	//获取被分析的文件路径
	const std::string& getFilePath()const { return mFilePath; }

private:
	FDL_interpreter();

	static FDL_interpreter* mptrInstance;//唯一的解释器对象之句柄
	std::string mFilePath;//被分析的“函数绘图语言源程序”之文件
};

#endif