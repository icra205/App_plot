//文件:Parser.cpp
//内容:实现语法分析器(C++面向对象版)
//作者:WXQ#2019,2018

#include <cstdio>//for fopen,fclose
#include<cstdlib>//for malloc(),free(),atexit()
#include <cstdarg>//for va_start(),va_end(),va_arg()

#include"../include/Errlog.h"//错误处理模块的接口
#include"../include/Parser.h"

using namespace module_Lexer;
using namespace module_ErrLog;

namespace module_Parser
{

	Parser::Parser(module_Lexer::Lexer* needScanner)
	{
		theScanner = needScanner;
		{//为完整输入的AST构造一个虚拟的根结点
			basic_token tv = { NONTOKEN,"0",0,NULL };
			tv.lexeme = theScanner->getFilename();
			Token virtual_tk(tv);
			virtual_tk.setPosition(position(0, 0));
			ASTroot = new AST_stmt_list(virtual_tk);
		}
		indent = 0;
		parameter = 0;
		ptr_t_storage = &parameter;//存放T值的缺省空间

		ErrLog::logPrint("Hi,FrontEnd is ready for [%s]...\n", theScanner->getFilename());
	}

	Parser::~Parser()
	{
		//语法树交由调用者释放
		//delete ASTroot;
	}

	const char* Parser::getFilename()const
	{
		if (NULL == theScanner)
			return NULL;
		else
			return theScanner->getFilename();
	};

	AST_stmt_list* Parser::run()
	{
		FetchToken();//获取第一个记号
		program();//递归下降分析

		ErrLog::logPrint("\n\n//whole AST(root is the file name):\n");
		ast_dump(ASTroot);//输出完整输入的语法树
		ErrLog::logPrint("\n//END of Whole AST.\n");

		//输入中没有一个正确的语句,返回空树
		if (ASTroot->children_count() < 1)
		{
			delete ASTroot;
			ASTroot = NULL;
		}

		return ASTroot;
	};

//***************program的递归子程序
	void Parser::program()
	{
		enter("program");
		while (curr_token.getType() != NONTOKEN)
		{
			AST_statement* stmt = statement();
			if (NULL != stmt)
			{
				//因为检测到语法错误之后,会丢弃若干记号,直到
				//看到了期望的记号才继续。
				//所以可能直到文件末尾也匹配不上,那就要丢弃当前语句。
				if (curr_token.getType() == NONTOKEN)
					delete stmt;
				else
					ASTroot->addstmt(stmt);
			}

			MatchToken(SEMICO);
		}
		exit("program");
	}

	//---statement的递归子程序
	AST_statement* Parser::statement()
	{
		enter("statement");

		AST_statement* root = NULL;
		switch (curr_token.getType())//FIRST(statement)of LL(1)grammar 
		{
		case ORIGIN:
			root = origin_statement();
			break;
		case SCALE:
			root = scale_statement();
			break;
		case ROT:
			root = rot_statement();
			break;
		case FOR:
			root = for_statement();
			break;
		case COLOR:
			root = color_statement();
			break;
		case SIZE:
			root = size_statement();
			break;
		default:
			/* 将在匹配分号时报告错误,否则会显示两次报告
			SyntaxError(2);
			*/
			break;
		}
		exit("statement");

		return root;
	}

	//-origin_statement的递归子程序
	AST_statement* Parser::origin_statement()
	{
		enter("origin_statement");

		MatchToken(ORIGIN);
		AST_statement* root = new AST_stmt_origin(lastToken);//生成ORIGIN语句的语法树根

		MatchToken(IS);
		MatchToken(L_BRACKET);
		AST_expression* ptr = expression();
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(COMMA);
		ptr = expression();
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE
			ast_dump(ptr);// print AST
		#endif 
		MatchToken(R_BRACKET);

		exit("origin_statement");
		return root;
	}

