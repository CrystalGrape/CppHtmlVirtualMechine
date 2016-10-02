#pragma once
#include "ISourceCodeLine.h"
#include "CppHtmlExpection.h"
#define command() [](std::string arg1,std::string arg2,std::string arg3)
typedef enum
{
	Load = 0,		//����װ����
	Running,		//��������
	Stop,			//ֹͣ����
	Dump,			//���ִ������
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
	std::CHCExpection LoadCHM(std::string module);
	std::CHCExpection LoadCodeLine(std::string line);
	std::CHCExpection Run();
	static std::string& trim(std::string &s);
	void ClearCommand();
};

