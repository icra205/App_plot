#include <cstdio>//fprintf(),stdout
#include<cstring>

#include"interpreter.h"

#pragma warning(disable:4996)

using namespace std;

FDL_interpreter::UI_Kind
judgeUIKind(const char* str)
{
	if (strcmp(str, "con") == 0)
		return FDL_interpreter::USE_UI_CONSOLE;
	else if (strcmp(str, "win") == 0)
		return FDL_interpreter::USE_UI_WINDOW;
	else if (strcmp(str, "conwin") == 0)
		return FDL_interpreter::USE_UI_WIN_CONSOLE;
	else if (strcmp(str, "wincon") == 0)
		return FDL_interpreter::USE_UI_WIN_CONSOLE;
	else
		return FDL_interpreter::USE_UI_UNKNOWN;
}

int parsecommandLineArgument(int argc, char* argv[], FDL_interpreter::UI_Kind& uk,
	const char** p2pFilePath)
{
	switch (argc)
	{
	case 1://没有其他参数
		*p2pFilePath = "";
		uk = FDL_interpreter::USE_UI_DEFAULT;
		return 0;

	case 2:
		uk = judgeUIKind(argv[1]);
		//判断参数是否为界面种类标识.
		//若不是则它就是文件路径
		if (uk == FDL_interpreter::USE_UI_UNKNOWN)
		{
			*p2pFilePath = argv[1];
			uk = FDL_interpreter::USE_UI_DEFAULT;
		}
		else
		{
			*p2pFilePath = "";
		}
		return 0;
	case 3:
	default:
#ifdef WIN32
		if (strcmp(argv[1], "con") == 0)
		{
			printf("windows环境中不能用con来命名文件.\n");
			return 2;
		}
#endif
		* p2pFilePath = argv[1];
		uk = judgeUIKind(argv[2]);
		if (uk == FDL_interpreter::USE_UI_UNKNOWN)
		{
			printf("Invalidate argument:%s\n", argv[2]);
			return 2;
		}
		return 0;
	}
}

int main(int argc, char* argv[])
{
	const char* pFilePath = "";
	FDL_interpreter::UI_Kind uk = FDL_interpreter::USE_UI_DEFAULT;

	if (0 != parsecommandLineArgument(argc, argv, uk, &pFilePath))
	{
		return 2;
	}


	FDL_interpreter* pInterpreter = FDL_interpreter::getInstance();
	return pInterpreter->run(pFilePath, uk, NULL);
}