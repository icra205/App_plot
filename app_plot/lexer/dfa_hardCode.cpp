//�ļ�:dfa_hardCode.cpp
//����:���岢ʵ��ֱ�ӱ�����DFA
//����:WXQ#2019,2018

#include"dfa.h"
namespace module_Lexer
{
	namespace DFA
	{
		//ֱ�ӱ�����DFA������.
		//�ҵ�ʵ�ֲ�����������״̬����ķ�ΧΪ0��255.
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
			//��DFA�������������delete����
			static dfa_hard_code dfa;
			return &dfa;
		}

		//�ж�ָ��״̬�Ƿ�ΪDFA����̬��
		//����,�򷵻ظ���̬��Ӧ�ļǺ����,���򷵻�ERRTOKEN.
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

		//����dfa::move()
		//����:
		//���ݵ�ǰ״̬����ǰ�������ַ�,����״̬ת�ơ�//�ú����൱��NFA�����е�move()����.
		//����ֵ����:
		//-1:��ʱû��״̬ת��(����DFA����״̬��ž�)=e)
		//>=0:�µ�״̬
		//ע��:
		//��������ǰDFA�����ܵ��ַ�ʱ,��û��״̬ת��.
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