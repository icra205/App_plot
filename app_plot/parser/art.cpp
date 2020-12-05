//文件:ast.cpp
//内容:语法树的类型的成员函数实现(C++面向对象版)
//作者:WXQ#2019,2018

#include<vector>
#include<cmath>//pow()
#include<map>

#include"../include/art.h"
#include"../include/Errlog.h"

using namespace std;

using namespace module_Lexer;
using namespace module_ErrLog;

namespace module_Parser
{

//在头文件中未声明存储孩子结点的数据结构类型,所以
// 在此处定义,以便于变更该类型时,对使用者的代码无影响。

typedef std::vector < AST *>t_children;
typedef t_children::iterator t_children_iterator;
#define CHILDREN (*(t_children *)(this->children))

//
//class AST的部分成员实现
//

AST::AST(Token& t) :mToken(t), children(NULL)
{
	children = new t_children();
};

AST::~AST()
{
	if (NULL == children)
		return;

	t_children_iterator itr = CHILDREN.begin();
	for (; itr != CHILDREN.end(); ++itr)
		delete* itr;

	delete(t_children*)(this->children);
};

AST* AST::getchild(unsigned int idx)const//返回第idx+1个孩子
{
	if (idx >= CHILDREN.size())
		return NULL;
	else return CHILDREN[idx];
};

size_t AST::children_count()const
{
	return CHILDREN.size();
};

double AST::childValue(unsigned int idx)const//返回第idx+1个孩子的值
{
	AST* C = getchild(idx);
	return(C == NULL) ? 0.0 : C->value();
};

void AST::print(int ident)const
{
	for (int i = 0; i < ident; ++i)//缩进一下先
		ErrLog::logPrint(" ");

	//先打印结点自身
#if 0
	ErrLog::logPrint("%s\ttype=%s\tline=%u col=%u\n",
		mToken.getText().c_str(),
		token_type_str(mToken.getType()),
		mToken.getPosition().line(),
		mToken.getPosition().column());
#else 
	ErrLog::logPrint("%s\n", mToken.getText().c_str());
#endif 
	t_children_iterator itr = CHILDREN.begin();
	for (; itr!= CHILDREN.end(); ++itr)//打印孩子们
		(*itr)->print(ident + 2);
};

//
//class AST的子类们的部分成员实现
//

AST_binary::AST_binary(Token& t, AST* L, AST* R) : AST_expression(t)
{
	CHILDREN.push_back(L);
	CHILDREN.push_back(R);
};

double AST_power::value() const
{
	return pow(leftValue(), rightValue());
};

AST_unary::AST_unary(Token& t, AST* C) : AST_expression(t)
{
	CHILDREN.push_back(C);
};

void AST_statement::addExpression(AST_expression* expTree)
{
	CHILDREN.push_back(expTree);
};

AST_statement* AST_stmt_list::getchild(unsigned int idx) const
{
	if (idx >= CHILDREN.size())
		return NULL;
	else
		return(AST_statement*)CHILDREN[idx];
};
void AST_stmt_list::addstmt(AST_statement* stmt, int where)
{
	if (where == 1)
		CHILDREN.push_back(stmt);
	else
	{
		CHILDREN.insert(CHILDREN.begin(), stmt);
	}
};

namespace
{
	typedef unsigned int color_int;
	map<string, color_int> namedColorMap;

#define AddColorName(name,r,g,b)\
		namedColorMap.insert(     \
			std::pair<string,color_int>(\
				#name,(((unsigned char)(r)) << 16 | \
					((unsigned char)(g)) << 8 | \
						((unsigned char)(b)))))

	void init_colormap()
	{
		namedColorMap.clear();

		//添加一些你命名的颜色定义
		AddColorName(RED, 255, 0, 0);
		AddColorName(GREEN, 0, 255, 0);
		AddColorName(BLUE, 0, 0, 255);
		AddColorName(BLACK, 255, 255, 255);
		//你可以在增加其他命名的颜色定义........,比如BLUE,BLACK,CYAN,etc.
	}

	bool colorIsDefined(const std::string& colorName)
	{
		if (namedColorMap.size() < 1)
			init_colormap();
		map<string, color_int>::iterator it = namedColorMap.find(colorName);
		if (it == namedColorMap.end())
			return false;
		else return true;
	}

	bool getRGBbyName(const std::string& colorName, double& r, double& g, double& b)
	{
		if (namedColorMap.size() < 1)
			init_colormap();

		map<string, color_int>::iterator it = namedColorMap.find(colorName);
		if (it == namedColorMap.end())
		{
			AST_stmt_color::getDefaultcolor(r, g, b);
			return false;
		}
		color_int n = it->second;
		r = (n & 0x00FF0000) >> 16;
		g = (n & 0x0000FF00) >> 8;
		b = (n & 0x000000FF);
		return true;
	}

}//namespace

	void AST_ColorName::getRGB(double& r, double& g, double& b)const
	{
		getRGBbyName(mToken.getUpperText(), r, g, b);
	}

	void AST_stmt_color::ChgColor(module_Lexer::Token & t)
	{
		if (!colorIsDefined(t.getUpperText()))
		{
			ErrLog::error_msg(t.getPosition().line(),
				t.getPosition().column(),
				"Unknown color name: ", t.getText().c_str());
		}

		//语法分析生成AST时,不检查名字的有效性;而是由语义分析完成
		AST_ColorName* pColorAst = new AST_ColorName(t);
		CHILDREN.push_back(pColorAst);
	}

	void AST_stmt_color::getDefaultcolor(double& r, double& g, double& b)
	{
		r = 255;
		g = 0;
		b = 0;
	}
}//namespace module_Parser