#pragma once
//�ļ�:parser.hpp
//����:�����﷨�������Ķ���ӿ�(C++��������)
//����:WXQ#2019,2818

#ifndef _PARSER_HPP
#define _PARSER_HPP

#include"../include/Lexer.h"//�ʷ��������ӿ�����
#include"../include/art.h"//�﷨����������

namespace module_Parser
{

	//�﷨�������Ĳ������Ҫ��:
	//1.�÷�����ҪΪȫ����������һ���������﷨����
	//���﷨��������������ʱ,���������Զ�����,����Ҫ����������١�//2.��������ɶԸ��﷨�����ж��ִ��������������ģ��������
	//���Ǽ�����ʽ��ֵ����ͼ,Ӧ�û��������顣
	//3.�﷨�������ĺ��Ļ��ڡ��ݹ��½���������������ġ����ķ��е�ÿ�����ս��,
	//������Ϊ����Parser��һ��private����protected ��Ա������

	//�﷨������������
	class Parser
	{
	public://������ģʹ�õĽӿڲ���
		Parser(module_Lexer::Lexer* needscanner);//ָ���Ѿ�׼���õĴʷ�������
		virtual ~Parser();

		//�﷨��������ڲ�����
		//����Ϊ�������빹����﷨��(����б�).
		//��������������һ������򷵻طǿյ���.
		virtual AST_stmt_list* run();

		//���ر��������ļ���
		const char* getFilename()const;
		//����Ϊ�������빹����﷨��(����б�)
		AST_stmt_list* getstmts()const { return ASTroot; }

		//�����AST��,����T�Ĵ洢�ռ䡣
		double* getTmemory()const { return ptr_t_storage; }
		//���ñ���T�Ĵ洢�ռ䡣
		//���÷����������﷨����ǰ���á�
		void setTmemory(double* t) { ptr_t_storage = t; }

	protected://��������:���ս����Ӧ���ӳ��������������á�
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
		AST_stmt_list* ASTroot;//��������ĳ����﷨��
		double* ptr_t_storage;//Tֵ�Ĵ�ſռ�
		double parameter;//default storage of T

		module_Lexer::Token curr_token;//��ǰ�Ǻ�
		module_Lexer::Token lastToken;//��ǰ�Ǻ�֮ǰ,���һ����ȷ�ļǺ�

	private://�﷨�����ĸ����ɷ�
		module_Lexer::Lexer* theScanner;
		int indent;//��ʾ��Ϣ��������,���� PrintsyntaxTree()

		module_Lexer::Token_Type FetchToken();//��ȡ��һ�Ǻ�
		void MatchToken(enum module_Lexer::Token_Type expected);//ƥ��Ǻ�(�ս��)
		void SyntaxError(int case_of);//�����﷨����
		void PrintSyntaxTree(AST* root, int arg_indent);//��ӡ�﷨��
		AST_expression* MakeExprNode(module_Lexer::Token* p_rt, ...);//Ϊ���ʽ�����﷨��

		//���к������ڸ��ٵ���,��ʾ���롢�˳��ĸ����ս���ĺ�����
		void enter(const char* nonTerminalName);
		void exit(const char* nonTerminalName);
		void ast_dump(AST* root);
	};
}//namespace module_Parser

#endif//#ifndef_PARSER_HPP