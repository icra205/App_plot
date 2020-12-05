//�ļ�:ast.cpp
//����:�﷨�������͵ĳ�Ա����ʵ��(C++��������)
//����:WXQ#2019,2018

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

//��ͷ�ļ���δ�����洢���ӽ������ݽṹ����,����
// �ڴ˴�����,�Ա��ڱ��������ʱ,��ʹ���ߵĴ�����Ӱ�졣

typedef std::vector < AST *>t_children;
typedef t_children::iterator t_children_iterator;
#define CHILDREN (*(t_children *)(this->children))

//
//class AST�Ĳ��ֳ�Աʵ��
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

AST* AST::getchild(unsigned int idx)const//���ص�idx+1������
{
	if (idx >= CHILDREN.size())
		return NULL;
	else return CHILDREN[idx];
};

size_t AST::children_count()const
{
	return CHILDREN.size();
};

double AST::childValue(unsigned int idx)const//���ص�idx+1�����ӵ�ֵ
{
	AST* C = getchild(idx);
	return(C == NULL) ? 0.0 : C->value();
};

void AST::print(int ident)const
{
	for (int i = 0; i < ident; ++i)//����һ����
		ErrLog::logPrint(" ");

	//�ȴ�ӡ�������
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
	for (; itr!= CHILDREN.end(); ++itr)//��ӡ������
		(*itr)->print(ident + 2);
};

//
//class AST�������ǵĲ��ֳ�Աʵ��
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

		//���һЩ����������ɫ����
		AddColorName(RED, 255, 0, 0);
		AddColorName(GREEN, 0, 255, 0);
		AddColorName(BLUE, 0, 0, 255);
		AddColorName(BLACK, 255, 255, 255);
		//�����������������������ɫ����........,����BLUE,BLACK,CYAN,etc.
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

		//�﷨��������ASTʱ,��������ֵ���Ч��;����������������
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