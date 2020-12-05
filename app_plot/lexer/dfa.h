#pragma once
//�ļ�:dfa.hpp
//����:����DFA�ĳ����༰��ӿ�
//����:WXQ#2019,2018

#ifndef _LEXER_DFA_HPP
#define _LEXER_DFA_HPP

#include "../include/Lexer.h"//use Token_type 

namespace module_Lexer 
{
	namespace DFA//�ʷ��������ڲ�����ģ��
	{
	//����DFA�Ĳ����ӿ�(abstract class).
	//�ýӿ���DFA��ʵ���޹�,��������DFA�Ǳ������ġ�����ֱ�ӱ�����޹أ�
	//���㶨�����DFAʱ,Ҫ�����е�״̬�÷Ǹ��������룡

		class dfa 
		{
		public:
			//��ȡDFA��̬(>=0)
			virtual  int get_start_state()const = 0;
			//��ѯת�Ʋ�������һ״̬��
			//������״̬state_src��,�����ַ�ch����һ״̬.
			//�����ص�״̬С��8,��ʾ��ת��.
			virtual int move(int state_src, char ch)const = 0;
			//�ж�ָ��״̬state�Ƿ�ΪDFA����̬��
			//�����򷵻ظ���̬��Ӧ�ļǺ����,���򷵻� ERRTOKEN.
			virtual Token_Type state_is_final(int state)const = 0;

			virtual ~dfa(){};
		public:
			//���ȱʡ�ġ��������͡���DFA����.
			//������DFA�������������delete����
			static dfa* get_table_driven_dfa();

			//��á�ֱ�ӱ����͡���DFA����.
			//������DFA�������������delete����
			static dfa* get_hardCode_dfa();
		};
	}//end of namespace DFA
}//namespace module_Lexer
#endif//#ifndef LEXER_DFA_HPP