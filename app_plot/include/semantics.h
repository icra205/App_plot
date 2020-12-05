#pragma once
#ifndef _SEMANTICS_HPP
#define _SEMANTICS_HPP

#include"Parser.h"
#include"art.h"
#include"ui.h"//use type PixelAttibute 

namespace module_Semantic
{
	// ��������������͡�
	//���ڽ���������,����������������ۺϺ�ˡ�
	//�����﷨�����������﷨��Ϊ����,�߶������,�߼���(��ͼ).
	class SemanticAnalyzer
	{
	public:
		SemanticAnalyzer(module_Parser::Parser* p);

		//�����������ڲ�����
		//ͨ����������������﷨����ɽ���ִ�У�
		//������õĲ�����:����������﷨������������ȷ�׶Σ�
		int run(module_Parser::AST_stmt_list* theprogram);

	private://helper functions
		//���㱻���Ƶ������
		void Calccoord(module_Parser::AST* x_tree, module_Parser::AST* y_tree,
			double* ptr_x_value, double* ptr_y_value);
		//ѭ���������ص�����겢������Щ��
		void Drawloop(module_Parser::AST* start_tree,
			module_Parser::AST* end_tree,
			module_Parser::AST* step_tree,
			module_Parser::AST* x_tree,
			module_Parser::AST* y_tree);

	private:
		module_Parser::Parser* theParser;
		void Zorro(module_Parser::AST_stmt_list*);

	private://����������ڲ�������:��ͼ����
		module_UI::UI::PixelAttibute attr;//���ص�����
		double origin_x, origin_y;//�ᡢ��ƽ�ƾ���,ȱʡ��ƽ��
		double Scale_x, Scale_y;
		//�ᡢ�ݱ�������,ȱʡ������
		double rot_angle;
		//��ת�Ƕ�,ȱʡ����ת
		double myT_storage;//����������ṩ��Tֵ�Ĵ洢λ��
	};//namespace module_Semantic
}
#endif