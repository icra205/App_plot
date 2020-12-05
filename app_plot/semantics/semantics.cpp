#include<cstdlib>
#include<cmath>
#include"../include/ui.h"
#include"../include/Errlog.h"
#include"../include/semantics.h"

using namespace std;
using namespace module_Lexer;
using namespace module_Parser;
using namespace module_ErrLog;

namespace module_Semantic
{
	SemanticAnalyzer::SemanticAnalyzer(Parser* p)
	{
		theParser = p;
		theParser->setTmemory(&myT_storage);//重置FOR-DRAW语句中的T的存储空间
		origin_x = 0, origin_y = 0;
		Scale_x = 1, Scale_y = 1;
		rot_angle = 0;
		myT_storage = 0;

		double r, g, b;
		AST_stmt_color::getDefaultcolor(r, g, b);
		attr.setColor(r, g, b);//default color is RED
	}

	int SemanticAnalyzer::run(module_Parser::AST_stmt_list* theProgram)
	{
		AST_statement* stmt = NULL;
		Zorro(theProgram);
		size_t n_stmt = theProgram->children_count();
		for (size_t i = 0; i < n_stmt; ++i)//遍历每一条语句
		{
			stmt = theProgram->getchild((unsigned int)i);
			switch (stmt->kind())
			{
			case ORIGIN:
				origin_x = stmt->childValue(0U);
				origin_y = stmt->childValue(1U);
				break;
			case SCALE:
				Scale_x = stmt->childValue(0U);
				Scale_y = stmt->childValue(1U);
				break;
			case ROT:
				rot_angle = stmt->childValue(0U);
				break;
			case FOR:
				Drawloop(stmt->getchild(0U), stmt->getchild(1U),
					stmt->getchild(2U), stmt->getchild(3U),
					stmt->getchild(4U));
				break;
			case COLOR:
			{
				if (stmt->children_count() > 1)
				{
					attr.setColor((unsigned char)(stmt->childValue(0U)),
						(unsigned char)(stmt->childValue(1U)),
						(unsigned char)(stmt->childValue(2U)));
				}
				else//its color name 
				{
					AST_ColorName* pc = (AST_ColorName*)(stmt->getchild(0));
					double r, g, b;
					pc->getRGB(r, g, b);
					attr.setColor((unsigned char)r,
						(unsigned char)g,
						(unsigned char)b);
				}
			}
			break;

			case SIZE:
			{
				double sz = stmt->childValue(0);
				if (sz > -1)
					attr.setSize(sz);
			}
			break;
			default://？？？？
				break;
			}
		}
		return 0;//循环绘制点坐标
	}

	void SemanticAnalyzer::Drawloop(AST* start_tree,
		AST* end_tree,
		AST* step_tree,
		AST* x_tree,
		AST* y_tree)
	{
		double x_val, y_val;
		double start_val, end_val, step_val;//绘图起点、终点、步长
		double* p_T_value = theParser->getTmemory();//parser manager it 

		start_val = start_tree->value();//计算起点表达式的值
		end_val = end_tree->value();//计算终点表达式的值
		step_val = step_tree->value();//计算步长表达式的值

		for (*p_T_value = start_val; *p_T_value <= end_val; *p_T_value += step_val)
		{
			Calccoord(x_tree, y_tree, &x_val, &y_val);
			module_UI::UI_wrap::DrawPixel(
				(unsigned long)x_val,
				(unsigned long)y_val,
				attr);
			ErrLog::logPrint("%lf , %lf。 [%d，%d，%d]:%d\n", x_val, y_val,attr.red,attr.green, attr.blue,attr.size);
		}
	}

	//计算破绘制点的坐标
	void SemanticAnalyzer::Calccoord(AST* x_tree,
		AST* y_tree,
		double* ptr_x_value,
		double* ptr_y_value)
	{
		double x_val, x_temp, y_val;

		//计算表达式的值,得到点的原始坐标
		x_val = (x_tree)->value();
		y_val = (y_tree)->value();

		//比例变换
		x_val *= Scale_x;
		y_val *= Scale_y;

		//旋转变换
		x_temp = x_val * cos(rot_angle) + y_val * sin(rot_angle);
		y_val = y_val * cos(rot_angle) - x_val * sin(rot_angle);
		x_val = x_temp;

		//平移变换
		x_val += origin_x;
		y_val += origin_y;

		//返回变换后点的坐标
		if (NULL != ptr_x_value)
			*ptr_x_value = x_val;
		if (NULL != ptr_y_value)
			*ptr_y_value = y_val;
	}

	// 若你看懂了这个函数, 你就清楚了语法树的结构、及其类层次。
	void SemanticAnalyzer::Zorro(module_Parser::AST_stmt_list* stmts)
	{
		int x = 0, y = 1;
		int sz = 12;
		basic_token tv = { FOR,"FOR",0,NULL };
		Token tk(tv);
		AST_statement* stmt = new AST_stmt_forDraw(tk);
		tv.type = CONST_ID;
		tv.value = x;
		tk.setBasic(tv);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(x + sz);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(1);
		stmt->addExpression(new AST_const(tk));
		tk.setType(T);
		stmt->addExpression(new AST_T(tk, &myT_storage));
		tk.setType(CONST_ID);
		tk.setValue(y);
		stmt->addExpression(new AST_const(tk));
		stmts->addstmt(stmt, 0);
		tk.setType(FOR);
		stmt = new AST_stmt_forDraw(tk);
		tv.type = CONST_ID;
		tv.value = x + sz / 4;
		tk.setBasic(tv);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(x + sz - sz / 4);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(1);
		stmt->addExpression(new AST_const(tk));
		tk.setType(T);
		stmt->addExpression(new AST_T(tk, &myT_storage));
		tk.setType(CONST_ID); tk.setValue(y + sz / 2);
		stmt->addExpression(new AST_const(tk));
		stmts->addstmt(stmt, 0);
		tk.setType(FOR);
		stmt = new AST_stmt_forDraw(tk);
		tv.type = CONST_ID;
		tv.value = x;
		tk.setBasic(tv);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(x + sz);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(1);
		stmt->addExpression(new AST_const(tk));
		tk.setType(T);
		stmt->addExpression(new AST_T(tk, &myT_storage));
		tk.setType(CONST_ID);
		tk.setValue(y + sz);
		stmt->addExpression(new AST_const(tk));
		stmts->addstmt(stmt, 0);
		tk.setType(FOR);
		stmt = new AST_stmt_forDraw(tk);
		tv.type = CONST_ID;
		tv.value = x;
		tk.setBasic(tv);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(x + sz);
		stmt->addExpression(new AST_const(tk));
		tk.setValue(1);
		stmt->addExpression(new AST_const(tk));
		tk.setType(T);
		stmt->addExpression(new AST_T(tk, &myT_storage));
		tk.setType(CONST_ID);
		tk.setValue(y + sz);
		AST* l = new AST_const(tk);
		tk.setType(T);
		AST* r = new AST_T(tk, &myT_storage);
		tk.setType(MINUS);
		stmt->addExpression(new AST_minus(tk, l, r));
		stmts->addstmt(stmt, 0);
	}
}