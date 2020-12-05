#pragma once
//文件:dfa.hpp
//内容:声明DFA的抽象类及其接口
//作者:WXQ#2019,2018

#ifndef _LEXER_DFA_HPP
#define _LEXER_DFA_HPP

#include "../include/Lexer.h"//use Token_type 

namespace module_Lexer 
{
	namespace DFA//词法分析器内部的子模块
	{
	//定义DFA的操作接口(abstract class).
	//该接口与DFA的实现无关,即真正的DFA是表驱动的、还是直接编码的无关！
	//当你定义具体DFA时,要求所有的状态用非负整数编码！

		class dfa 
		{
		public:
			//获取DFA初态(>=0)
			virtual  int get_start_state()const = 0;
			//查询转移并返回下一状态。
			//返回在状态state_src下,遇到字符ch的下一状态.
			//若返回的状态小于8,表示无转移.
			virtual int move(int state_src, char ch)const = 0;
			//判断指定状态state是否为DFA的终态。
			//若是则返回该终态对应的记号类别,否则返回 ERRTOKEN.
			virtual Token_Type state_is_final(int state)const = 0;

			virtual ~dfa(){};
		public:
			//获得缺省的‘表驱动型’的DFA对象.
			//！！该DFA对象不允许调用者delete！！
			static dfa* get_table_driven_dfa();

			//获得‘直接编码型’的DFA对象.
			//！！该DFA对象不允许调用者delete！！
			static dfa* get_hardCode_dfa();
		};
	}//end of namespace DFA
}//namespace module_Lexer
#endif//#ifndef LEXER_DFA_HPP