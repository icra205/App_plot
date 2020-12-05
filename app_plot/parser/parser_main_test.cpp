//�ļ�:parser_main.cpp
//����:�﷨����������������(C++���԰�)
//����:WXQ#2019,2018

#ifdef TEST_PARSER//��������ʱ�����������,�Ž��дʷ�����

#include <iostream>
#include<iomanip>

#include"../include/Parser.h"
#include"../include/Errlog.h"

using namespace std;
using namespace module_Lexer;
using namespace module_Parser;
using namespace module_ErrLog;

//�������﷨����,������
void doparse(const char* file_name)
{
	ErrLog::reOpen(true);

	module_Lexer::Lexer theScanner;
	if (0 == theScanner.InitScanner(file_name))
	{
		ErrLog::error_msg("Open file [%s] fail.\n", file_name);
		return;
	}

	module_Parser::Parser theParser(&theScanner);//������ָ��һ���ʷ�������
	try
	{
		AST_stmt_list* stmts = theParser.run();
		if (stmts != NULL)
			delete stmts;
	}
	catch (...)
	{
	}
	ErrLog::reOpen(false);

	printf("\n�﷨�������Խ���.\n��������ʹ�����Ϣ���Ķ����ɵ��ļ�:\n"
		"(1) %s\n(2) %s\nBye Bye.\n",
		LOGFILE_PATH, ERRFILE_PATH);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "please specify the Source File !\n";
		return 1;
	}

	doparse(argv[1]);

	return 0;
}
#endif//#ifdef TEST_PARSER