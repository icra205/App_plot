#pragma once
//�ļ���ui_console.hpp
//���ݣ����ڿ���̨�Ľ������Ͷ��壨C++���԰棩
//���ߣ�WXQ#2019��2818
#ifndef _UI_CONSOLE_HPP
#define _UI_CONSOLE HPP

#include"../include/ui.h"

namespace module_UI
{
	
	//���ڿ���̨�Ľ�������
	class UI_console :public UI
	{
	public:
		//������Ȩ���ɽ������󣬸ö���ִ�г���������߼���
		/*Override*/virtual void run();
		//����һ����
		/*Override*/virtual void _DrawPixel(
			unsigned long x, unsigned long y,//���ڴ����е�����
			const PixelAttibute& attr);
		//��ʾ�ı�
		/*Override*/virtual void _ShowMessage(int flag,//��������Ϣ=0����������Ϣ=1
			const char* msg);
	protected:
		/*Override*/virtual std::string selectFile();
	};
}//namespace module_UI
#endif//#ifndef_UI_CONSOLE_HPP