//文件:lexer.cpp
//内容:词法分析器类Lexer的实现(C++语言版)
//作者:WXQ#2019,2018

#include<cstdio>//fopen(),fclose(),getc(),ungetc()
#include<cstring>//memset()
#include <cctype>//isspace(),toupper()
#include <cstdlib>//atof()of GCC
#include<cmath>//atof(),sin(),cos(),...

#include<map>//内置符号表的存储结构map

#include"../include/Lexer.h"//interface of class Lexer,Token,etc.
#include"dfa.h"//interface of class DFA 

using namespace std;

namespace module_Lexer
{ 
//
//Part I Implementation of class Token
//

	Token::Token() :where(0, 0)
	{
		clear();
	};

	Token::Token(const position& w) :where(w)
	{
		clear();
	};

	Token::Token(const basic_token& base) :basic(base), where(0, 0)
	{};

	void Token::clear(bool resetPosition)
	{
		basic.clear(); if (resetPosition)
			this->where.set(0, 0);
	};
	void Token::setBasic(const basic_token& base)
	{
		basic = base;
	};

	inline void Token :: setType(Token_Type t) { basic.type = t; };

	bool Token::is_empty() const
	{
		if (basic.lexeme.length() < 1)
			return true;
		else
			return false;
	};

	inline void Token::setChar(int c)
	{
		//对于可见字符,填写它。从！’到～
		if (32 < c && c <= 126)
		{
			basic.lexeme = char(c);
		}
		else
		{
			char str[10];
			snprintf(str, sizeof(str), "\\X%02X", (unsigned char)c);
			basic.lexeme = str;
		}
	};

	inline void Token::setValue(double v)
	{
		if (basic.type == CONST_ID)
			basic.value = v;
	};

	void Token::setText(const char* str)
	{
		if (NULL != str)
			this->basic.lexeme = str;
	};

	void Token::setText(const std::string& str)
	{
		this->basic.lexeme = str;
	};

	//将字符c追加到记号文本末尾.
	inline void Token::appendText(char c)
	{
		this->basic.lexeme += (c);
	};
	void Token::appendText(const char* str)
	{
		if (NULL != str)
			this->basic.lexeme += str;
	};

	inline void Token::setPosition(const position& w)
	{
		where = w;
	};

	/* static member*/
	std::string Token::str2upper(const std::string& txt)
	{
		string upperText;
		for (string::const_iterator c = txt.begin(); c != txt.end(); ++c)
		{
			upperText += toupper(*c);
		}
		return upperText;
	};

	std::string Token::getUpperText() const
	{
		return str2upper(basic.lexeme);
	};

	//
	//Part II Implementation of class Lexer
	//

	Lexer::Lexer()
	{
		in_flie = NULL;
		pDFA =//下面是表驱动型DFA 
			DFA::dfa::get_table_driven_dfa();
		//下面是直接编码型DFA
		//DFA::dfa::get_hardCode_dfa();
	};

	Lexer::~Lexer()
	{
		CloseScanner();
	};

	//接口操作1:初始化词法分析器
	int Lexer::InitScanner(const char* fileName)
	{
		current_pos.set(1, 0);
		InitSymbolTable();
		
		errno_t err;
		err = fopen_s(&in_flie,fileName, "r");
		if (err == 0)
		{
			this->file_name = fileName;
			return 1;
		}
		else
			return 0;
	};

	//接口操作2:关闭词法分析器
	void Lexer::CloseScanner()
	{
		if (in_flie != NULL)
			fclose(in_flie);
		in_flie = NULL;
	};

	//接口操作3:识别并返回一个记号。
	//遇到非法输入时.type=ERRTOKEN、文件结束时.type=NONTOKEN 
	Token Lexer::GetToken()
	{
		int first_char;//记号开始的第1个字符
		int last_state = -1;//识别记号结束时的状态
		position where;//当前记号的起始位置
		Token theToken;//被识别到的记号对象
		int to_bo_continue;

		do
		{
			theToken.clear();

			//第1步:预处理,跳过空白字符
			first_char = pre_process(&theToken);
			if (first_char == -1)//文件结束了
			{
				theToken.setPosition(current_pos);//结束的位置
				theToken.setType(NONTOKEN);
				return theToken;
			}
			where = current_pos;
			theToken.setPosition(where);//当前记号开始的位置

			//第2步:边扫描输入,边转移状态
			last_state = scan_move(&theToken, first_char);

			//第3步:后处理:根据终态所标记的记号种类信息,进行特殊处理
			to_bo_continue = post_process(&theToken, last_state);
		} while (to_bo_continue != 0);

		//theToken.setPosition(where);//修正记号的位置
		return theToken;
	};