	//----scale_statement的递归子程序
	AST_statement* Parser::scale_statement()
	{
		enter("scale_statement");

		MatchToken(SCALE);
		AST_statement* root = new AST_stmt_scale(lastToken);//生成SCALE语句的语法树根

		MatchToken(IS);
		MatchToken(L_BRACKET);
		AST_expression* ptr = expression();
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST 
		#endif
		MatchToken(COMMA);
		ptr = expression();
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(R_BRACKET);

		exit("scale_statement");
		return root;
	}

	//-rot_statement的递归子程序
	AST_statement* Parser::rot_statement()
	{
		enter("rot_statement");

		MatchToken(ROT);
		AST_statement* root = new AST_stmt_rot(lastToken);//生成ROT 语句的语法树根

		MatchToken(IS);
		AST_expression* ptr = expression();
		root->addExpression(ptr);

		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 

		exit("rot_statement");
		return root;
	}

	//---for_statement的递归子程序
	AST_statement* Parser::for_statement()
	{
		enter("for_statement");

		MatchToken(FOR);
		AST_statement* root = new AST_stmt_forDraw(lastToken);//生成FOR 语句的语法树根

		MatchToken(T);
		MatchToken(FROM);
		AST_expression* ptr = expression();//构造参数起始表达式语法树
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE
			ast dump(ptr);//print AST
		#endif 
		MatchToken(TO);
		ptr = expression();//构造参数终结表达式语法树
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast dump(ptr);//print AST
		#endif 
		MatchToken(STEP);
		ptr = expression();//构造参数步长表达式语法树
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(DRAW);
		MatchToken(L_BRACKET);
		ptr = expression();//构造横坐标表达式语法树
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(COMMA);
		ptr = expression();//构造纵坐标表达式语法树
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(R_BRACKET);

		exit("for_statement");

		return root;
	}

	//CoLoRstmt:=color is(expr_red,expr_green,expr_blue)
	//--color_statement的递归子程序
	AST_statement* Parser::color_statement()
	{
		enter("color_statement");

		MatchToken(COLOR);
		AST_stmt_color* root = new AST_stmt_color(lastToken);//生成ROT语句的语法树根

		MatchToken(IS);

		if (curr_token.getType() == L_BRACKET)//LL(1)
		{
			MatchToken(L_BRACKET);
			AST_expression* ptr = expression();//red part 
			root->addExpression(ptr);
			#ifdef TRACE_EXPTREE 
				ast_dump(ptr);//print AST
			#endif
		//你来补充.............
			MatchToken(COMMA);
			ptr = expression();//green part
			root->addExpression(ptr);
			#ifdef TRACE_EXPTREE 
				ast_dump(ptr);//print AST
			#endif
			MatchToken(COMMA);
			ptr = expression();//green part
			root->addExpression(ptr);
			#ifdef TRACE_EXPTREE 
				ast_dump(ptr);//print AST
			#endif 
			MatchToken(R_BRACKET);
		}
		else
		{
			MatchToken(ID);
			root->ChgColor(lastToken);
		}

		exit("color_statement");
		return root;
	}

	AST_statement* Parser::size_statement()
	{
		enter("size_statement");

		MatchToken(SIZE);
		AST_stmt_pixelsize* root = new AST_stmt_pixelsize(lastToken);//生成ROT语句的语法树根
		//你来补充.............

		MatchToken(IS);

		if (curr_token.getType() == ID)//LL(1)
		{
			MatchToken(ID);
		}
		else
		{
			AST_expression* ptr = expression();//red part 
			root->addExpression(ptr);
			#ifdef TRACE_EXPTREE 
				ast_dump(ptr);//print AST
			#endif
		}

		exit("size_statement");
		return root;
	}


	//expression的递归子程序
	AST_expression* Parser::expression()
	{
		AST_expression* left, * right;//左右子树节点的指针

		enter("expression");
		left = term();//分析左操作数且得到其语法树
		while (curr_token.getType() == PLUS || curr_token.getType() == MINUS)
		{
			Token t = curr_token;
			MatchToken(curr_token.getType());
			right = term();//分析右操作数且得到其语法树
			left = MakeExprNode(&t, left, right);
			//构造运算的语法树,结果为左子树
		}
		exit("expression");
		return left;//返回最终表达式的语法树
	}

