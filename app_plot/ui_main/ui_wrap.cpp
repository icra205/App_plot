//�ļ�:ui_wrap.cpp
//����:�˻������װ��UI_wrap��ʵ��(C++���԰�)
//����:WXQ#2019,2018

#include <cstdio>//fprintf(),stdout

#include"../include/ui.h"
#include"./ui_console.h"

using namespace std;
namespace module_UI
{
	int UI::prepare(const char* file_name)
	{
		SrcFilePath = file_name;
		return 0;
	}
	UI* UI_wrap::uiObj = NULL;//�˻��������

	void UI_wrap::setUIObject(UI* p_obj)
	{
		uiObj = p_obj;
	}
	UI* UI_wrap ::getUIObject() { return uiObj; }

	void UI_wrap::DrawPixel(
		unsigned long x, unsigned long y,//���ڴ����е�����
		const UI::PixelAttibute& attr)
	{
		if (uiObj == NULL)
			uiObj = new UI_console();

		uiObj->_DrawPixel(x, y, attr);
	}

	//��ʾ�ı�
	void UI_wrap::ShowMessage(int flag,//��������Ϣ=e,��������Ϣ=1
		const char* msg)
	{
		if (uiObj == NULL)
		{
			uiObj = new UI_console();
		}
		uiObj->_ShowMessage(flag, msg);
	}
}//namespace module_UI