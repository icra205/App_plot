//�ļ�:lexer_main.cpp
//����:�ʷ�����������������(C++���԰�)
//����:WXQ#2019,2018

// #define TEST_LEXER
#ifdef TEST_LEXER//��������ʱ�����������,�Ž��дʷ�����
#include<iostream>
#include<iomanip>
#include"../include/Lexer.h"

using namespace std;
using namespace module_Lexer;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "please specify the Source File��\n";
		return 1;
	}

	Lexer lexer;
	if (0 == lexer.InitScanner(argv[1]))//��ʼ���ʷ�������
	{
		cerr << "Open Source File Error��\n";
		return 2;
	}
	cout << "�Ǻ����     �ַ���            ����ֵ      ����ָ��       λ��\n";
	cout << "_________________________________________________________________________\n";

	Token token;
	while (1)
	{
		token = lexer.GetToken();//��üǺ�
		if (token.getType() != NONTOKEN)//��ӡ�Ǻŵ�����
		{
			//"%-10s%125%12f%12p(%d,%d)\n",
			cout << setiosflags(ios::left) << setw(12)
				<< token_type_str(token.getType()) << " "
				<< setw(12) << token.getText() << " "
				<< resetiosflags(ios::left);
			cout << setw(12) << token.getValue() << " "
				<< setw(12) << (void*)(token.getFuncPtr()) << "("
				<< token.getPosition().line() << ","
				<< token.getPosition().column() << ")\n";
		}
		else break;
	};
	cout << "_________________________________________________________________________\n";
	//��������ʱ:
	//~1exer()�����Զ�����
	return 0;
}
#endif//#ifdef TEST_LEXER