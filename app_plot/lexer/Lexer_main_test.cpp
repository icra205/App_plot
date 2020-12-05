//文件:lexer_main.cpp
//内容:词法分析器测试主程序(C++语言版)
//作者:WXQ#2019,2018

// #define TEST_LEXER
#ifdef TEST_LEXER//仅当编译时定义了这个宏,才进行词法测试
#include<iostream>
#include<iomanip>
#include"../include/Lexer.h"

using namespace std;
using namespace module_Lexer;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "please specify the Source File！\n";
		return 1;
	}

	Lexer lexer;
	if (0 == lexer.InitScanner(argv[1]))//初始化词法分析器
	{
		cerr << "Open Source File Error！\n";
		return 2;
	}
	cout << "记号类别     字符串            常数值      函数指针       位置\n";
	cout << "_________________________________________________________________________\n";

	Token token;
	while (1)
	{
		token = lexer.GetToken();//获得记号
		if (token.getType() != NONTOKEN)//打印记号的内容
		{
			//"%-10s%125%12f%12p(%d,%d)\n",
			cout << setiosflags(ios::left) << setw(12)
				<< token_type_str(token.getType()) << " "
				<< setw(12) << token.getText() << " "
				<< resetiosflags(ios::left);
			cout << setw(12) << token.getValue() << " "
				<< setw(12) << (void*)(token.getFuncPtr()) << "("
				<< token.getPosition().line() << ","
				<< token.getPosition().column() << ")\n";
		}
		else break;
	};
	cout << "_________________________________________________________________________\n";
	//函数结束时:
	//~1exer()将被自动调用
	return 0;
}
#endif//#ifdef TEST_LEXER