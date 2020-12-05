//�ļ�:Parser.cpp
//����:ʵ���﷨������(C++��������)
//����:WXQ#2019,2018

#include <cstdio>//for fopen,fclose
#include<cstdlib>//for malloc(),free(),atexit()
#include <cstdarg>//for va_start(),va_end(),va_arg()

#include"../include/Errlog.h"//������ģ��Ľӿ�
#include"../include/Parser.h"

using namespace module_Lexer;
using namespace module_ErrLog;

namespace module_Parser
{

	Parser::Parser(module_Lexer::Lexer* needScanner)
	{
		theScanner = needScanner;
		{//Ϊ���������AST����һ������ĸ����
			basic_token tv = { NONTOKEN,"0",0,NULL };
			tv.lexeme = theScanner->getFilename();
			Token virtual_tk(tv);
			virtual_tk.setPosition(position(0, 0));
			ASTroot = new AST_stmt_list(virtual_tk);
		}
		indent = 0;
		parameter = 0;
		ptr_t_storage = &parameter;//���Tֵ��ȱʡ�ռ�

		ErrLog::logPrint("Hi,FrontEnd is ready for [%s]...\n", theScanner->getFilename());
	}

	Parser::~Parser()
	{
		//�﷨�����ɵ������ͷ�
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
		FetchToken();//��ȡ��һ���Ǻ�
		program();//�ݹ��½�����

		ErrLog::logPrint("\n\n//whole AST(root is the file name):\n");
		ast_dump(ASTroot);//�������������﷨��
		ErrLog::logPrint("\n//END of Whole AST.\n");

		//������û��һ����ȷ�����,���ؿ���
		if (ASTroot->children_count() < 1)
		{
			delete ASTroot;
			ASTroot = NULL;
		}

		return ASTroot;
	};

//***************program�ĵݹ��ӳ���
	void Parser::program()
	{
		enter("program");
		while (curr_token.getType() != NONTOKEN)
		{
			AST_statement* stmt = statement();
			if (NULL != stmt)
			{
				//��Ϊ��⵽�﷨����֮��,�ᶪ�����ɼǺ�,ֱ��
				//�����������ļǺŲż�����
				//���Կ���ֱ���ļ�ĩβҲƥ�䲻��,�Ǿ�Ҫ������ǰ��䡣
				if (curr_token.getType() == NONTOKEN)
					delete stmt;
				else
					ASTroot->addstmt(stmt);
			}

			MatchToken(SEMICO);
		}
		exit("program");
	}

