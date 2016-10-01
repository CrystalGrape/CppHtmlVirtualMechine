#pragma once
#include "ISourceCodeLine.h"
#include "CppHtmlExpection.h"
#define command() [](std::string arg1,std::string arg2,std::string arg3)
typedef enum
{
	Load = 0,		//代码装载中
	Running,		//正在运行
	Stop,			//停止运行
	Dump,			//出现错误崩溃
}VMState;

typedef enum
{
	Cmd,
	File,
}RunMode;
typedef std::CHCExpection (*StateMechine)();
class CppHtmlVM
{
public:
	CppHtmlVM(RunMode runMode = File);
	void InitStateMechine();
	void InitExeContainer();
	std::CHCExpection LoadCodeLine(std::string line);
	std::CHCExpection Run();
	void ClearCommand();
};

