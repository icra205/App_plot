#pragma once
//�ļ�:ast.hpp
//����:�����﷨��������(C++��������)
//����:WXQ#2019,2018

#ifndef _PARSER_AST_HPP
#define _PARSER_AST_HPP

#include<string>

#include"../include/Lexer.h"

/*���ڳ����﷨��(AST)����̳в��:
1.����AST�����﷨������������,�����﷨����������֪�����������Ρ�
���ڴ�,�Ұ�AST���ֵ��﷨����ģ�顣
2.Ϊ��߳���Ŀ�ά���ԡ���������,���İ��ձ��������ԡ�������ͼ����
�Ľṹ,������֧�ֵĳ����﷨��(���)���������໮��:ÿһ�ֽṹ��
���Լ�����(���)����,�Ӷ�����һ����̳в�Ρ���������AST�����г�
���﷨��(���)���͵Ķ�����ࡣ
*/

namespace module_Parser
{

	//AST�Ǻ�����ͼ�������нṹ��Ƶĳ����﷨��(���)������
	//����3��ֱ������:
	//AST_expression:���б��ʽ��Ӧ���Ĺ�ͬ����
	//AST statement:�������֮�������Ĺ�ͬ����
	//AST_stmt_list:Ϊȫ������(һϵ�����)������﷨��

	//the top abstract class for all AST 
	class AST
	{
	public:
		//���㵱ǰ���Ϊ����������Ӧ���ʽ��ֵ
		virtual double value()const = 0;

	public:
		virtual ~AST();
		//���ص�ǰ���ĵ�idx+1�����ӽ��
		virtual AST* getchild(unsigned int idx)const;
		//��ȡ��ǰ���ĺ��ӽ��(����)������
		size_t children_count()const;

		//���ص�ǰ���ĵ�idx+1�����ӵ�ֵ
		virtual double childValue(unsigned int idx)const;

		virtual void print(int ident)const;//��������Դ�ӡ(��)��
		const module_Lexer::Token& token()const//��ý���еļǺ�
		{
			return mToken;
		}

		module_Lexer::Token_Type kind()const//��ý���ж�Ӧ�ļǺ�����
		{
			return mToken.getType();
		}

	protected://only for derived classes 
		AST(module_Lexer::Token& t);

	protected:
		module_Lexer::Token mToken;//����д�ŵļǺŶ���
		void* children;//��ǰ��������(������)
	};

	//
	//�����Ǹ��ֱ��ʽ��AST֮����
	//

	//root class of all expression,abstract 
	class AST_expression :public AST
	{
	protected://only for derived classes 
		AST_expression(module_Lexer::Token& t):AST(t){}
	};

	//root class of all binary operator 
	class AST_binary :public AST_expression//abstract class 
	{
	protected://only for derived classes 
		AST_binary(module_Lexer::Token& t,AST* L,AST* R);

			protected:
			double leftValue()const//value of left operand
		{
			return childValue(0);
		}

			double rightValue()const//value of right operand 
		{
			return childValue(1);
		}
	};

	//��Ԫ+(ע��:�﷨����ʱ��һԪ�Ӷ�����)
	class AST_plus :public AST_binary//concrete class 
	{
		public:
			AST_plus(module_Lexer::Token& t,AST* L,AST* R)
			:AST_binary(t,L,R){}
			virtual double value()const
		{
			return leftValue() + rightValue();
		}
	};

	//��Ԫ-(ע��:�﷨����ʱ��һԪ�Ӷ�����)
	class AST_minus :public AST_binary//concrete class 
	{
		public:
			AST_minus(module_Lexer::Token& t,AST* L,AST* R)
			:AST_binary(t,L,R){}
			virtual double value()const
		{
			return leftValue() - rightValue();
		}
	};

	//��Ԫ *
	class AST_multiple :public AST_binary//concrete class 
	{
		public:
			AST_multiple(module_Lexer::Token& t,AST* L,AST* R)
			:AST_binary(t,L,R){}
			virtual double value()const
		{
			return leftValue() * rightValue();
		}
	};

	//��Ԫ /
	class AST_div :public AST_binary//concrete class 
	{
		public:
			AST_div(module_Lexer::Token& t,AST* L,AST* R)
			:AST_binary(t,L,R){}
			virtual double value()const
		{
			return leftValue() / rightValue();
		}
	};

	//��Ԫ�˷�
	class AST_power :public AST_binary//concrete class 
	{
	public:AST_power(module_Lexer::Token& t,AST* L,AST* R)
		:AST_binary(t,L,R){}
		virtual double value()const;
	};

