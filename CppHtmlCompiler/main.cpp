#include "CppHtmlExpection.h"
#include "CppHtmlVM.h"
using namespace std;
CHCExpection CommandMode();
CHCExpection CompilerCH(string srcName);
int main(int argc,char **argv)
{
	//if (argc == 1){
	//	//命令行模式
	//	CommandMode();
	//}
	//else{
	//	//编译文件
	//	string FileName = argv[1];
	//	CompilerCH(FileName);
	//}
	string FileName = "demo/index.cpphtml";
	CompilerCH(FileName);
	return 0;
}

CHCExpection CompilerCH(string srcName)
{
	
	ifstream srcfile(srcName);
	if (!srcfile.is_open()){
		return new CppHtmlCompilerExpection(Failed, "can't open source file");
	}
	CppHtmlVM chv;
	chv.LoadCHM("sysvar");
	char line[8096];
	bool isCpp = false;
	while (!srcfile.eof()){
		srcfile.getline(line, 8096);
		string tmpLine = line;
		if (isCpp){
			if (tmpLine.find("?>") != string::npos){
				delete chv.Run();
				isCpp = false;
			}
			else{
				CppHtmlVM::trim(tmpLine);
				delete chv.LoadCodeLine(tmpLine);
			}
		}
		else if (tmpLine.find("<?cpp") != string::npos){
			isCpp = true;
		}
		else{
			cout << tmpLine.data() << endl;
		}
	}
	srcfile.close();
	chv.FreeResource();
	return new CppHtmlCompilerExpection(Success);
}

CHCExpection CommandMode()
{
	char ch;
	CppHtmlVM chv(Cmd);
	while (1){
		cout << "chvm#:";
		string cmd;
		while ((ch = getchar())!='\n')
			cmd += ch;
		CppHtmlVM::trim(cmd);
		delete chv.LoadCodeLine(cmd);
		delete chv.Run();
	}
	return new CppHtmlCompilerExpection(Success);
}