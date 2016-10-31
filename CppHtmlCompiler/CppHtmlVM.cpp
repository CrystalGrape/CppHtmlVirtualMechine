#include "CppHtmlVM.h"
#include "lib\Calculator.h"
#include <sstream>
#include <queue>
#include "json\json.h"
using namespace std;
//运行模式
static RunMode SysMode;	
//指令集
static ExeContainer ExeList;
//虚拟机状态
static VMState	vmState;	
//虚拟机状态处理
static StateMechine stateMechine[4];	
//函数列表<代码区>
static FuncContainer FuncList;	
//当前加载的函数名	
static string LoadFuncName = "_entry";	
//堆栈
static vector<ProgramCounter> SP;	
//变量区 
static map<string, string>  Variable;		
//局部变量区
static vector<map<string, string>>::iterator CurrentStack;
//局部变量堆栈(v2.0)
static vector<map<string, string>> LocalVariableStack;
//溢出标志位
static char OverLoad;		
//当前视图包
static Json::Value ViewBag;		
//程序计数器
static ProgramCounter PC =
{
	"_entry", false, 0
};											
/*初始化命令执行参数*/
CppHtmlVM::CppHtmlVM(RunMode runMode)
{
	SysMode = runMode;
	vmState = Running;
	InitStateMechine();
	if (ExeList.size() == 0){
		InitExeContainer();
	}
}

/*初始化状态机*/
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

/*释放所有资源*/
void CppHtmlVM::FreeResource()
{
	ExeList.clear();
	FuncList.clear();
	LoadFuncName = "_entry";
	SP.clear();
	Variable.clear();
	LocalVariableStack.clear();
	//LocalVariable.clear();
}

/*取出空格和制表符*/
string& CppHtmlVM::trim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	size_t index;
	if ((index = s.find_first_not_of("\t")) != string::npos)
		s.erase(0, index);
	if ((index = s.find_first_not_of(" ")) != string::npos)
		s.erase(0, index);
	if ((index = s.find_last_not_of("\t")) != string::npos)
		s.erase(index + 1);
	if ((index = s.find_last_not_of(" ")) != string::npos)
		s.erase(index + 1);
	return s;
}

/*加载模块文件*/
CHCExpection CppHtmlVM::LoadCHM(string module)
{
	module += ".chmodule";
	ifstream srcfile(module);
	if (!srcfile.is_open()){
		return new CppHtmlCompilerExpection(Failed, "can't open source file");
	}
	char line[8096];
	while (!srcfile.eof()){
		srcfile.getline(line, 8096);
		string tmpLine = line;
		trim(tmpLine);
		LoadCodeLine(tmpLine);
	}
	srcfile.close();
	return new CppHtmlCompilerExpection(Success);
}

/*加载代码*/
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
		if (opcode == "#import"){
			delete LoadCHM(rest);
			return new CppHtmlCompilerExpection(Success);
		}
		scl.SetCode(opcode, rest);
		FuncList[LoadFuncName].push_back(scl);
		return new CppHtmlCompilerExpection(Success);
	}
	string arg1 = rest.substr(last, index - last);
	rest = rest.substr(index + 1, rest.size());
	index = rest.find_first_of(',', last);
	if (opcode == "sysvar"){
		scl.SetCode(opcode, arg1, rest);
		FuncList[LoadFuncName].push_back(scl);
		return new CppHtmlCompilerExpection(Success);
	}
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

/*运行代码*/
CHCExpection CppHtmlVM::Run()
{
	if (FuncList.find("_entry") == FuncList.end())
		return new CppHtmlCompilerExpection(Failed, "can't find entry of script");
	if (SysMode == File){
		PC.function = "_entry";
		PC.line = 0;	
	}
	vmState = Running;
	map<string, string> tmp;
	LocalVariableStack.push_back(tmp);
	CurrentStack = LocalVariableStack.end() - 1;
	while (1){
		CHCExpection e = stateMechine[vmState]();
		if (e->ErrorCode != Success){
			return e;
		}
		delete e;
		//运行代码
		if (PC.line >= (int)FuncList[PC.function].size()){
			return new CppHtmlCompilerExpection(Warning, "error address");
		}
		FuncList[PC.function][PC.line++].Execute(ExeList);
	}
	return new CppHtmlCompilerExpection(Success);
}

/*根据一个算式计算*/
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

