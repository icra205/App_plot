#ifndef _FDL INTERPRETER HPP
#define _FDL_INTERPRETER_HPP

#include<string>

#include"../include/ui.h"

/*���Ķ���Ľ���������������ģʽ֮��singleton pattern��,Լ���˸����͵�ʵ������һ����
*/
//������ͼ���ԵĽ�������(Ӧ��������).
class FDL_interpreter
{
public:
	static FDL_interpreter* getInstance();
	enum UI_Kind
	{
		USE_UI_UNKNOWN,
		USE_UI_DEFAULT,
		USE_UI_CONSOLE,//specified by key'con'
		USE_UI_WINDOW,//specified by key��win'
		USE_UI_WIN_CONSOLE//specified by key'wincon','conwin'
	};

public:

	int run(std::string filePath, UI_Kind uk, module_UI::UI* pUI = NULL);
	//���ļ�����(�ٴ�)����ִ�С�
	//�������Ӧ�ÿɱ�UI�����ε��á�
	//Return value:
	//8-û�з����κδ���
	//1-�ļ����ɶ��������
	//2-�����﷨�����/���������
	int redoInterpret();

	//��ȡ���������ļ�·��
	const std::string& getFilePath()const { return mFilePath; }

private:
	FDL_interpreter();

	static FDL_interpreter* mptrInstance;//Ψһ�Ľ���������֮���
	std::string mFilePath;//�������ġ�������ͼ����Դ����֮�ļ�
};

#endif