//�ļ�:lexer.cpp
//����:�ʷ���������Lexer��ʵ��(C++���԰�)
//����:WXQ#2019,2018

#include<cstdio>//fopen(),fclose(),getc(),ungetc()
#include<cstring>//memset()
#include <cctype>//isspace(),toupper()
#include <cstdlib>//atof()of GCC
#include<cmath>//atof(),sin(),cos(),...

#include<map>//���÷��ű�Ĵ洢�ṹmap

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
		//���ڿɼ��ַ�,��д�����ӣ�������
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

	//���ַ�c׷�ӵ��Ǻ��ı�ĩβ.
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
		pDFA =//�����Ǳ�������DFA 
			DFA::dfa::get_table_driven_dfa();
		//������ֱ�ӱ�����DFA
		//DFA::dfa::get_hardCode_dfa();
	};

	Lexer::~Lexer()
	{
		CloseScanner();
	};

	//�ӿڲ���1:��ʼ���ʷ�������
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

	//�ӿڲ���2:�رմʷ�������
	void Lexer::CloseScanner()
	{
		if (in_flie != NULL)
			fclose(in_flie);
		in_flie = NULL;
	};

	//�ӿڲ���3:ʶ�𲢷���һ���Ǻš�
	//�����Ƿ�����ʱ.type=ERRTOKEN���ļ�����ʱ.type=NONTOKEN 
	Token Lexer::GetToken()
	{
		int first_char;//�Ǻſ�ʼ�ĵ�1���ַ�
		int last_state = -1;//ʶ��ǺŽ���ʱ��״̬
		position where;//��ǰ�Ǻŵ���ʼλ��
		Token theToken;//��ʶ�𵽵ļǺŶ���
		int to_bo_continue;

		do
		{
			theToken.clear();

			//��1��:Ԥ����,�����հ��ַ�
			first_char = pre_process(&theToken);
			if (first_char == -1)//�ļ�������
			{
				theToken.setPosition(current_pos);//������λ��
				theToken.setType(NONTOKEN);
				return theToken;
			}
			where = current_pos;
			theToken.setPosition(where);//��ǰ�Ǻſ�ʼ��λ��

			//��2��:��ɨ������,��ת��״̬
			last_state = scan_move(&theToken, first_char);

			//��3��:����:������̬����ǵļǺ�������Ϣ,�������⴦��
			to_bo_continue = post_process(&theToken, last_state);
		} while (to_bo_continue != 0);

		//theToken.setPosition(where);//�����Ǻŵ�λ��
		return theToken;
	};

	//
	//�����Ǹ������������������ȶ���
	//

	//������Դ�����ж���һ���ַ���������
	//�������ļ������򷵻�-1.
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
				//////return toupper(next_char);//ͳһ���ش�д
				return next_char;
			}
		}
	};

	//��Ԥ�����ַ��˻ص�����Դ������
	void Lexer::BackChar(char next_char)
	{
		//�ļ�������־�����в�����
		if (next_char == EOF || next_char == '\n')
			return;

		ungetc(next_char, in_flie);
		--(current_pos);
	};

	int Lexer::is_space(char c)
	{
		if (c < 0 || c>0x70)//�����ASCII�ַ�,��(���)�����ַ�
			return 0;
		return isspace(c);
	};

	//ʶ��һ���Ǻŵ�Ԥ����:�����հ��ַ�,����ȡ��1���ǿհ��ַ���
	//����ֵ:-1�ļ�����,����ֵ��ʾ�ַ�����.
	int Lexer::pre_process(Token* pToken)
	{
		int current_char;//��ǰ�������ַ�
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
		//��ʱ,current_char���ǼǺŵĵ�1���ַ�
		return current_char;
	};

	//ʶ��Ǻŵĺ��Ĳ���:��ɨ��߽���״̬ת��
	int Lexer::scan_move(Token* pToken, int first_char)
	{
		int current_state, next_state;//��ǰ״̬,��һ״̬
		int current_char;//��ǰ�ַ�

		current_char = first_char;
		current_state = pDFA->get_start_state();
		for (;;)
		{
			next_state = pDFA->move(current_state, current_char);
			if (next_state < 0)//û��ת����
			{
				//��һ���ַ�����Ч,������.������Ϊ�����������ַ���������ѭ��
				if (pToken->is_empty())
				{
					pToken->setChar(current_char);
				}
				else
				{//�˻ص�ǰ�ַ�,��Ӧ���ǿհ��ַ�,��������һ�ǺŵĿ�ʼ.
					BackChar(current_char);
				}
				break;
			}

			pToken->appendText(current_char);//׷�ӼǺŵ��ı�

			current_state = next_state;
			current_char = GetChar();
			if (current_char == -1)//�ļ�������
				break;
		}
		return current_state;
	};

	//������̬����ǵļǺ�������Ϣ,�������⴦��.
	//�����ط�0,���ʾ��ǰ�մ������ˡ�ע�͡�,��Ҫ�����߽��Ż�ȡ��һ���Ǻš�
	int Lexer::post_process(Token* pToken, int last_state)
	{
		int to_bo_continue = 0;//FALSE 
		enum Token_Type tk = pDFA->state_is_final(last_state);
		pToken->setType(tk);
		switch (tk)
		{
		case ID://����ű�,��һ������Ǻ���Ϣ
			{
				basic_token* id = JudgekeyToken(pToken->getText());
				if (NULL == id)//�Ҳ���ʶ��,�����﷨������ȥ�����
					pToken->setType(ID);//ERRTOKEN);
				else
				{
					pToken->setBasic(*id);
				}
			}
			break;
		case CONST_ID://תΪ��ֵ
			pToken->setValue(atof(pToken->getText().c_str()));
			break;
		case COMMENT://��ע��:����ֱ����β���ı�,
		//��ͨ������ֵ��֪�����߶�ȡ��һ���Ǻ�
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

	//����ĳ�����Ϊ�������ֿռ�ĳ�Ա,�����ƽ���
	//����ǰ�ļ����á�
	namespace
	{
		//�ú�������ʾ��:
		//�������п��Զ��û��ṩ�����API����,
		//ֻ������Ҫһ��double�Ͳ���,����double���ɡ�
		//�ͺñ�������C���ʱ���Ե���cos(x)������
		double demo_ayy(double)
		{
			return 2019.07 - 2018.10;
		};

		/*���ű���(�����ݹ̶�)*/
		struct basic_token TokenTab[] =
		{
			/*ÿ�����ݵĵ�2����(�����ı�)����ȫ����д*/
			{CONST_ID,"PI",3.1415926,NULL},//��������
			{CONST_ID,"E",2.71828,NULL},

			{CONST_ID,"XD",10701,NULL},//�����ĳ�����
			{CONST_ID,"WXQ",5.28,NULL},//�����ĳ�����

			{T,"T",0.0,NULL},//����

			{FUNC,"SIN",0.0,sin},//֧�ֵĺ�����
			{FUNC,"COS",0.0,cos},//��Щ��������math.h��
			{FUNC,"TAN",0.0,tan},//��Ȼ��Ҳ�����Լ�ʵ�֣�
			{FUNC,"LN",0.0, log},
			{FUNC,"EXP",0.0,exp},
			{FUNC,"SQRT",0.0,sqrt},

			{FUNC,"_AYY_",0.0,demo_ayy},//WxQ�Լ�����ĺ���

			{ORIGIN,"ORIGIN",0.0,NULL},//�����ؼ���
			{SCALE,"SCALE",0.0,NULL},
			{ROT,"ROT",0.0,NULL},
			{IS,"IS",0.0,NULL},
			{FOR,"FOR",0.0,NULL},
			{FROM,"FROM",0.0,NULL},
			{TO,"TO",0.0,NULL},
			{STEP,"STEP",0.0,NULL},
			{DRAW,"DRAW",0.0,NULL},
			{COLOR,"COLOR",0.0,NULL}//��������ɫ���ùؼ���

			//������ٶ��������ؼ��֡�����������������.....
		};

		typedef map<std::string, basic_token*> t_SymTable;
		t_SymTable _innerSymTable;
		//���ű���--����
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

	//�ж��������ַ����Ƿ��ڷ��ű���
	basic_token* Lexer::JudgekeyToken(const std::string& str)
	{
		int count;
		string upperText = Token::str2upper(str);

		t_SymTable::iterator where = _innerSymTable.find(upperText);
		if (where == _innerSymTable.end())
			return NULL;
		else
			return where->second;

		/* C++ STL��map֮����Ч��Զ������ͨ����
		for(count=0;count< sizeof(TokenTab)/sizeof(TokenTab[e]);++count)
		{
			if(TokenTab[count].lexeme==upperText)
				return&(TokenTab[count]);
		}
		return NULL;
		*/
	};
		
	//
	//����Ĵ����Ϊ��ʾ�Ǻ�����������ı�
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