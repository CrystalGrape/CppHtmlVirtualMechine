#include "CppHtmlExpection.h"
#include "CppHtmlVM.h"
using namespace std;
CHCExpection CommandMode();
CHCExpection CompilerCH(string srcName);
int main(int argc,char **argv)
{
	if (argc == 1){
		//命令行模式
		CommandMode();
	}
	else
	{
		//编译文件
		string FileName = argv[1];
		CompilerCH(FileName);
	}
	return 0;
}

string& trim(string &s)
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

CHCExpection CompilerCH(string srcName)
{
	
	ifstream srcfile(srcName);
	if (!srcfile.is_open()){
		return new CppHtmlCompilerExpection(Failed, "can't open source file");
	}
	CppHtmlVM chv;
	char line[1000];
	bool isCpp = false;
	ofstream cppfile;
	while (!srcfile.eof()){
		srcfile.getline(line, 1000);
		string tmpLine = line;
		if (isCpp){
			if (tmpLine.find("?>") != string::npos){
				delete chv.Run();
				isCpp = false;
			}
			else{
				trim(tmpLine);
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
		trim(cmd);
		delete chv.LoadCodeLine(cmd);
		delete chv.Run();
	}
	return new CppHtmlCompilerExpection(Success);
}