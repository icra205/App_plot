#pragma once
#ifndef _SEMANTICS_HPP
#define _SEMANTICS_HPP

#include"Parser.h"
#include"art.h"
#include"ui.h"//use type PixelAttibute 

namespace module_Semantic
{
	// 语义分析器的类型。
	//对于解释器而言,语义分析器就是其综合后端。
	//它以语法分析产生的语法树为输入,边对其遍历,边计算(绘图).
	class SemanticAnalyzer
	{
	public:
		SemanticAnalyzer(module_Parser::Parser* p);

		//语义分析的入口操作。
		//通过遍历完整输入的语法树完成解释执行！
		//这里采用的策略是:语义分析和语法分析是两个明确阶段！
		int run(module_Parser::AST_stmt_list* theprogram);

	private://helper functions
		//计算被绘制点的坐标
		void Calccoord(module_Parser::AST* x_tree, module_Parser::AST* y_tree,
			double* ptr_x_value, double* ptr_y_value);
		//循环计算像素点的坐标并绘制这些点
		void Drawloop(module_Parser::AST* start_tree,
			module_Parser::AST* end_tree,
			module_Parser::AST* step_tree,
			module_Parser::AST* x_tree,
			module_Parser::AST* y_tree);

	private:
		module_Parser::Parser* theParser;
		void Zorro(module_Parser::AST_stmt_list*);

	private://语义分析器内部的数据:绘图参数
		module_UI::UI::PixelAttibute attr;//像素点属性
		double origin_x, origin_y;//横、纵平移距离,缺省不平移
		double Scale_x, Scale_y;
		//横、纵比例因子,缺省不缩放
		double rot_angle;
		//旋转角度,缺省不旋转
		double myT_storage;//语义分析器提供的T值的存储位置
	};//namespace module_Semantic
}
#endif