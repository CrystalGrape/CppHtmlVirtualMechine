/*
*	源代码行类
*/
#pragma once
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <fstream>
typedef void (*CExecute)(std::string arg1, std::string arg2, std::string arg3);
typedef std::map<std::string, CExecute> ExeContainer;
class ISourceCodeLine
{
public:
	void SetCode(std::string opcode
		, std::string arg1 = "0"
		, std::string arg2 = "0"
		, std::string arg3 = "0")
	{
		this->opcode = opcode;
		this->arg1 = arg1;
		this->arg2 = arg2;
		this->arg3 = arg3;
	}
	void Execute(ExeContainer con)
	{
		if (con.find(opcode) == con.end())
			return;
		//std::cout<<">>>"<<opcode<<" "<<arg1<<","<<arg2<<","<<arg3<<std::endl;
		con[opcode](arg1, arg2, arg3);
	}
	bool IsFor()
	{
		return (opcode == "for") ? true : false;
	}
	bool IsRof()
	{
		return (opcode == "rof") ? true : false;
	}
	bool IsIf()
	{
		return (opcode == "if") ? true : false;
	}
	bool IsElse()
	{
		return (opcode == "else") ? true : false;
	}
	bool IsFi()
	{
		return (opcode == "fi") ? true : false;
	}
private:
	std::string opcode;
	std::string arg1;
	std::string arg2;
	std::string arg3;
}; 
typedef std::vector<ISourceCodeLine> CodeContainer;
typedef std::map<std::string, CodeContainer> FuncContainer;


