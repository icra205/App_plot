#pragma once
//�ļ�:lexer.hpp
//����:�����ʷ��������Ķ���ӿ�(C++��������)
//����:WXQ#2019,2018
/*���������еĴ��ڴ���֮���
*1.�ʷ������׶�ֻ��ʶ�𵽡���Ч�ַ���������Ч���ʡ������ִ���
*2.�������ϴ���,�ʷ�������������߷��صļǺ�֮����ͳһ��дΪ"ERRTOKEN".
*3.���ڡ���Ч�ַ���,���ᱻ�ʷ������ڲ�ֱ�Ӷ���,�������ַ�ת��Ϊһ��16����
*������ŵ��Ǻŵ��ı���,�Ա�����ߵõ���
*4.���д���,��Ҫ������(�﷨������,��ʷ����Գ���)������
*/
#ifndef _LEXER_HPP
#define _LEXER_HPP
#include <cstdio>//for FILE
#include<string>//for std::string 

namespace module_Lexer
{
	//
	//�ʷ��������ṩ�����Ͷ����볣������
	//

	enum Token_Type
	{
		COMMENT,//���ڴ�����ע��
		ID,//���ڽ�һ�����ָ������֡���������������,����ʶ�ı�ʶ��

		ORIGIN,//�ؼ���origin 
		SCALE,//�ؼ���scale 
		ROT,//�ؼ���rot 
		IS,//�ؼ���is
		TO,//�ؼ���to 
		STEP,//�ؼ���step 
		DRAW,//�ؼ��� draw 
		FOR,//�ؼ���for 
		FROM,//�ؼ���from 
		T,//����,�������õ�Ψһ�ı���

		COLOR,//͵͵���ӵĹؼ���,������ɫ�������
		SIZE,//͵͵���ӵĹؼ���,�������õ��С���

		SEMICO,//�������:�ֺ�
		L_BRACKET,//�������:��Բ����
		R_BRACKET,//�������:��Բ����
		COMMA,//�������:�ָ�����
		PLUS,//�����:�Ӻ�
		MINUS,//�����:����
		MUL,//�����:�˺�
		DIV,//�����:����
		POWER,//�����:�˷�
		FUNC,//������(��ʶ��)
		CONST_ID,//����(������or��ֵ������)
		NONTOKEN,//�ռǺ�,���ڱ�ʾ���������
		ERRTOKEN//����Ǻ�
	};

	const char* token_type_str(enum Token_Type tk);

	typedef unsigned int uint;
	class position//�Ǻ�����λ�õ���������
	{
	private:
		uint _line;//�к�
		uint _col;//�к�
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
			if (--_col == 0xFFffFFff)//��ֹ�������߼�����
				_col = 0;
			return *this;
		}
		uint line()const { return _line; }
		uint column()const { return _col; }
	};

	//֧�ֵı����ú���֮ԭ��/����
	typedef double(*t_func)(double);

	//�ǺŵĻ������ݽṹ
	struct basic_token {
		Token_Type type = ERRTOKEN; //�Ǻŵ����
		std::string lexeme;//���ɼǺŵ��ַ���
		double value = 0; //��Ϊ����,���ǳ�����ֵ
		t_func FuncPtr = NULL;//��Ϊ����,���Ǻ�����ָ��
		void clear()
		{
			type = ERRTOKEN;;
			value = 0;
			FuncPtr = NULL;
			lexeme.clear();
		}
	};

	//�ǺŶ��������
	class Token {
	public:
		Token();
		Token(const position& w);
		Token(const basic_token& base);

		//all Setters 
		void clear(bool resetPosition = false);//���ǺŶ�������Ϊ��ʼ״̬
		void setPosition(const position& w);//
		void setBasic(const basic_token& base);//��д�ǺŵĻ�������
		void setType(Token_Type t);//�޸ļǺ����
		void setText(const char* str);//�޸ļǺŵ��ı�
		void setText(const std::string& str);//�޸ļǺŵ��ı�
			//���ַ�c׷�ӵ��Ǻ��ı���
		void appendText(char c);
		//���ַ���str׷�ӵ��Ǻ��ı�֮ĩβ��
		void appendText(const char* str);

		//����Ч�ַ�c��дΪ�Ǻ��ı�
		void setChar(int c);
		//����CONST_ID,��д����ֵ
		void setValue(double v);

		//all Getters 

		Token_Type getType()const { return basic.type; }
		//�Ƿ�Ϊ�ռǺ�
		bool is_empty()const;
		const std::string& getText()const { return basic.lexeme; }
		std::string getUpperText()const;
		double getValue()const { return basic.value; }
		t_func getFuncPtr()const { return basic.FuncPtr; }
		position getPosition()const { return where; }

		static std::string str2upper(const std::string& txt);
	private:
		//�ǺŵĻ�����Ϣ
		basic_token basic;
		//�Ǻ��������е�λ��
		position where;
	};

	//
	//Part II �����ӿ�
	//

	namespace DFA//sub-module of module_Lexer 
	{
		class dfa;//class body is hidden from user
	}

	//�ʷ�������������
	class Lexer {
	public:
		Lexer();
		~Lexer();//�رմʷ�������,cal1CloseScanner()

			//��ʼ���ʷ���������Դ,�ɹ�ʱ���ط�0,ʧ�ܷ���0
		int InitScanner(const char* fileName);
		const char* getFilename()const
		{
			return file_name.c_str();
		}
		//ʶ�𲢷���һ���Ǻš�
		//�����Ƿ�����ʱ.type=ERRTOKEN���ļ�����ʱ.type=NONTOKEN 
		Token GetToken();
		//ʶ�𲢷���һ���Ǻ�(GetToken()�ı���)��
		//�����Ƿ�����ʱ.type=ERRTOKEN���ļ�����ʱ.type=NONTOKEN 
		Token nextToken() { return GetToken(); }

	private:
		void CloseScanner();//�رմʷ�������
		int pre_process(Token* pToken);//ʶ��Ǻ�ǰ��Ԥ����
		int scan_move(Token* pToken, int first_char);//��ɨ���ת��
		int post_process(Token* pToken, int last_state);//ʶ��Ǻŵĺ���

		char GetChar();//������Դ�����ж���һ���ַ�
		void BackChar(char c);//��Ԥ�����ַ��˻ص�����Դ������
		int is_space(char c);//�ַ�c�Ƿ�Ϊ�հ��ַ�

		//�ж��������ַ����Ƿ��ڷ��ű���
		basic_token* JudgekeyToken(const std::string& str);
		void InitSymbolTable();//��ʼ�����ű�

	protected:
		std::FILE* in_flie;//�����ļ���,ʹ��C���Ե��ļ�����API 
		DFA::dfa* pDFA;//��������,ָ��DFA����
		position current_pos;//��ǰ�ļ�λ��(�к�,�к�)
		std::string file_name;//���������ļ���
	};
}//namespace module_Lexer

#endif//#ifndef_LEXER_HPP
//end of file