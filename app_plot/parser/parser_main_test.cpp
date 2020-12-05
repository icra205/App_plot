//文件:parser_main.cpp
//内容:语法分析器测试主程序(C++语言版)
//作者:WXQ#2019,2018

#ifdef TEST_PARSER//仅当编译时定义了这个宏,才进行词法测试

#include <iostream>
#include<iomanip>

#include"../include/Parser.h"
#include"../include/Errlog.h"

using namespace std;
using namespace module_Lexer;
using namespace module_Parser;
using namespace module_ErrLog;

//仅进行语法分析,调试用
void doparse(const char* file_name)
{
	ErrLog::reOpen(true);

	module_Lexer::Lexer theScanner;
	if (0 == theScanner.InitScanner(file_name))
	{
		ErrLog::error_msg("Open file [%s] fail.\n", file_name);
		return;
	}

	module_Parser::Parser theParser(&theScanner);//参数来指定一个词法分析器
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

	printf("\n语法分析测试结束.\n分析结果和错误信息请阅读生成的文件:\n"
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