	//---term的递归子程序
	AST_expression* Parser::term()
	{
		//enter(“term");//太多了,就不显示了吧,下同

		AST_expression* left, * right;

		left = factor();
		while (curr_token.getType() == MUL || curr_token.getType() == DIV)
		{
			Token t = curr_token;
			MatchToken(curr_token.getType());
			right = factor();
			left = MakeExprNode(&t, left, right);
		}

		//exit(“term");//太多了,就不显示了吧,下同

		return left;
	}

	//-------------------factor的递归子程序
	AST_expression* Parser::factor()
	{
		//enter(“factor");//太多了,就不显示了吧,下同

		AST_expression* left, * right;

		if (curr_token.getType() == PLUS)//匹配一元加运算
		{
			MatchToken(PLUS);
			right = factor();//表达式退化为仅有右操作数的表达式
		}
		else if (curr_token.getType() == MINUS)//匹配一元减运算
		{
			Token t = curr_token;
			MatchToken(MINUS);//表达式转化为二元减运算的表达式
			right = factor();

			basic_token tv = { CONST_ID,"0",0,NULL };
			position pos(0, 0);
			Token virtual_op(tv);
			virtual_op.setPosition(pos);

			left = MakeExprNode(&virtual_op);//构造虚拟的左操作数
			right = MakeExprNode(&t, left, right);
		}
		else
			right = component();//匹配非终结符component

			//exit(“factor");//太多了,就不显示了吧,下同

		return right;
	}

	//component的递归子程序
	AST_expression* Parser::component()
	{
		//enter(“component");//太多了,就不显示了吧,下同

		AST_expression* left, * right;

		left = atom();
		if (curr_token.getType() == POWER)
		{
			Token t = curr_token;
			MatchToken(POWER);
			right = component();//递归调用component以实现POWER的右结合性质
			left = MakeExprNode(&t, left, right);
		}

		//exit(“component");//太多了,就不显示了吧,下同

		return left;
	}

	//atom的递归子程序
	AST_expression* Parser::atom()
	{
		//enter(“atom");//太多了,就不显示了吧,下同

		AST_expression* root = NULL;

		switch (curr_token.getType())//LL(1)
		{
		case CONST_ID:
			MatchToken(CONST_ID);
			root = MakeExprNode(&lastToken);
			break;
		case T:
			MatchToken(T);
			root = MakeExprNode(&lastToken);
			break;
		case FUNC:
		{
			MatchToken(FUNC);
			Token t = lastToken;
			MatchToken(L_BRACKET);

			AST_expression* tmp = expression();//参数的树
			root = MakeExprNode(&t, tmp);

			MatchToken(R_BRACKET);
		}
		break;

		case L_BRACKET://语法树中丢弃括号(没啥用)
			MatchToken(L_BRACKET);
			root = expression();
			MatchToken(R_BRACKET);
			break;
		default:
			SyntaxError(2);
			break;
		}

		//exit(“atom");//太多了,就不显示了吧,下同

		return root;
	}

	//
	/*************辅助函数***********/
	//

	//通过词法分析器接口get_token获取一个记号
	Token_Type Parser::FetchToken()
	{
		Token_Type tk = ERRTOKEN;

		//遇到词法错误时,直接丢弃并读取下一有效记号
		while (1)
		{
			curr_token = theScanner->GetToken();
			if ((tk = curr_token.getType()) == ERRTOKEN)
			{
				ErrLog::logPrint("discard ERR-text \"%s\" at [%d,%d]！\n",
					curr_token.getText().c_str(),
					curr_token.getPosition().line(),
					curr_token.getPosition().column());
				SyntaxError(1);
				continue;
			}
			break;
		}

		return tk;
	}

