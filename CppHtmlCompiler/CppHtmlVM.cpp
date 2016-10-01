#include "CppHtmlVM.h"
#include <sstream>
#include <regex>
using namespace std;
static RunMode SysMode;						//����ģʽ
static ExeContainer ExeList;				//ָ�
static VMState	vmState;					//�����״̬
static StateMechine stateMechine[4];		//�����״̬����
static CHCExpection ChcLastError;			//��������һ������
static FuncContainer FuncList;				//�����б�<������>
static string LoadFuncName = "_entry";		//��ǰ���صĺ�����	
static struct ProgramCounter
{
	string function;
	bool isLooper;
	int line;
}PC = { "_entry", false, 0 };				//���������
static vector<ProgramCounter> SP;			//��ջ
static map<string, string>  Variable;		//������ 
static char OverLoad;						//�����־λ
/*��ʼ������ִ�в���*/
CppHtmlVM::CppHtmlVM(RunMode runMode)
{
	SysMode = runMode;
	vmState = Running;
	InitStateMechine();
	InitExeContainer();
}
void CppHtmlVM::InitStateMechine()
{
	stateMechine[Load] = [](){
		return new CppHtmlCompilerExpection(Failed, "chc is loading code,cant't execute");
	};

	stateMechine[Running] = [](){
		return new CppHtmlCompilerExpection(Success);
	};

	stateMechine[Stop] = [](){
		return new CppHtmlCompilerExpection(Warning, "chc is stop");
	};

	stateMechine[Dump] = [](){
		return new CppHtmlCompilerExpection(Warning, "chc is dump");
	};
}

/*���ش���*/
CHCExpection CppHtmlVM::LoadCodeLine(std::string line)
{
	if (line.size() == 0)
		return new CppHtmlCompilerExpection(Warning, "there is a empty line");
	size_t last = 0;
	size_t index = line.find_first_of(' ', last);
	vmState = Load;
	ISourceCodeLine scl;
	if (index == string::npos){
		scl.SetCode(line);
		FuncList[LoadFuncName].push_back(scl);
		if (line == "end"){
			LoadFuncName = "_entry";
		}
		return new CppHtmlCompilerExpection(Success);
	}
	string opcode = line.substr(last, index - last);
	string rest = line.substr(index + 1, line.size());
	index = rest.find_first_of(',', last);
	if (index == string::npos){
		if (opcode == "function"){
			LoadFuncName = rest.substr(0, rest.size() - 1);
			return new CppHtmlCompilerExpection(Success);
		}
		scl.SetCode(opcode, rest);
		FuncList[LoadFuncName].push_back(scl);
		return new CppHtmlCompilerExpection(Success);
	}
	string arg1 = rest.substr(last, index - last);
	rest = rest.substr(index + 1, rest.size());
	index = rest.find_first_of(',', last);
	if (index == string::npos){
		scl.SetCode(opcode, arg1, rest);
		FuncList[LoadFuncName].push_back(scl);
		return new CppHtmlCompilerExpection(Success);
	}
	string arg2 = rest.substr(last, index - last);
	rest = rest.substr(index + 1, rest.size());
	scl.SetCode(opcode, arg1, arg2, rest);
	FuncList[LoadFuncName].push_back(scl);
	return new CppHtmlCompilerExpection(Success);
}

/*���д���*/
CHCExpection CppHtmlVM::Run()
{
	if (FuncList.find("_entry") == FuncList.end())
		return new CppHtmlCompilerExpection(Failed, "can't find entry of script");
	if (SysMode == File){
		PC.function = "_entry";
		PC.line = 0;
	}
	vmState = Running;
	while (1){
		CHCExpection e = stateMechine[vmState]();
		if (e->ErrorCode != Success){
			return e;
		}
		delete e;
		//���д���
		if (PC.line >= (int)FuncList[PC.function].size()){
			return new CppHtmlCompilerExpection(Warning, "error address");
		}
		FuncList[PC.function][PC.line++].Execute(ExeList);
	}
	return new CppHtmlCompilerExpection(Success);
}

