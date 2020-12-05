#pragma once
#ifndef _UI_CONSOLE_WIN32_HPP
#define _UI_CONSOLE_WIN32_HPP
#include"ui_win32.h"
namespace module_UI
{ 
	//���ڿ���̨�Ľ�������
	class UI_console_win32:public UI_win32
	{
	public:
		UI_console_win32();
	~UI_console_win32();

	/*���û����public����*/
	public:
	//����׼��
	/*Override*/virtual int prepare(const char* file_name);
	/*Override*/virtual bool callInterpreterByUI()const 
	{ return false; }
	//����һ����
	/*Override*/virtual void _DrawPixel(
		unsigned long x, unsigned long y,//���ڴ����е�����
		const PixelAttibute& attr);
	//��ʾ�ı�
	/*Override*/virtual void _ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
	const char* msg);

	/*���û���UI_win32���ڲ�����*/
	protected:
	/*Override*/virtual LRESULT CALLBACK WndProc(HWND hwin,UINT Message,WPARAM WParam,LPARAM lParam);
	/*Override*/virtual void ProcessFile();
	//��Կ���̨���ڵĲ���
		private:
		void AttachMyConsole();//����һ������̨����
		void DetachMyConsole();//���ٿ���̨����
		void ClearConsole();
		void prepareMemoryobject();//׼���ڴ滺����
	/*���ݳ�Ա��һ���ּ̳���UI_win32,�˴����趨��������*/
		private:
		HDC dcBuffer;//˫������Ҫ���ڴ��ͼ����
		HBITMAP memBM;//˫������Ҫ���ڴ�ͼ��
	};
}//namespace module_UI
#endif//#ifndef_UI_CONSOLE_WIN32_HPP