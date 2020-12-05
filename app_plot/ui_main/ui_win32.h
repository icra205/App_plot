#pragma once
#ifndef _UI_WINDOW32_HPP
#define _UI_WINDOW32_HPP
/*��ģ�����Windows APIʵ��,Ҳ������
��Win32Application����Ӧ�õ�������WinMain()
*/

#include <windows.h>//windows API
#include <wingdi.h>
//#include <winuser.h>//include windows.h

#include<string>
#include"../include/ui.h"

namespace module_UI
{ 
//���ڿ���̨�Ľ�������
	class UI_win32 :public UI
	{
	public:
		//����׼��
		/*override*/virtual int prepare(const char* file_name);
		/*Override*/virtual bool callInterpreterByUI()const
		{
			return true;
		}

		//������Ȩ���ɽ�������,�ö���ִ�г���������߼�,
		/*Override*/virtual void run();
		//����һ����
		/*Override*/virtual void _DrawPixel(
			unsigned long x, unsigned long y,//���ڴ����е�����
			const PixelAttibute& attr);
		//��ʾ�ı�
		/*override*/virtual void _ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
			const char* msg);

	public:
		UI_win32();

	protected:
		//��ʼ������
		virtual int Preparewindow(HINSTANCE hInst);

		//���ڴ�����
		virtual LRESULT CALLBACK WndProc(HWND hWin, UINT Message, WPARAM wParam, LPARAM lParam);
		//����������ִ���ļ�
		virtual void ProcessFile();

		//��Ϣ������
		static LRESULT CALLBACK
			process_message(
				HWND hwin,
				UINT Message,
				WPARAM wParam,
				LPARAM lParam);

	protected:
		/*Override*/virtual std::string selectFile();

	protected:
		HDC hDC;//handle of display device context 
		HWND hMainwindow;//handle of main window 

		std::string winName;//������
	};
}//namespace module_UI
#endif//#ifndef_UI_WINDOW32_HPP