string calculate(string code)
{
	string ret;
	stringstream ss;
	size_t last = 0;
	size_t index = code.find_first_of('+', last);
	if (index != string::npos){
		string arg1 = code.substr(0, index);
		string arg2 = code.substr(index + 1, code.size());
		if (arg1.find_first_of('.') != string::npos || arg2.find_first_of('.') != string::npos){
			double result = atof(arg1.data()) + atof(arg2.data());
			ss << result;
		}
		else
		{
			int result = (atoi(arg1.data()) + atoi(arg2.data()));
			ss << result;
		}
		ss >> ret;
		return ret;
	}
	index = code.find_first_of('-', last);
	if (index != string::npos){
		string arg1 = code.substr(0, index);
		string arg2 = code.substr(index + 1, code.size());
		if (arg1.find_first_of('.') != string::npos || arg2.find_first_of('.') != string::npos){
			double result = atof(arg1.data()) - atof(arg2.data());
			ss << result;
		}
		else
		{
			int result = (atoi(arg1.data()) - atoi(arg2.data()));
			ss << result;
		}
		ss >> ret;
		return ret;
	}
	index = code.find_first_of('*', last);
	if (index != string::npos){
		string arg1 = code.substr(0, index);
		string arg2 = code.substr(index + 1, code.size());
		if (arg1.find_first_of('.') != string::npos || arg2.find_first_of('.') != string::npos){
			double result = atof(arg1.data()) * atof(arg2.data());
			ss << result;
		}
		else
		{
			int result = (atoi(arg1.data()) * atoi(arg2.data()));
			ss << result;
		}
		ss >> ret;
		return ret;
	}
	index = code.find_first_of('/', last);
	if (index != string::npos){
		string arg1 = code.substr(0, index);
		string arg2 = code.substr(index + 1, code.size());
		if (arg1.find_first_of('.') != string::npos || arg2.find_first_of('.') != string::npos){
			double result = atof(arg1.data()) / atof(arg2.data());
			ss << result;
		}
		else
		{
			int result = (atoi(arg1.data()) / atoi(arg2.data()));
			ss << result;
		}
		ss >> ret;
		return ret;
	}
	index = code.find_first_of('%', last);
	if (index != string::npos){
		string arg1 = code.substr(0, index);
		string arg2 = code.substr(index + 1, code.size());
		if (arg1.find_first_of('.') != string::npos || arg2.find_first_of('.') != string::npos){
			return "error:operator side of % must be interger";
		}
		else{
			int result = (atoi(arg1.data()) % atoi(arg2.data()));
			ss << result;
		}
		
		ss >> ret;
		return ret;
	}
	return ret;
}