	//
	//下面是辅助函数、辅助变量等定义
	//

	//从输入源程序中读入一个字符并返回它
	//若遇到文件结束则返回-1.
	char Lexer::GetChar()
	{
		int next_char = getc(in_flie);
		if (EOF == next_char)
			return -1;
		else
		{
			if ('\n' == next_char)
			{
				current_pos.nextLine();
				return next_char;
			}
			else
			{
				++current_pos;
				//////return toupper(next_char);//统一返回大写
				return next_char;
			}
		}
	};

	//把预读的字符退回到输入源程序中
	void Lexer::BackChar(char next_char)
	{
		//文件结束标志、换行不回退
		if (next_char == EOF || next_char == '\n')
			return;

		ungetc(next_char, in_flie);
		--(current_pos);
	};

	int Lexer::is_space(char c)
	{
		if (c < 0 || c>0x70)//处理非ASCII字符,如(半个)中文字符
			return 0;
		return isspace(c);
	};

	//识别一个记号的预处理:跳过空白字符,并读取第1个非空白字符。
	//返回值:-1文件结束,其他值表示字符本身.
	int Lexer::pre_process(Token* pToken)
	{
		int current_char;//当前读到的字符
		for (;;)
		{
			current_char = GetChar();
			if (current_char == -1)
			{
				return -1;
			}
			if (!is_space(current_char))
				break;
		}//end of for
		//
		//此时,current_char就是记号的第1个字符
		return current_char;
	};

	//识别记号的核心操作:边扫描边进行状态转移
	int Lexer::scan_move(Token* pToken, int first_char)
	{
		int current_state, next_state;//当前状态,下一状态
		int current_char;//当前字符

		current_char = first_char;
		current_state = pDFA->get_start_state();
		for (;;)
		{
			next_state = pDFA->move(current_state, current_char);
			if (next_state < 0)//没有转移了
			{
				//第一个字符就无效,则丢弃它.否则因为反复读到该字符而陷入死循环
				if (pToken->is_empty())
				{
					pToken->setChar(current_char);
				}
				else
				{//退回当前字符,它应该是空白字符,或者是下一记号的开始.
					BackChar(current_char);
				}
				break;
			}

			pToken->appendText(current_char);//追加记号的文本

			current_state = next_state;
			current_char = GetChar();
			if (current_char == -1)//文件结束了
				break;
		}
		return current_state;
	};

	//根据终态所标记的记号种类信息,进行特殊处理.
	//若返回非0,则表示当前刚处理完了“注释”,需要调用者接着获取下一个记号。
	int Lexer::post_process(Token* pToken, int last_state)
	{
		int to_bo_continue = 0;//FALSE 
		enum Token_Type tk = pDFA->state_is_final(last_state);
		pToken->setType(tk);
		switch (tk)
		{
		case ID://查符号表,进一步计算记号信息
			{
				basic_token* id = JudgekeyToken(pToken->getText());
				if (NULL == id)//我不认识它,交给语法分析器去处理吧
					pToken->setType(ID);//ERRTOKEN);
				else
				{
					pToken->setBasic(*id);
				}
			}
			break;
		case CONST_ID://转为数值
			pToken->setValue(atof(pToken->getText().c_str()));
			break;
		case COMMENT://行注释:忽略直到行尾的文本,
		//并通过返回值告知调用者读取下一个记号
		{
			int c;
			while (1)
			{
				c = GetChar();
				if (c == '\n' || c == '\r' || c == -1)
					break;
			}
		}
		to_bo_continue = 1;//TRUE 
		break;
		default:
			break;
		}
		return to_bo_continue;
	};

	//下面的程序定义为匿名名字空间的成员,以限制仅能
	//被当前文件调用。
	namespace
	{
		//该函数用于示范:
		//解释器中可以对用户提供任意的API函数,
		//只需其需要一个double型参数,返回double即可。
		//就好比我们用C编程时可以调用cos(x)那样。
		double demo_ayy(double)
		{
			return 2019.07 - 2018.10;
		};