	//---statement�ĵݹ��ӳ���
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
			/* ����ƥ��ֺ�ʱ�������,�������ʾ���α���
			SyntaxError(2);
			*/
			break;
		}
		exit("statement");

		return root;
	}

	//-origin_statement�ĵݹ��ӳ���
	AST_statement* Parser::origin_statement()
	{
		enter("origin_statement");

		MatchToken(ORIGIN);
		AST_statement* root = new AST_stmt_origin(lastToken);//����ORIGIN�����﷨����

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

	//----scale_statement�ĵݹ��ӳ���
	AST_statement* Parser::scale_statement()
	{
		enter("scale_statement");

		MatchToken(SCALE);
		AST_statement* root = new AST_stmt_scale(lastToken);//����SCALE�����﷨����

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

	//-rot_statement�ĵݹ��ӳ���
	AST_statement* Parser::rot_statement()
	{
		enter("rot_statement");

		MatchToken(ROT);
		AST_statement* root = new AST_stmt_rot(lastToken);//����ROT �����﷨����

		MatchToken(IS);
		AST_expression* ptr = expression();
		root->addExpression(ptr);

		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 

		exit("rot_statement");
		return root;
	}

	//---for_statement�ĵݹ��ӳ���
	AST_statement* Parser::for_statement()
	{
		enter("for_statement");

		MatchToken(FOR);
		AST_statement* root = new AST_stmt_forDraw(lastToken);//����FOR �����﷨����

		MatchToken(T);
		MatchToken(FROM);
		AST_expression* ptr = expression();//���������ʼ���ʽ�﷨��
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE
			ast dump(ptr);//print AST
		#endif 
		MatchToken(TO);
		ptr = expression();//��������ս���ʽ�﷨��
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast dump(ptr);//print AST
		#endif 
		MatchToken(STEP);
		ptr = expression();//��������������ʽ�﷨��
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(DRAW);
		MatchToken(L_BRACKET);
		ptr = expression();//�����������ʽ�﷨��
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(COMMA);
		ptr = expression();//������������ʽ�﷨��
		root->addExpression(ptr);
		#ifdef TRACE_EXPTREE 
			ast_dump(ptr);//print AST
		#endif 
		MatchToken(R_BRACKET);

		exit("for_statement");

		return root;
	}

	//CoLoRstmt:=color is(expr_red,expr_green,expr_blue)
	//--color_statement�ĵݹ��ӳ���
	AST_statement* Parser::color_statement()
	{
		enter("color_statement");

		MatchToken(COLOR);
		AST_stmt_color* root = new AST_stmt_color(lastToken);//����ROT�����﷨����

		MatchToken(IS);

		if (curr_token.getType() == L_BRACKET)//LL(1)
		{
			MatchToken(L_BRACKET);
			AST_expression* ptr = expression();//red part 
			root->addExpression(ptr);
			#ifdef TRACE_EXPTREE 
				ast_dump(ptr);//print AST
			#endif
		//��������.............
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
		AST_stmt_pixelsize* root = new AST_stmt_pixelsize(lastToken);//����ROT�����﷨����
		//��������.............

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


	//expression�ĵݹ��ӳ���
	AST_expression* Parser::expression()
	{
		AST_expression* left, * right;//���������ڵ��ָ��

		enter("expression");
		left = term();//������������ҵõ����﷨��
		while (curr_token.getType() == PLUS || curr_token.getType() == MINUS)
		{
			Token t = curr_token;
			MatchToken(curr_token.getType());
			right = term();//�����Ҳ������ҵõ����﷨��
			left = MakeExprNode(&t, left, right);
			//����������﷨��,���Ϊ������
		}
		exit("expression");
		return left;//�������ձ��ʽ���﷨��
	}

	//---term�ĵݹ��ӳ���
	AST_expression* Parser::term()
	{
		//enter(��term");//̫����,�Ͳ���ʾ�˰�,��ͬ

		AST_expression* left, * right;

		left = factor();
		while (curr_token.getType() == MUL || curr_token.getType() == DIV)
		{
			Token t = curr_token;
			MatchToken(curr_token.getType());
			right = factor();
			left = MakeExprNode(&t, left, right);
		}

		//exit(��term");//̫����,�Ͳ���ʾ�˰�,��ͬ

		return left;
	}

	//-------------------factor�ĵݹ��ӳ���
	AST_expression* Parser::factor()
	{
		//enter(��factor");//̫����,�Ͳ���ʾ�˰�,��ͬ

		AST_expression* left, * right;

		if (curr_token.getType() == PLUS)//ƥ��һԪ������
		{
			MatchToken(PLUS);
			right = factor();//���ʽ�˻�Ϊ�����Ҳ������ı��ʽ
		}
		else if (curr_token.getType() == MINUS)//ƥ��һԪ������
		{
			Token t = curr_token;
			MatchToken(MINUS);//���ʽת��Ϊ��Ԫ������ı��ʽ
			right = factor();

			basic_token tv = { CONST_ID,"0",0,NULL };
			position pos(0, 0);
			Token virtual_op(tv);
			virtual_op.setPosition(pos);

			left = MakeExprNode(&virtual_op);//����������������
			right = MakeExprNode(&t, left, right);
		}
		else
			right = component();//ƥ����ս��component

			//exit(��factor");//̫����,�Ͳ���ʾ�˰�,��ͬ

		return right;
	}

	//component�ĵݹ��ӳ���
	AST_expression* Parser::component()
	{
		//enter(��component");//̫����,�Ͳ���ʾ�˰�,��ͬ

		AST_expression* left, * right;

		left = atom();
		if (curr_token.getType() == POWER)
		{
			Token t = curr_token;
			MatchToken(POWER);
			right = component();//�ݹ����component��ʵ��POWER���ҽ������
			left = MakeExprNode(&t, left, right);
		}

		//exit(��component");//̫����,�Ͳ���ʾ�˰�,��ͬ

		return left;
	}

	//atom�ĵݹ��ӳ���
	AST_expression* Parser::atom()
	{
		//enter(��atom");//̫����,�Ͳ���ʾ�˰�,��ͬ

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

			AST_expression* tmp = expression();//��������
			root = MakeExprNode(&t, tmp);

			MatchToken(R_BRACKET);
		}
		break;

		case L_BRACKET://�﷨���ж�������(ûɶ��)
			MatchToken(L_BRACKET);
			root = expression();
			MatchToken(R_BRACKET);
			break;
		default:
			SyntaxError(2);
			break;
		}

		//exit(��atom");//̫����,�Ͳ���ʾ�˰�,��ͬ

		return root;
	}

	//
	/*************��������***********/
	//

	//ͨ���ʷ��������ӿ�get_token��ȡһ���Ǻ�
	Token_Type Parser::FetchToken()
	{
		Token_Type tk = ERRTOKEN;

		//�����ʷ�����ʱ,ֱ�Ӷ�������ȡ��һ��Ч�Ǻ�
		while (1)
		{
			curr_token = theScanner->GetToken();
			if ((tk = curr_token.getType()) == ERRTOKEN)
			{
				ErrLog::logPrint("discard ERR-text \"%s\" at [%d,%d]��\n",
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

	//ƥ��Ǻ�,����ȡ��һ���Ǻ�
	void Parser::MatchToken(enum Token_Type expected)
	{
		lastToken = curr_token;

		if (curr_token.getType() == expected)//��ƥ�����˼Ǻ�,���ӡ�Ǻ��ı�
		{
			for (int i = 0; i < indent; ++i)
				ErrLog::logPrint("    ");
			ErrLog::logPrint("matchtoken %s\n", curr_token.getText().c_str());

			FetchToken();//���Ŷ�ȡ��һ�Ǻ�
		}
		else//���Ǻ�ƥ�䲻��,��������Ǻ�,ֱ�������������ļǺ�Ϊֹ
		{
			while (1)
			{
				SyntaxError(2);
				ErrLog::logPrint("discard mis-match token \"%s\" at [%d,%d]��\n",
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

	//�﷨������
	void Parser::SyntaxError(int case_of)
	{
		switch (case_of)
		{
		case 1:
			ErrLog::error_msg(curr_token.getPosition().line(),
				curr_token.getPosition().column(),
				"(�ʷ�����)�Ƿ��ı� / �ַ�",
				curr_token.getText().c_str());
			break;
		case 2:
			ErrLog::error_msg(curr_token.getPosition().line(),
				curr_token.getPosition().column(),
				"(Syntax error)unexpected token",//����Ԥ�ڵļǺ�
				curr_token.getText().c_str());
			break;
		}
		//exit(1));
	}

	//�����������ӡ���ʽ���﷨��
	void Parser::PrintSyntaxTree(AST* root, int arg_indent)
	{
		if (NULL == root)
			return;
		int L = arg_indent + indent;//�롰enter..."����
		root->print(L);//�������д�ӡ
		return;
	}

	//�����﷨����һ���ڵ�
	//ʵ���п���������һ�������ĸ����,�亢���ڲ���.��˵��
	AST_expression* Parser::MakeExprNode(Token* p_rt, ...)
	{
		AST_expression* root = NULL;

		va_list arg_ptr;
		va_start(arg_ptr, p_rt);
		Token& rt = *p_rt;
		switch (rt.getType())//���ݼǺŵ�����첻ͬ�Ľڵ�
		{
		case CONST_ID://����,Ҷ�ӽڵ�
			root = new AST_const(rt);
			break;
		case T://����,Ҷ�ӽڵ�
			root = new AST_T(rt, ptr_t_storage);
			break;
		case FUNC://��������,һ�����ӵĸ��ڵ�
			root = new AST_func(rt, (AST*)va_arg(arg_ptr, AST_expression*));
			break;
		default://��Ԫ����,�������ӵĸ��ڵ�
			AST_expression* left//������
				= (AST_expression*)va_arg(arg_ptr, AST_expression*);
			AST_expression* right//������
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

	//�����﷨�������еĸ��ٵ���

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