void CppHtmlVM::InitExeContainer()
{
	/*ͣ������*/
	ExeList["halt"] = command()
	{
		vmState = Stop;
	};

	/*������������*/
	ExeList["set"] = command()
	{
		arg1 = arg1.substr(1, arg1.size());
		if (arg2.data()[0] == '@'){
			arg2 = Variable[arg2.substr(1, arg2.size())];
		}
		if (arg2.data()[0] == 'e'
			&&arg2.data()[1] == 'x'
			&&arg2.data()[2] == 'p'
			&&arg2.data()[3] == 'r'
			&&arg2.data()[4] == ':')
		{
			arg2 = arg2.substr(5, arg2.size());
			string expr;
			for (int i = 0; i < (int)arg2.size(); i++){
				if (arg2.data()[i] == '@'){
					string var;
					for (int j = i + 1;; j++){
						if (arg2.data()[j] == '+'
							|| arg2.data()[j] == '-'
							|| arg2.data()[j] == '*'
							|| arg2.data()[j] == '/'
							|| arg2.data()[j] == '%' || j == arg2.size()){
							expr += Variable[var];
							expr += arg2.data()[j];
							i = j;
							break;
						}
						var += arg2.data()[j];
					}
				}
				else
					expr += arg2[i];
			}
			arg2 = calculate(expr);
		}
		Variable[arg1] = arg2;
	};

	/*������������*/
	ExeList["call"] = command()
	{
		if (FuncList.find(arg1) == FuncList.end())
			return;
		SP.push_back(PC);
		PC.function = arg1;
		PC.line = 0;
	};

	/*������������*/
	ExeList["end"] = command()
	{
		PC = SP.back();
		SP.pop_back();
	};

	/*ѭ����־����*/
	ExeList["looper"] = command()
	{
		PC.line -= 1;
		PC.isLooper = true;
		SP.push_back(PC);
		PC.isLooper = false;
		PC.line += 1;
	};

	/*������ת����*/
	ExeList["back"] = command()
	{
		string next;
		switch (OverLoad)
		{
		case 1:
			next = arg1;
			break;
		case 0:
			next = arg2;
			break;
		case -1:
			next = arg3;
			break;
		default:
			return;
		}
		if (next == "looper"){
			PC = SP.back();
			SP.pop_back();
		}
		else if (next == "next"){
			if (SP.size() != 0 && SP.back().isLooper)
				SP.pop_back();
		}
		else{
			SP.push_back(PC);
			PC.function = next;
			PC.line = 0;
		}
	};
	/*�Ƚ�����*/
	ExeList["cmp"] = command()
	{
		if (arg1.data()[0] == '@'){
			arg1 = Variable[arg1.substr(1, arg1.size())];
		}
		if (arg2.data()[0] == '@'){
			arg2 = Variable[arg2.substr(1, arg2.size())];
		}

		//��֤�����ֻ����ַ���
		if (arg2.data()[0] == '\"'&&arg2.data()[arg2.size() - 1] == '\"'
			||arg1.data()[0] == '\"'&&arg1.data()[arg1.size() - 1]){
			arg1 = arg2.substr(1, arg2.size() - 2);
			arg2 = arg2.substr(1, arg2.size() - 2);
			if (arg1 > arg2)
				OverLoad = 1;
			if (arg1 == arg2)
				OverLoad = 0;
			if (arg1 < arg2)
				OverLoad = -1;
		}
		else
		{
			double nu1 = atof(arg1.data());
			double nu2 = atof(arg2.data());
			if (nu1 > nu2)
				OverLoad = 1;
			if (nu1 == nu2)
				OverLoad = 0;
			if (nu1 < nu2)
				OverLoad = -1;
		}
	};
	/*�������*/
	ExeList["echo"] = command()
	{
		if (arg1.data()[0] == '@'){
			arg1 = Variable[arg1.substr(1, arg1.size())];
		}
		if (arg1.data()[0] == '"'&&arg1.data()[arg1.size() - 1] == '"'){
			arg1 = arg1.substr(1, arg1.size() - 2);
			string expr;
			for (int i = 0; i < (int)arg1.size(); i++){
				if (arg1.data()[i] == '@'&&arg1.data()[i + 1] == '{'){
					string var;
					for (int j = i + 2;; j++){
						if (arg1.data()[j] == '}' || j == arg2.size()){
							string res = Variable[var];
							if (res.data()[0] == '\"'&&res.data()[res.size() - 1] == '\"')
							{
								res = res.substr(1, res.size() - 2);
							}
							expr += res;
							i = j;
							break;
						}
						var += arg1.data()[j];
					}
				}
				else
					expr += arg1[i];
			}
			arg1 = expr;
		}
		cout << arg1.data() << endl;
	};
	/*ɾ������*/
	ExeList["free"] = command()
	{
		if (arg1.data()[0] != '@')
			return;
		arg1 = arg1.substr(1, arg1.size());
		if (Variable.find(arg1) == Variable.end())
			return;
		Variable.erase(arg1);
	};
}