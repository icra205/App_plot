#pragma once
//文件:ast.hpp
//内容:声明语法树的类型(C++面向对象版)
//作者:WXQ#2019,2018

#ifndef _PARSER_AST_HPP
#define _PARSER_AST_HPP

#include<string>

#include"../include/Lexer.h"

/*关于抽象语法树(AST)的类继承层次:
1.由于AST是由语法分析器建立的,所以语法分析器必须知道完整的类层次。
鉴于此,我把AST划分到语法分析模块。
2.为提高程序的可维护性、可重用性,本文按照被分析语言—函数绘图语言
的结构,对所有支持的抽象语法树(结点)进行了种类划分:每一种结构都
有自己的树(结点)类型,从而构成一个类继承层次。其中类型AST是所有抽
象语法树(结点)类型的顶层基类。
*/

namespace module_Parser
{

	//AST是函数绘图语言所有结构设计的抽象语法树(结点)的类型
	//它有3个直接子类:
	//AST_expression:所有表达式对应结点的共同基类
	//AST statement:所有语句之树根结点的共同基类
	//AST_stmt_list:为全部输入(一系列语句)构造的语法树

	//the top abstract class for all AST 
	class AST
	{
	public:
		//计算当前结点为根的子树对应表达式的值
		virtual double value()const = 0;

	public:
		virtual ~AST();
		//返回当前结点的第idx+1个孩子结点
		virtual AST* getchild(unsigned int idx)const;
		//获取当前结点的孩子结点(子树)的数量
		size_t children_count()const;

		//返回当前结点的第idx+1个孩子的值
		virtual double childValue(unsigned int idx)const;

		virtual void print(int ident)const;//按先序策略打印(子)树
		const module_Lexer::Token& token()const//获得结点中的记号
		{
			return mToken;
		}

		module_Lexer::Token_Type kind()const//获得结点中对应的记号种类
		{
			return mToken.getType();
		}

	protected://only for derived classes 
		AST(module_Lexer::Token& t);

	protected:
		module_Lexer::Token mToken;//结点中存放的记号对象
		void* children;//当前结点的子树(儿子们)
	};

	//
	//下面是各种表达式的AST之类层次
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

	//二元+(注意:语法分析时将一元加丢弃了)
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

	//二元-(注意:语法分析时将一元加丢弃了)
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

	//二元 *
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

	//二元 /
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

	//二元乘方
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

	//函数调用结点:仅有一个参数,即一元运算。
	//词法分析时,识别到函数后,即将“解释器”内置的函数入口填写到
	//记号对象中了。
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

	//叶子类结点的类型
	class AST_leaf :public AST_expression//abstract class 
	{
	public:
		AST_leaf(module_Lexer::Token& t) :AST_expression(t) {}
	};

	//表达式中出现的参数T对应的结点类型
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

	//常量类结点的类型。
	//词法分析时已经将常数值记录在记号对象中。
	class AST_const :public AST_leaf//concrete class 
	{
	public:
		AST_const(module_Lexer::Token& t) :AST_leaf(t) {}
		virtual double value()const
		{
			return mToken.getValue();
		}
	};

	//标识符作为表达式的叶子,目前用于支持命名的颜色。
	//其求值操作由派生类提供
	class AST_ID :public AST_leaf//concrete class 
	{
	public:
		AST_ID(module_Lexer::Token& t) :AST_leaf(t) {}
	};

	//
	//下面是各种语句的AST之类层次
	//

	//语句的语法树结点之祖先类。
	//此类结点的孩子结点都对应表达式AST_expression的子类.
	class AST_statement :public AST
	{
	public:
		virtual double value()const { return 0.0; }
		void addExpression(AST_expression* expTree);//追加一个表达式
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
		//用命名的颜色名字来表示颜色的值.
		void ChgColor(module_Lexer::Token &t);
	public:
		static void getDefaultcolor(double &r, double& g, double& b);
	};
	
	class AST_stmt_pixelsize : public AST_statement
	{
	public:
		AST_stmt_pixelsize(module_Lexer::Token& t) : AST_statement(t) {}
	};
//完整输入的树根结点类型。
//其每个孩子对应一条语句(AST_statement对象)

	class AST_stmt_list :public AST
	{
	public:
		//根结点存放被分析的文件名
		AST_stmt_list(module_Lexer::Token& t) :AST(t) {}
		virtual double value()const { return 0.0; }
		//返回第idx+1条语句
		virtual AST_statement* getchild(unsigned int idx) const;
		void addstmt(AST_statement* stmt, int where = 1);//增加一个语句
	};
}//namespace module_Parser
#endif//#ifndef_PARSER_AST_HPP