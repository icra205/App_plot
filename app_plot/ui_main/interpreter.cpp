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

// ������ľ�̬���ݳ�
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
		//������������ѡ��һ���ļ���
		std::string youFilepath = pUI->selectFile();
		if (youFilepath.length() == 0)
			return 1;

		FILE* fp = fopen(youFilepath.c_str(), "r");
		if (fp == NULL)
		{
			pUI->_ShowMessage(1, "��ѡ����ļ����ɶ�ѽ��");
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
			//����������������á�����ִ�С�,
			//�����������
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
		snprintf(msg, sizeof(msg) - 1, "\n��Դ�����ļ�ʧ��,�ļ�����[%s]��\n",
			mFilePath.c_str());
		module_UI::UI_wrap::ShowMessage(1, msg);
		return 1;
	}
	Parser theParser(&theScanner);

	//�﷨������+�ʷ������������˽�������ǰ��
	//������������ǽ������ĺ�ˡ�
	SemanticAnalyzer backEnd(&theParser);

	ErrLog::reOpen(true);
	int hasError = 0;
	try
	{
		//module_UI::UI_wrap::ShowMessage(0,����ʼ�����﷨����..\n");
		AST_stmt_list* stmts = theParser.run();
		if (ErrLog::error_count() > 0)
		{
			module_UI::UI_wrap::ShowMessage(1,
				"\n\n����,��Ȼ���﷨���󡢴ʷ�����,�������� :- (\n\n");
		}

		if (stmts != NULL)
		{
			//module_UI::UI_wrap::ShowMessage(e,����ʼ�������(����)..\n");
			backEnd.run(stmts);
			//module_UI::UI_wrap::ShowMessage(e,���������(����)������\n");
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