		/*符号表定义(内容暂固定)*/
		struct basic_token TokenTab[] =
		{
			/*每条数据的第2分量(符号文本)必须全部大写*/
			{CONST_ID,"PI",3.1415926,NULL},//命名常数
			{CONST_ID,"E",2.71828,NULL},

			{CONST_ID,"XD",10701,NULL},//新增的常量名
			{CONST_ID,"WXQ",5.28,NULL},//新增的常量名

			{T,"T",0.0,NULL},//参数

			{FUNC,"SIN",0.0,sin},//支持的函数名
			{FUNC,"COS",0.0,cos},//这些函数都是math.h的
			{FUNC,"TAN",0.0,tan},//当然你也可以自己实现！
			{FUNC,"LN",0.0, log},
			{FUNC,"EXP",0.0,exp},
			{FUNC,"SQRT",0.0,sqrt},

			{FUNC,"_AYY_",0.0,demo_ayy},//WxQ自己定义的函数

			{ORIGIN,"ORIGIN",0.0,NULL},//保留关键字
			{SCALE,"SCALE",0.0,NULL},
			{ROT,"ROT",0.0,NULL},
			{IS,"IS",0.0,NULL},
			{FOR,"FOR",0.0,NULL},
			{FROM,"FROM",0.0,NULL},
			{TO,"TO",0.0,NULL},
			{STEP,"STEP",0.0,NULL},
			{DRAW,"DRAW",0.0,NULL},
			{COLOR,"COLOR",0.0,NULL}//新增的颜色设置关键字

			//你可以再定义其他关键字、常量名、函数名等.....
		};

		typedef map<std::string, basic_token*> t_SymTable;
		t_SymTable _innerSymTable;
		//符号表定义--结束
	}//namespace 

	void Lexer::InitSymbolTable()
	{
		_innerSymTable.clear();
		for (size_t count = 0;
			count < sizeof(TokenTab) / sizeof(TokenTab[0]);
			++count)
		{
			_innerSymTable[TokenTab[count].lexeme] = &(TokenTab[count]);
		}
	};

	//判断所给的字符串是否在符号表中
	basic_token* Lexer::JudgekeyToken(const std::string& str)
	{
		int count;
		string upperText = Token::str2upper(str);

		t_SymTable::iterator where = _innerSymTable.find(upperText);
		if (where == _innerSymTable.end())
			return NULL;
		else
			return where->second;

		/* C++ STL的map之查找效率远高于普通数组
		for(count=0;count< sizeof(TokenTab)/sizeof(TokenTab[e]);++count)
		{
			if(TokenTab[count].lexeme==upperText)
				return&(TokenTab[count]);
		}
		return NULL;
		*/
	};
		
	//
	//下面的代码仅为显示记号种类的名称文本
	//
	struct tk_print
	{
		enum Token_Type tk;
		const char* str;
	};
	#define MKSTR(x)\
	{				\
		x,#x		\
	}
	struct tk_print tk_names[] =
	{
	MKSTR(ID),
	MKSTR(ORIGIN),
	MKSTR(SCALE),MKSTR(ROT),
	MKSTR(IS),MKSTR(TO),MKSTR(STEP),
	MKSTR(DRAW),MKSTR(FOR),MKSTR(FROM),
	MKSTR(T),MKSTR(COLOR),MKSTR(SIZE),

	MKSTR(SEMICO),MKSTR(L_BRACKET),
	MKSTR(R_BRACKET),MKSTR(COMMA),

	MKSTR(PLUS),MKSTR(MINUS),MKSTR(MUL),
	MKSTR(DIV),MKSTR(POWER),
	MKSTR(FUNC),
	MKSTR(CONST_ID),
	MKSTR(ERRTOKEN),

	MKSTR(NONTOKEN)//flag item for end
	};
	#undef MKSTR

	const char* token_type_str(enum Token_Type tk)
	{
		struct tk_print* p = tk_names;
		for (; p->tk != NONTOKEN; ++p)
		{
			if (p->tk == tk)
				return p->str;
		}
		return "UNKNOW";
	}
}// namespace module_Lexer