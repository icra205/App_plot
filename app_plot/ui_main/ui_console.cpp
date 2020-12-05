//�ļ���ui_console.cpp
//���ݣ����ڿ���̨�Ľ���ʵ�֣�C++���԰棩
//���ߣ�WXQ#2019��2018
/*���Ĵ������C/C++�Ŀ⺯��ʵ��*/

#include<cstdio>//fprintf������stdout
#include<string>
#include<iostream>

#include"ui_console.h"//interface of this module
#include"../include/Errlog.h"

using namespace std;
namespace module_UI
{
	void UI_console::_DrawPixel(unsigned long x, unsigned long y, const PixelAttibute &attr)
	{
		fprintf(stdout, "......DrawPixel��%1d��%1d��with color"
			"[%02X��%02X��%02X] and size %d\n",
			x, y, attr.red, attr.green, attr.blue, attr.size);
	}

	void UI_console::_ShowMessage(int flag, const char* msg)
	{
		if (flag == 0)
			fprintf(stdout, msg);
		else
			fprintf(stderr, msg);
	}

	void UI_console::run()
	{
		std::string msg("\n����ִ�в��������ļ���\n");
		msg += LOGFILE_PATH;
		msg += "�洢�﷨������������ı�\n";
		msg += ERRFILE_PATH;
		msg += "�洢����Ĵ�����Ϣ\n\n";

		_ShowMessage(0, msg.c_str());
		return;
	}
	std::string UI_console::selectFile()
	{
		cout << "��������������ļ�·�������»س�����" << endl;
		string yourFile;
		getline(cin, yourFile);
		return yourFile;
	}
}