		//root class of all unary operator 
		class AST_unary :public AST_expression//abstract class 
	{
	protected://only for derived classes 
		AST_unary(module_Lexer::Token& t,AST* C);

			protected:
			double operand()const
		{
		return childValue(0);
		}
	};

	//�������ý��:����һ������,��һԪ���㡣
	//�ʷ�����ʱ,ʶ�𵽺�����,�����������������õĺ��������д��
	//�ǺŶ������ˡ�
	class AST_func :public AST_unary//concrete class 
	{
		public:
			AST_func(module_Lexer::Token& t,AST* child)
			:AST_unary(t,child){}
			virtual double value()const
		{
			//call the function 
			return( * mToken.getFuncPtr())(operand());
		}
	};

	//Ҷ�����������
	class AST_leaf :public AST_expression//abstract class 
	{
	public:
		AST_leaf(module_Lexer::Token& t) :AST_expression(t) {}
	};

	//���ʽ�г��ֵĲ���T��Ӧ�Ľ������
	class AST_T :public AST_leaf//concrete class 
	{
	public:AST_T(module_Lexer::Token& t, double* p)
		:AST_leaf(t), ptr(p) {}
		  virtual double value()const
		  {
			  return(ptr == NULL) ? 0 : *ptr;
		  }
		  void setstorage(double* p)
		  {
			  ptr = p;
		  }

	private:
		double* ptr;
	};

	//������������͡�
	//�ʷ�����ʱ�Ѿ�������ֵ��¼�ڼǺŶ����С�
	class AST_const :public AST_leaf//concrete class 
	{
	public:
		AST_const(module_Lexer::Token& t) :AST_leaf(t) {}
		virtual double value()const
		{
			return mToken.getValue();
		}
	};

	//��ʶ����Ϊ���ʽ��Ҷ��,Ŀǰ����֧����������ɫ��
	//����ֵ�������������ṩ
	class AST_ID :public AST_leaf//concrete class 
	{
	public:
		AST_ID(module_Lexer::Token& t) :AST_leaf(t) {}
	};

	//
	//�����Ǹ�������AST֮����
	//

	//�����﷨�����֮�����ࡣ
	//������ĺ��ӽ�㶼��Ӧ���ʽAST_expression������.
	class AST_statement :public AST
	{
	public:
		virtual double value()const { return 0.0; }
		void addExpression(AST_expression* expTree);//׷��һ�����ʽ
	protected://only for derived-classes 
		AST_statement(module_Lexer::Token& t) :AST(t) {}
	};

	class AST_stmt_origin :public AST_statement
	{
	public:
		AST_stmt_origin(module_Lexer::Token& t) :AST_statement(t) {}
	};

	class AST_stmt_rot :public AST_statement
	{
	public:
		AST_stmt_rot(module_Lexer::Token& t) :AST_statement(t) {}
	};

	class AST_stmt_scale : public AST_statement
	{
	public:
	AST_stmt_scale(module_Lexer::Token & t) :AST_statement(t) {}
	};

	class AST_stmt_forDraw :public AST_statement
	{
	public:
		AST_stmt_forDraw(module_Lexer::Token& t) :AST_statement(t) {}
	};
	
	class AST_ColorName :public AST_ID
	{
	public:
		AST_ColorName(module_Lexer::Token& t) :AST_ID(t) {}
		virtual double value()const { return 0.0; }
		void getRGB(double& r, double& g, double& b)const;
	};
	
	class AST_stmt_color : public AST_statement
	{
	public:
		AST_stmt_color(module_Lexer::Token &t) :AST_statement(t) {}
		//����������ɫ��������ʾ��ɫ��ֵ.
		void ChgColor(module_Lexer::Token &t);
	public:
		static void getDefaultcolor(double &r, double& g, double& b);
	};
	
	class AST_stmt_pixelsize : public AST_statement
	{
	public:
		AST_stmt_pixelsize(module_Lexer::Token& t) : AST_statement(t) {}
	};
//�������������������͡�
//��ÿ�����Ӷ�Ӧһ�����(AST_statement����)

	class AST_stmt_list :public AST
	{
	public:
		//������ű��������ļ���
		AST_stmt_list(module_Lexer::Token& t) :AST(t) {}
		virtual double value()const { return 0.0; }
		//���ص�idx+1�����
		virtual AST_statement* getchild(unsigned int idx) const;
		void addstmt(AST_statement* stmt, int where = 1);//����һ�����
	};
}//namespace module_Parser
#endif//#ifndef_PARSER_AST_HPP