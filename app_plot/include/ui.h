#pragma once
//�ļ�:ui.hpp
//����:�˻���������ӿ�����(C++��������).
//����:WXQ#2019,2018

#ifndef _UI HPP
#define _UI_HPP

#include<string>//for file path 

namespace module_UI {

	//�˻��������͵Ķ�����ࡣ
	//���������н������Ĺ����ӿڡ�
	class UI
	{
		public:
			struct PixelAttibute//���ص����������
		{
			int size;//���ص����ʾ�ߴ�,ȱʡΪ1
				unsigned char red;//��ɫ֮��ɫ����
				unsigned char green;//��ɫ֮��ɫ����
				unsigned char blue;//��ɫ֮��ɫ����

				PixelAttibute(unsigned char r = 255,
				unsigned char g = 0,
				unsigned char b = 0,
				int sz = 1)
				:red(r),green(g),blue(b),size(sz)
			{
			}
			void setColor(unsigned char r,unsigned char g,unsigned char b)
			{
				red = r,green = g;blue = b;
			}
				void setSize(int sz)
			{
				size = sz;
			}
		};

	public://��������Ľ�������������ԵĲ���ʵ��
		//����׼����׼����֮��Ϳ��Ի�ͼ�ˡ�
		//��׼���ɹ��򷵻�0,���򷵻ط�0��
		virtual int prepare(const char* file_name);

			//������ִ�С������Ƿ���UI������������.
			//����,�����ز����á�����ִ�С���
			virtual bool callInterpreterByUI()const
		{
		return false;
		}

			//������Ȩ���ɽ�������,�ö���ά��/ά�ִ���״ֱ̬���ر�.
			virtual void run() = 0;

			//����һ����
			virtual void _DrawPixel(
			unsigned long x,unsigned long y,//���ڴ����е�����
			const PixelAttibute & attr) = 0;

			//��ʾ�ı���Ϣ��
			virtual void _ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
			const char* msg) = 0;
			//ѡ��һ���������ĺ�����ͼ����Դ�ļ�.
			//***��������δָ���ļ�ʱ,�������ص��ø÷�����
			//Return:
			//��δѡ���ļ��򷵻ؿմ�,����
			//������ѡ���ļ���·����
			virtual std::string selectFile() = 0;
			virtual~UI(){}

			protected:
			std::string SrcFilePath;//���ڴ��Դ�����ļ�·��
	};

	//�˻��������İ�װ����.
	//��������Ա����ⲿ������ز�����
	//**��û��UI����,���ͬ���ڿ���̨��ʾ��Ϣ,���ܻ�ͼ�ġ�
	class UI_wrap
	{
	public://Ϊ��������ģ�����,ȫ����Ϊ��̬����
	//���������õ�UI����
		static void setUIObject(UI* p_obj);
		//���������õ�UI����
		static UI* getUIObject();

		//����һ����
		static void DrawPixel(
			unsigned long x, unsigned long y,//���ڴ����е�����
			const UI::PixelAttibute& attr);

		//��ʾ�ı�
		static void ShowMessage(int flag,//��������Ϣ=0,��������Ϣ=1
			const char* msg);

	private:
		static UI* uiObj;//ȫ��Ψһ���˻��������

		UI_wrap(int);//disable create any instance
	};
}
#endif