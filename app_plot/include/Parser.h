#pragma once
//文件:parser.hpp
//内容:声明语法分析器的对外接口(C++面向对象版)
//作者:WXQ#2019,2818

#ifndef _PARSER_HPP
#define _PARSER_HPP

#include"../include/Lexer.h"//词法分析器接口声明
#include"../include/art.h"//语法树类型声明

namespace module_Parser
{

	//语法分析器的部分设计要点:
	//1.该分析器要为全部输入生成一个完整的语法树。
	//当语法分析器对象消亡时,此树不会自动销毁,而是要求调用者销毁。//2.其他程序可对该语法树进行多种处理。其中语义分析模块所做的
	//就是计算表达式的值、绘图,应该还有语义检查。
	//3.语法分析器的核心基于“递归下降分析”方法构造的。即文法中的每个非终结符,
	//都定义为类型Parser的一个private或者protected 成员函数。

	//语法分析器的类型
	class Parser
	{
	public://供其他模使用的接口操作
		Parser(module_Lexer::Lexer* needscanner);//指定已经准备好的词法分析器
		virtual ~Parser();

		//语法分析的入口操作。
		//返回为完整输入构造的语法树(语句列表).
		//如果输入存在至少一条语句则返回非空的树.
		virtual AST_stmt_list* run();

		//返回被分析的文件名
		const char* getFilename()const;
		//返回为整个输入构造的语法树(语句列表)
		AST_stmt_list* getstmts()const { return ASTroot; }

		//获得在AST中,变量T的存储空间。
		double* getTmemory()const { return ptr_t_storage; }
		//重置变量T的存储空间。
		//！该方法必须在语法分析前调用。
		void setTmemory(double* t) { ptr_t_storage = t; }

	protected://辅助函数:非终结符对应的子程序。允许子类重置。
		virtual void program();
		virtual AST_statement* statement();
		virtual AST_statement* for_statement();
		virtual AST_statement* origin_statement();
		virtual AST_statement* rot_statement();
		virtual AST_statement* scale_statement();
		virtual AST_statement* color_statement();
		virtual AST_statement* size_statement();
		virtual AST_expression* expression();
		virtual AST_expression* term();
		virtual AST_expression* factor();
		virtual AST_expression* component();
		virtual AST_expression* atom();

	protected:
		AST_stmt_list* ASTroot;//整个输入的抽象语法树
		double* ptr_t_storage;//T值的存放空间
		double parameter;//default storage of T

		module_Lexer::Token curr_token;//当前记号
		module_Lexer::Token lastToken;//当前记号之前,最后一个正确的记号

	private://语法分析的辅助成分
		module_Lexer::Lexer* theScanner;
		int indent;//显示信息的缩进量,用于 PrintsyntaxTree()

		module_Lexer::Token_Type FetchToken();//获取下一记号
		void MatchToken(enum module_Lexer::Token_Type expected);//匹配记号(终结符)
		void SyntaxError(int case_of);//报告语法错误
		void PrintSyntaxTree(AST* root, int arg_indent);//打印语法树
		AST_expression* MakeExprNode(module_Lexer::Token* p_rt, ...);//为表达式构造语法树

		//下列函数用于跟踪调试,显示进入、退出哪个非终结符的函数。
		void enter(const char* nonTerminalName);
		void exit(const char* nonTerminalName);
		void ast_dump(AST* root);
	};
}//namespace module_Parser

#endif//#ifndef_PARSER_HPP