	//匹配记号,并获取下一个记号
	void Parser::MatchToken(enum Token_Type expected)
	{
		lastToken = curr_token;

		if (curr_token.getType() == expected)//若匹配上了记号,则打印记号文本
		{
			for (int i = 0; i < indent; ++i)
				ErrLog::logPrint("    ");
			ErrLog::logPrint("matchtoken %s\n", curr_token.getText().c_str());

			FetchToken();//接着读取下一记号
		}
		else//若记号匹配不上,则丢弃错误记号,直到遇到所期望的记号为止
		{
			while (1)
			{
				SyntaxError(2);
				ErrLog::logPrint("discard mis-match token \"%s\" at [%d,%d]！\n",
					curr_token.getText().c_str(),
					curr_token.getPosition().line(),
					curr_token.getPosition().column());
				Token_Type tk = FetchToken();
				if (tk == expected)
				{
					ErrLog::logPrint("*** matchtoken %s after discard~~~\n",
						curr_token.getText().c_str());
					lastToken = curr_token;
					FetchToken();
					break;
				}
				else if (tk == NONTOKEN)
					break;
			}
		}
	}

	//语法错误处理
	void Parser::SyntaxError(int case_of)
	{
		switch (case_of)
		{
		case 1:
			ErrLog::error_msg(curr_token.getPosition().line(),
				curr_token.getPosition().column(),
				"(词法错误)非法文本 / 字符",
				curr_token.getText().c_str());
			break;
		case 2:
			ErrLog::error_msg(curr_token.getPosition().line(),
				curr_token.getPosition().column(),
				"(Syntax error)unexpected token",//不是预期的记号
				curr_token.getText().c_str());
			break;
		}
		//exit(1));
	}

	//先序遍历并打印表达式的语法树
	void Parser::PrintSyntaxTree(AST* root, int arg_indent)
	{
		if (NULL == root)
			return;
		int L = arg_indent + indent;//与“enter..."对齐
		root->print(L);//让树自行打印
		return;
	}

	//生成语法树的一个节点
	//实际有可能是生成一个子树的根结点,其孩子在参数.中说明
	AST_expression* Parser::MakeExprNode(Token* p_rt, ...)
	{
		AST_expression* root = NULL;

		va_list arg_ptr;
		va_start(arg_ptr, p_rt);
		Token& rt = *p_rt;
		switch (rt.getType())//根据记号的类别构造不同的节点
		{
		case CONST_ID://常数,叶子节点
			root = new AST_const(rt);
			break;
		case T://参数,叶子节点
			root = new AST_T(rt, ptr_t_storage);
			break;
		case FUNC://函数调用,一个孩子的父节点
			root = new AST_func(rt, (AST*)va_arg(arg_ptr, AST_expression*));
			break;
		default://二元运算,两个孩子的父节点
			AST_expression* left//左子树
				= (AST_expression*)va_arg(arg_ptr, AST_expression*);
			AST_expression* right//右子树
				= (AST_expression*)va_arg(arg_ptr, AST_expression*);

			switch (rt.getType())
			{
			case PLUS:
				root = new AST_plus(rt, left, right);
				break;
			case MINUS:
				root = new AST_minus(rt, left, right);
				break;
			case MUL:
				root = new AST_multiple(rt, left, right);
				break;
			case DIV:
				root = new AST_div(rt, left, right);
				break;
			case POWER:
				root = new AST_power(rt, left, right);
				break;
			}
			break;
		}
		va_end(arg_ptr);

		return root;
	}

	//用于语法分析器中的跟踪调试

	void Parser::enter(const char* nonTerminalName)
	{
		int i;
		for (i = 0; i < indent; ++i)
			ErrLog::logPrint("    ");
		ErrLog::logPrint("enter in %s\n", nonTerminalName);
		indent += 2;
	}

	void Parser::exit(const char* nonTerminalName)
	{
		int i;
		indent -= 2;
		for (i = 0; i < indent; ++i)
			ErrLog::logPrint(" ");
		ErrLog::logPrint("exit from %s\n", nonTerminalName);
	}

	void Parser::ast_dump(AST* root)
	{
		PrintSyntaxTree(root, 0);
	}
}