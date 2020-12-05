#pragma once
//文件:errlog.hpp
//内容:错误处理模块之接口声明(C++面向对象版)
//作者:WXQ#2019,2018
/* 该模块包括:
*  报错错误的操作
*  将显示到stdout上的信息写入指定文件
*/

#ifndef _ERRLOG_HPP
#define _ERRLOG_HPP

#include<cstdio>//for FILE 

namespace module_ErrLog{

//将应写入stdout的信息写入下面这个文件中
#define LOGFILE_PATH "parser.log"
//将应写入stderr的信息写入下面这个文件中
#define ERRFILE_PATH "error.log"

	class ErrLog
	{
	public://静态方法便于调用。
		static void error_msg(int line, const char* descrip, const char* string);
		static void error_msg(int line, int col, const char* descrip, const char* string);
		static void error_msg(const char* string_fmt, ...);
		static void logPrint(const char* format, ...);
		static void reOpen(bool open_it = true);
		static int error_count();//错误计数

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
		static ErrLog logger;//唯一的ErrLog对象
	};

}//namespace module_ErrLog

#endif//#ifndef_ERRLOG_HPP