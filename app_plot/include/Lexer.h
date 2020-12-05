#pragma once
//文件:lexer.hpp
//内容:声明词法分析器的对外接口(C++面向对象版)
//作者:WXQ#2019,2018
/*关于输入中的存在错误之设计
*1.词法分析阶段只能识别到“无效字符”、“无效单词”这两种错误。
*2.对于以上错误,词法分析器向调用者返回的记号之种类统一填写为"ERRTOKEN".
*3.对于“无效字符”,还会被词法分析内部直接丢弃,但将该字符转换为一个16进制
*整数存放到记号的文本中,以便调用者得到。
*4.所有错误,需要调用者(语法分析器,或词法测试程序)来处理。
*/
#ifndef _LEXER_HPP
#define _LEXER_HPP
#include <cstdio>//for FILE
#include<string>//for std::string 

namespace module_Lexer
{
	//
	//词法分析器提供的类型定义与常量声明
	//

	enum Token_Type
	{
		COMMENT,//用于处理行注释
		ID,//用于进一步区分各保留字、常量名、函数名,或不认识的标识符

		ORIGIN,//关键字origin 
		SCALE,//关键字scale 
		ROT,//关键字rot 
		IS,//关键字is
		TO,//关键字to 
		STEP,//关键字step 
		DRAW,//关键字 draw 
		FOR,//关键字for 
		FROM,//关键字from 
		T,//参数,语言内置的唯一的变量

		COLOR,//偷偷增加的关键字,用于颜色设置语句
		SIZE,//偷偷增加的关键字,用于设置点大小语句

		SEMICO,//特殊符号:分号
		L_BRACKET,//特殊符号:左圆括号
		R_BRACKET,//特殊符号:右圆括号
		COMMA,//特殊符号:分隔符号
		PLUS,//运算符:加号
		MINUS,//运算符:减号
		MUL,//运算符:乘号
		DIV,//运算符:除号
		POWER,//运算符:乘方
		FUNC,//函数名(认识的)
		CONST_ID,//常数(命名的or数值字面量)
		NONTOKEN,//空记号,用于表示输入结束了
		ERRTOKEN//出错记号
	};

	const char* token_type_str(enum Token_Type tk);

	typedef unsigned int uint;
	class position//记号所在位置的数据类型
	{
	private:
		uint _line;//行号
		uint _col;//列号
	public:
		position(uint L = 1, uint C = 0) : _line(L), _col(C) {}
		void set(uint L, uint C) { _line = (L), _col = (C); }
		void nextLine() {
			++_line; _col = 0;
		}
		position& operator++() {
			++_col; return *this;
		}
		position& operator--() {
			if (--_col == 0xFFffFFff)//防止调用者逻辑错误
				_col = 0;
			return *this;
		}
		uint line()const { return _line; }
		uint column()const { return _col; }
	};

	//支持的被调用函数之原型/类型
	typedef double(*t_func)(double);

	//记号的基本数据结构
	struct basic_token {
		Token_Type type = ERRTOKEN; //记号的类别
		std::string lexeme;//构成记号的字符串
		double value = 0; //若为常数,则是常数的值
		t_func FuncPtr = NULL;//若为函数,则是函数的指针
		void clear()
		{
			type = ERRTOKEN;;
			value = 0;
			FuncPtr = NULL;
			lexeme.clear();
		}
	};

	//记号对象的类型
	class Token {
	public:
		Token();
		Token(const position& w);
		Token(const basic_token& base);

		//all Setters 
		void clear(bool resetPosition = false);//将记号对象重置为初始状态
		void setPosition(const position& w);//
		void setBasic(const basic_token& base);//填写记号的基础数据
		void setType(Token_Type t);//修改记号类别
		void setText(const char* str);//修改记号的文本
		void setText(const std::string& str);//修改记号的文本
			//将字符c追加到记号文本。
		void appendText(char c);
		//将字符串str追加到记号文本之末尾。
		void appendText(const char* str);

		//将无效字符c填写为记号文本
		void setChar(int c);
		//对于CONST_ID,填写其数值
		void setValue(double v);

		//all Getters 

		Token_Type getType()const { return basic.type; }
		//是否为空记号
		bool is_empty()const;
		const std::string& getText()const { return basic.lexeme; }
		std::string getUpperText()const;
		double getValue()const { return basic.value; }
		t_func getFuncPtr()const { return basic.FuncPtr; }
		position getPosition()const { return where; }

		static std::string str2upper(const std::string& txt);
	private:
		//记号的基本信息
		basic_token basic;
		//记号在输入中的位置
		position where;
	};

	//
	//Part II 操作接口
	//

	namespace DFA//sub-module of module_Lexer 
	{
		class dfa;//class body is hidden from user
	}

	//词法分析器的类型
	class Lexer {
	public:
		Lexer();
		~Lexer();//关闭词法分析器,cal1CloseScanner()

			//初始化词法分析数据源,成功时返回非0,失败返回0
		int InitScanner(const char* fileName);
		const char* getFilename()const
		{
			return file_name.c_str();
		}
		//识别并返回一个记号。
		//遇到非法输入时.type=ERRTOKEN、文件结束时.type=NONTOKEN 
		Token GetToken();
		//识别并返回一个记号(GetToken()的别名)。
		//遇到非法输入时.type=ERRTOKEN、文件结束时.type=NONTOKEN 
		Token nextToken() { return GetToken(); }

	private:
		void CloseScanner();//关闭词法分析器
		int pre_process(Token* pToken);//识别记号前的预处理
		int scan_move(Token* pToken, int first_char);//边扫描边转移
		int post_process(Token* pToken, int last_state);//识别记号的后处理

		char GetChar();//从输入源程序中读入一个字符
		void BackChar(char c);//把预读的字符退回到输入源程序中
		int is_space(char c);//字符c是否为空白字符

		//判断所给的字符串是否在符号表中
		basic_token* JudgekeyToken(const std::string& str);
		void InitSymbolTable();//初始化符号表

	protected:
		std::FILE* in_flie;//输入文件流,使用C语言的文件操作API 
		DFA::dfa* pDFA;//辅助数据,指向DFA对象
		position current_pos;//当前文件位置(行号,列号)
		std::string file_name;//被分析的文件名
	};
}//namespace module_Lexer

#endif//#ifndef_LEXER_HPP
//end of file