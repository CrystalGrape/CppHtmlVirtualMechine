#pragma once
/*
*	异常返回类型
*/
#include <iostream>
#include <fstream>
namespace std
{
	typedef enum
	{
		Success,
		Warning,
		Failed
	}RetState;
	typedef class CppHtmlCompilerExpection
	{
	public:
		CppHtmlCompilerExpection(RetState ErrCode, string Explain = " ", string Trace = " ")
		{
			this->ErrorCode = ErrCode;
			this->Explain = Explain;
			this->Trace = Trace;
		}
		friend ostream &operator<<(ostream &os, CppHtmlCompilerExpection &c)
		{
			os << "ErrorCode:" << c.ErrorCode << endl;
			os << "Explain:" << c.Explain.data() << endl;
			os << "Trace:" << c.Trace.data() << endl;
			return os;
		}
		RetState ErrorCode;
	private:
		string Trace;
		string Explain;
	}*CHCExpection;
}
