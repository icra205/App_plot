#include"interpreter.h"//my interface
#include"ui_console.h"
#include"ui_console_win32.h"
#pragma warning(disable:4996)
#include"../include/Errlog.h"
#include"../include/semantics.h"

using namespace std;
using namespace module_UI;
using namespace module_Lexer;
using namespace module_Parser;
using namespace module_ErrLog;
using namespace module_Semantic;

// 定义类的静态数据成
FDL_interpreter* FDL_interpreter::mptrInstance = NULL;

FDL_interpreter* FDL_interpreter::getInstance()
{
	if (NULL == mptrInstance)
	{
		mptrInstance = new FDL_interpreter();
	}
	return mptrInstance;
}

FDL_interpreter::FDL_interpreter()
{}

int FDL_interpreter::run(std::string filepath, UI_Kind uk, module_UI::UI* pUI)
{
	if (NULL == pUI)
	{
		switch (uk)
		{
		case USE_UI_CONSOLE:
			pUI = new module_UI::UI_console();
			break;
		case USE_UI_WINDOW:
			pUI = new module_UI::UI_win32();
			break;
		default:
			pUI = new module_UI::UI_console_win32();
			break;
		}
	}

	module_UI::UI_wrap::setUIObject(pUI);

	if (filepath.length() > 0 && filepath[0] != '-')
		mFilePath = filepath;
	else
	{
		//具体界面对象来选择一个文件吧
		std::string youFilepath = pUI->selectFile();
		if (youFilepath.length() == 0)
			return 1;

		FILE* fp = fopen(youFilepath.c_str(), "r");
		if (fp == NULL)
		{
			pUI->_ShowMessage(1, "你选择的文件不可读呀！");
			return 1;
		}
		fclose(fp);
		mFilePath = youFilepath;

		if (pUI->prepare(mFilePath.c_str()) != 0)
		{
			return 3;
		}

		int hasError = 0;
		if (false == pUI->callInterpreterByUI())
		{
			//若界面对象不主动调用“解释执行”,
			//就在这里调用
			hasError = redoInterpret();
		}

		if (hasError != 1)
			pUI->run();

		delete pUI;

		return hasError;
	}
}

int FDL_interpreter::redoInterpret()
{
	Lexer theScanner;
	if (0 == theScanner.InitScanner(mFilePath.c_str()))
	{
		char msg[2048];
		snprintf(msg, sizeof(msg) - 1, "\n打开源程序文件失败,文件名是[%s]！\n",
			mFilePath.c_str());
		module_UI::UI_wrap::ShowMessage(1, msg);
		return 1;
	}
	Parser theParser(&theScanner);

	//语法分析器+词法分析器构成了解释器的前端
	//语义分析器则是解释器的后端。
	SemanticAnalyzer backEnd(&theParser);

	ErrLog::reOpen(true);
	int hasError = 0;
	try
	{
		//module_UI::UI_wrap::ShowMessage(0,“开始进行语法分析..\n");
		AST_stmt_list* stmts = theParser.run();
		if (ErrLog::error_count() > 0)
		{
			module_UI::UI_wrap::ShowMessage(1,
				"\n\n啊哈,虽然有语法错误、词法错误,但管他呢 :- (\n\n");
		}

		if (stmts != NULL)
		{
			//module_UI::UI_wrap::ShowMessage(e,“开始语义分析(解释)..\n");
			backEnd.run(stmts);
			//module_UI::UI_wrap::ShowMessage(e,“语义分析(解释)结束了\n");
			delete stmts;
		}
		hasError = ErrLog::error_count();
	}
	catch (...)
	{
		hasError = 2;
	}
	ErrLog::reOpen(false);
	return hasError < 1 ? 0 : 2;
}