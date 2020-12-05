#pragma once
//�ļ�:errlog.hpp
//����:������ģ��֮�ӿ�����(C++��������)
//����:WXQ#2019,2018
/* ��ģ�����:
*  �������Ĳ���
*  ����ʾ��stdout�ϵ���Ϣд��ָ���ļ�
*/

#ifndef _ERRLOG_HPP
#define _ERRLOG_HPP

#include<cstdio>//for FILE 

namespace module_ErrLog{

//��Ӧд��stdout����Ϣд����������ļ���
#define LOGFILE_PATH "parser.log"
//��Ӧд��stderr����Ϣд����������ļ���
#define ERRFILE_PATH "error.log"

	class ErrLog
	{
	public://��̬�������ڵ��á�
		static void error_msg(int line, const char* descrip, const char* string);
		static void error_msg(int line, int col, const char* descrip, const char* string);
		static void error_msg(const char* string_fmt, ...);
		static void logPrint(const char* format, ...);
		static void reOpen(bool open_it = true);
		static int error_count();//�������

	public:
		void open();
		void close();

	private:
		ErrLog();
		~ErrLog();

	private:
		int mcount;
		std::FILE* fpLog;
		std::FILE* fpErr;
		static ErrLog logger;//Ψһ��ErrLog����
	};

}//namespace module_ErrLog

#endif//#ifndef_ERRLOG_HPP