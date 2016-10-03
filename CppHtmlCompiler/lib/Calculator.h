#ifndef calculator_h
#define calculator_h
#include <string.h>
#include <cstdio>
#include <iostream>
#include <sstream>
const int MAXN = 200;
class Calc{
private:
	char Exp[MAXN];         //���ʽ
	int NextB[MAXN];        //ƥ������λ��
	double Ans;             //��ֵ���
	void DelandLower(char *str);        //ɾ�����ַ� ת��ΪСд
	bool Check(char *str, int & len);
	bool CheckCh(const char *str, int pos);  //����ַ�
	bool Is_Num(char c);        //�Ƿ�Ϊ����
	bool Operat(char c);        //�Ƿ�Ϊ�����
	bool CheckError(const char *str, int len);
	bool CrectB(const char *str);       //�������ƥ��
	bool Equal(double a, double b);      //�жϸ��������
	int Prio(double x);             //�������ȼ��ж�
	double hash(char c);            //���ŵ�������ӳ��
	double GetV(const char *str, int st, int ed);     //������ֵ
public:
	void Input(const char *str){
		memset(Exp, 0, MAXN);
		memcpy(Exp, str, strlen(str));
	}
	std::string Output(){ std::stringstream ss; ss << Ans; std::string res; ss >> res; return res; }
	bool Cac();
};

#endif