//文件:dfa_hardCode.cpp
//内容:定义并实现直接编码型DFA
//作者:WXQ#2019,2018

#include"dfa.h"
namespace module_Lexer
{
	namespace DFA
	{
		//直接编码型DFA的类型.
		//我的实现策略限制了其状态编码的范围为0～255.
		class dfa_hard_code :public dfa {
		public:
			dfa_hard_code() {}
		public:
			/*Override*/virtual int get_start_state() const { return 0; };
			/*Override*/virtual int move(int state_src, char ch)const;
			/*Override*/virtual Token_Type state_is_final(int state)const;
		};

		/*static member function*/
		dfa* dfa::get_hardCode_dfa()
		{
			//该DFA对象不允许调用者delete！！
			static dfa_hard_code dfa;
			return &dfa;
		}

		//判断指定状态是否为DFA的终态。
		//若是,则返回该终态对应的记号类别,否则返回ERRTOKEN.
		Token_Type dfa_hard_code::state_is_final(int state)const
		{
			switch (state)
			{
			case 1:
				return ID;
			case 2:
			case 3:
				return CONST_ID;
			case 4:
				return MUL;
			case 5:
				return POWER;
			case 6:
				return DIV;
			case 7:
				return MINUS;
			case 8:
				return PLUS;
			case 9:
				return COMMA;
			case 10:
				return SEMICO;
			case 11:
				return L_BRACKET;
			case 12:
				return R_BRACKET;
			case 13:
				return COMMENT;
			default:
				return ERRTOKEN;
			}
		}

		//函数dfa::move()
		//功能:
		//根据当前状态、当前遇到的字符,进行状态转移。//该函数相当于NFA定义中的move()函数.
		//返回值定义:
		//-1:此时没有状态转移(假设DFA所有状态编号均)=e)
		//>=0:新的状态
		//注意:
		//若遇到当前DFA不接受的字符时,则没有状态转移.
		int dfa_hard_code::move(int state_src, char ch)const
		{
			switch (state_src)
			{
			case 0:
				if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || '_' == ch)
					return 1;
				else if ('0' <= ch && ch <= '9')
					return 2;
				else if (ch == '*')
					return 4;
				else if (ch == '/')
					return 6;
				else if (ch == '-')
					return 7;
				else if (ch == '+')
					return 8;
				else if (ch == ',')
					return 9;
				else if (ch == ';')
					return 10;
				else if (ch == '(')
					return 11;
				else if (ch == ')')
					return 12;
				break;

			case 1:
				if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || '_' == ch || ('0' <= ch && ch <= '9'))
					return 1;
				break;
			case 2:
				if (ch == '.')
					return 3;
				if ('0' <= ch && ch <= '9')
					return 2;
				break;
			case 3:
				if ('0' <= ch && ch <= '9')
					return 3;
				break;
			case 4:
				if (ch == '*')
					return 5;
				break;
			case 6:
				if (ch == '/')
					return 13;
				break;
			case 7:
				if (ch == '-')
					return 13;
				break;
			}
			return -1;
		}
	}// end of namespace DFA

}//namespace module_Lexer