/*展开条件*/
void explain(string &arg1, string &arg2, string &arg3)
{
	string mainstr = arg1;
	arg1.clear();
	arg2.clear();
	arg3.clear();
	int index = 0;
	for (int i = 0; i < (int)mainstr.size(); i++)
	{
		if (mainstr.data()[i] == ' ' || mainstr.data()[i] == '\t')
			continue;
		if (mainstr.data()[i] == '<' 
			|| mainstr.data()[i] == '>' 
			|| mainstr.data()[i] == '=')
		{
			index = 1;
			arg2 += mainstr.data()[i];
			continue;
		}
		if (index == 0)
			arg1 += mainstr.data()[i];
		else
			arg3 += mainstr.data()[i];
	}
}

string GetVar(string var)
{
	if (CurrentStack->find(var) != CurrentStack->end())
		return (*CurrentStack)[var];
	else
		return Variable[var];
}

void SetVar(string var, string value)
{
	if (CurrentStack->find(var) != CurrentStack->end())
		(*CurrentStack)[var] = value;
	else
		Variable[var] = value;
}
/*初始化指令集*/
void CppHtmlVM::InitExeContainer()
{
	/*停机命令*/
	ExeList["halt"] = command()
	{
		vmState = Stop;
	};

	/*变量设置命令*/
	ExeList["set"] = command()
	{
		arg1 = arg1.substr(1, arg1.size());
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
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
							expr += GetVar(var);
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
			Calc calc;
			calc.Input(expr.data());
			calc.Cac();
			arg2 = calc.Output();
			//arg2 = calculate(expr);
		}
		Variable[arg1] = arg2;
	};

	/*局部变量声明*/
	ExeList["local"] = command()
	{
		arg1 = arg1.substr(1, arg1.size());
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
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
							expr += GetVar(var);
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
			Calc calc;
			calc.Input(expr.data());
			calc.Cac();
			arg2 = calc.Output();
		}
		(*CurrentStack)[arg1] = arg2;
	};

	/*函数调用命令*/
	ExeList["call"] = command()
	{
		if (arg1.data()[0] == '@'){
			arg1 = GetVar(arg1.substr(1, arg1.size()));
		}
		if (FuncList.find(arg1) == FuncList.end())
			return;
		SP.push_back(PC);
		PC.function = arg1;
		PC.line = 0;
		map<string, string> tmp;
		LocalVariableStack.push_back(tmp);
		CurrentStack = LocalVariableStack.end() - 1;
	};

	/*函数结束命令*/
	ExeList["end"] = command()
	{
		//清除局部变量
		LocalVariableStack.pop_back();
		CurrentStack = LocalVariableStack.end() - 1;
		PC = SP.back();
		SP.pop_back();
	};

	/*循环标志命令*/
	ExeList["looper"] = command()
	{
		PC.line -= 1;
		PC.isLooper = true;
		SP.push_back(PC);
		PC.isLooper = false;
		PC.line += 1;
	};

	/*返回跳转命令*/
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
	/*比较命令*/
	ExeList["cmp"] = command()
	{
		if (arg1.data()[0] == '@'){
			arg1 = GetVar(arg1.substr(1, arg1.size()));
		}
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
		}

		//验证是数字还是字符串
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
	/*输出命令*/
	ExeList["echo"] = command()
	{
		if (arg1.data()[0] == '@'){
			arg1 = GetVar(arg1.substr(1, arg1.size()));
		}
		if (arg1.data()[0] == '"'&&arg1.data()[arg1.size() - 1] == '"'){
			arg1 = arg1.substr(1, arg1.size() - 2);
			string expr;
			for (int i = 0; i < (int)arg1.size(); i++){
				if (arg1.data()[i] == '@'&&arg1.data()[i + 1] == '{'){
					string var;
					for (int j = i + 2;; j++){
						if (arg1.data()[j] == '}' || j == arg2.size()){
							string res = GetVar(var);
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
	/*删除变量*/
	ExeList["free"] = command()
	{
		if (arg1.data()[0] != '@')
			return;
		arg1 = arg1.substr(1, arg1.size());
		if (CurrentStack->find(arg1) != CurrentStack->end())
		{
			CurrentStack->erase(arg1);
		}
		if (Variable.find(arg1) != Variable.end())
			Variable.erase(arg1);
	};
	/*系统变量设置*/
	ExeList["sysvar"] = command(){
		arg1 = arg1.substr(1, arg1.size());
		Variable[arg1] = arg2;
	};
	/*设置当前视图*/
	ExeList["setview"] = command()
	{
		Json::Reader reader;
		if (arg1.data()[0] == '@'){
			arg1 = GetVar(arg1.substr(1, arg1.size()));
		}
		reader.parse(arg1, ViewBag);
	};
	/*读取当前视图*/
	ExeList["readview"] = command()
	{
		arg1 = arg1.substr(1, arg1.size());
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
		}
		if (arg3.data()[0] == '@'){
			arg2 = GetVar(arg3.substr(1, arg3.size()));
		}
		else{
			arg3 = arg3.substr(1, arg3.size() - 2);
		}

		int index = atoi(arg2.data());
		SetVar(arg1, ViewBag[index][arg3].asString());
	};

	/*for循环*/
	ExeList["for"] = command()
	{
		string var = arg1;
		string step = arg3;
		arg1 = GetVar(arg1.substr(1, arg1.size()));
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
		}
		if (arg3.data()[0] == '@'){
			arg3 = GetVar(arg3.substr(1, arg3.size()));
		}
		double nu1 = atof(arg1.data());
		double nu2 = atof(arg2.data());
		double nu3 = atof(arg3.data());
		bool ret = (nu3 >= 0) ? (nu1 >= nu2) : (nu1 <= nu2);
		if (!ret){
			//继续循环
			PC.isLooper = true;
			PC.line -= 1;
			PC.var = var;
			PC.step = step;
			SP.push_back(PC);
			PC.var.clear();
			PC.step.clear();
			PC.isLooper = false;
			PC.line += 1;
		}
		else{
			//跳出循环，寻找下面的rof关键字
			int state = 0;
			for (int index = PC.line;; index++){
				if (FuncList[PC.function][index].IsFor())
					state++;
				if (FuncList[PC.function][index].IsRof()){
					if (state != 0){
						state--;
						continue;
					}
					PC.line = index + 1;
					break;
				}
			}
		}
	};
	/*for结束*/
	ExeList["rof"] = command()
	{
		PC = SP.back();
		SP.pop_back();
		arg1 = GetVar(PC.var.substr(1, PC.var.size()));
		arg2 = PC.step;
		if (arg2.data()[0] == '@'){
			arg2 = GetVar(arg2.substr(1, arg2.size()));
		}
		double nu1 = atof(arg1.data());
		double nu2 = atof(arg2.data());
		stringstream ss;
		string res;
		ss << (nu1 + nu2);
		ss >> res;
		SetVar(PC.var.substr(1, PC.var.size()), res);
		PC.step.clear();
		PC.var.clear();
	};
	/*跳出循环*/
	ExeList["break"] = command()
	{
		for (int index = PC.line;; index++){
			if (FuncList[PC.function][index].IsRof()){
				PC.line = index + 1;
				break;
			}
		}
	};
	/*继续循环*/
	ExeList["continue"] = command()
	{
		ExeList["rof"]("0", "0", "0");
	};
	/*条件判断*/
	ExeList["if"] = command()
	{
		explain(arg1, arg2, arg3);
		if (arg1.data()[0] == '@'){
			arg1 = GetVar(arg1.substr(1, arg1.size()));
		}
		if (arg3.data()[0] == '@'){
			arg3 = GetVar(arg3.substr(1, arg3.size()));
		}

		ExeList["cmp"](arg1, arg3, "0");
		bool ret = false;
		if (OverLoad == -1 && (arg2 == "<" || arg2 == "<=")){
			ret = true;
		}
		if (OverLoad == 0 && (arg2 == "==" || arg2 == "<=" || arg2 == ">=")){
			ret = true;
		}
		if (OverLoad == 1 && (arg2 == ">" || arg2 == ">=")){
			ret = true;
		}
		if (!ret){
			//如果条件不满足，执行else后的语句
			int state = 0;
			for (int index = PC.line;; index++){
				if (FuncList[PC.function][index].IsIf())
					state++;
				if (FuncList[PC.function][index].IsElse() && state != 0)
					continue;
				if (FuncList[PC.function][index].IsElse() && state == 0)
				{
					PC.line = index + 1;
					break;
				}
				if (FuncList[PC.function][index].IsFi())
				{
					if (state != 0){
						state--;
						continue;
					}
					PC.line = index + 1;
					break;
				}
			}
		}
	};
	/*else语句*/
	ExeList["else"] = command()
	{
		for (int index = PC.line;; index++){
			if (FuncList[PC.function][index].IsFi()){
				PC.line = index + 1;
				break;
			}
		}
	};
}