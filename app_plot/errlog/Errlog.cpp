//�ļ�:errlog.cpp
//����:������ģ��(C++��������)
//����:WXQ#2019,2018
/* ��ģ�����:
* �������Ĳ���
* ����ʾ��stdout�ϵ���Ϣд��ָ���ļ�
*/

#include<cstdio>
#include<cstdlib>//exit()
#include<cstdarg>//for va_*

#include"../include/Errlog.h"
#include"../include/ui.h"

using namespace std; 
using namespace module_UI;
namespace module_ErrLog {
	ErrLog ErrLog::logger; //��������ľ�̬����,�Զ����졢�Զ�����
	ErrLog::ErrLog() :mcount(0), fpLog(NULL), fpErr(NULL) {
		//open();
	}

	ErrLog::~ErrLog()
	{
		close();
	}

	void ErrLog::open()
	{
		mcount = 0;
		errno_t er;
		er = fopen_s(&fpLog, LOGFILE_PATH, "w");
		if (er)
			printf("Error open errlog");

		er = fopen_s(&fpErr, ERRFILE_PATH, "w");
		if (er)
			printf("Error open errlogerr");
	}

	void ErrLog::close()
	{
		if (NULL != fpLog)
			fclose(fpLog);
		fpLog = NULL;

		if (NULL != fpErr)
			fclose(fpErr);
		fpErr = NULL;
	}

	//
	//�����Ǿ�̬�ķ���
	//

	void ErrLog::reOpen(bool open_it)
	{
		logger.close();
		if (open_it)
			logger.open();
	}

	int ErrLog::error_count()
	{
		return logger.mcount;
	}

	void ErrLog::error_msg(const char* string_fmt, ...)
	{
		++logger.mcount;

		char msg[2048];
		va_list args;
		va_start(args, string_fmt);
		vsnprintf(msg, sizeof(msg), string_fmt, args);
		va_end(args);

		FILE* fp = (logger.fpErr == NULL) ? stderr : logger.fpErr;
		if (fp != stderr)
			fprintf(fp, msg);//������stderr���ظ�����

		UI_wrap::ShowMessage(1, msg);
	}

	void ErrLog::error_msg(int line, const char* descrip, const char* string)
	{
		error_msg("Error in line %d:%s %s\n", line, descrip, string);
	}

	void ErrLog::error_msg(int line, int col, const char* descrip, const char* string)
	{
		error_msg("Error at[%d,%d]:%s %s\n", line, col, descrip, string);
	}

	void ErrLog::logPrint(const char* format, ...)
	{
		if (logger.fpLog == NULL)
			logger.open();

		va_list args; FILE* fp = (logger.fpLog == NULL) ? stdout : logger.fpLog;

		va_start(args, format);
		vfprintf(fp, format, args);
		va_end(args);
	}

}//namespace module_ErrLog