#include "Calculator.h"
#include <stack>
#include <cmath>
#include <cstring>
#include <iostream>

const double inf = 1e11;
const double eps = 1e-6; //eps 调整精度
const int MAXFUN = 3;

#define HASHA (-inf+1)
#define HASHS (-inf+2)
#define HASHM (-inf+3)
#define HASHD (-inf+4)
#define HASHL (-inf+5)
#define ERRORX (-inf+6)

using namespace std;

static char MathFun[][4] = { "sin", "cos", "tan" };

double Calc::hash(char c){
	switch (c){
	case '+':return HASHA;
	case '-':return HASHS;
	case '*':return HASHM;
	case '/':return HASHD;
	default:return HASHL;
	}
}

int Calc::Prio(double x){
	if (x<-inf + 3 - eps)  //代表加法和减法
		return 1;
	if (x<-inf + 5 - eps) //乘法和除法
		return 2;
	return 3;
}

void Calc::DelandLower(char *str){
	int i, j;
	for (i = j = 0; *(str + i); i++){
		if (*(str + i) == ' ' || *(str + i) == '\t')
			continue;
		if (*(str + i) >= 'A' && *(str + i) <= 'Z')
			*(str + i) += 'a' - 'A';
		*(str + j) = *(str + i);
		j++;
	}
	*(str + j) = 0;
}

bool Calc::Operat(char c){
	switch (c){
	case '+':
	case '-':
	case '*':
	case '/':return 1;
	default:return 0;
	}
}

bool Calc::Is_Num(char c){
	return c >= 48 && c <= 57;
}

bool Calc::CheckCh(const char *str, int pos)
{
	int i, j, k; //i扫描到字符串第i个字符，j控制行，k控制列

	for (i = pos; *(str + i); i++){
		if (Is_Num(*(str + i))
			|| Operat(*(str + i)) || *(str + i) == '.'
			|| *(str + i) == '(' || *(str + i) == ')')
			continue;
		for (j = 0; j<MAXFUN; j++){
			for (k = 0; k<MAXFUN; k++){
				if (*(str + i + k) != *(*(MathFun + j) + k)) //递归调用MathFun 检查是否匹配数学函数
					break;
			}
			if (k >= 3)
				break;
		}
		if (j >= 3){
			printf("在%d位置出现非法字符\n", i);
			return 0;
		}
		else{
			if (*(str + i + 3) != '('){
				printf("在%d位置缺少左括号\n", i + 3);
				return 0;
			}
			return CheckCh(str, i + 3);
		}
	}
	return 1;
}

bool Calc::CrectB(const char *str)
{
	stack<int> s;

	for (int i = 0; *(str + i); i++){
		if (*(str + i) != '(' && *(str + i) != ')')
			continue;
		if (*(str + i) == '('){
			s.push(i);
		}
		else
		if (s.empty()){
			printf("在%d位置出现多余右括号\n", i);
			return 0;
		}
		else{
			NextB[s.top()] = i;
			s.pop();
		}
	}
	if (!s.empty()){
		printf("在%d位置出现多余左括号\n", s.top());
		return 0;
	}
	return 1;
}

bool Calc::CheckError(const char *str, int len){
	for (int i = 0; i<len; i++){
		if (*(str + i) == '('){
			if (i<len - 1 && Operat(str[i + 1]) && str[i + 1] != '-'){
				printf("在%d位置缺少运算符\n", i + 1);
				return 0;
			}
			if (i>0 && (Is_Num(str[i - 1]) || str[i - 1] == ')')){
				printf("在%d位置缺少运算符\n", i);
				return 0;
			}
		}
		else
		if (*(str + i) == ')'){
			if (i>0 && (Operat(str[i - 1]) || str[i - 1] == '(')){
				if (Operat(str[i - 1]))
					printf("在%d位置缺少运算符\n", i);
				else
					printf("在%d位置缺少数字\n", i);
				return 0;
			}
			if (i<len - 1 && Is_Num(str[i + 1])){
				printf("在%d位置缺少运算符\n", i + 1);
				return 0;
			}
		}
		else
		if (i>0 && Operat(*(str + i)) && Operat(str[i - 1])){
			printf("在%d位置缺少数字\n", i);
			return 0;
		}
	}
	return 1;
}

bool Calc::Check(char *str, int & len){
	if (len<(1 << 1)){
		puts("表达式长度异常");
		return 0;
	}
	str[len] = 0;
	if (!CheckCh(str, 0) || !CrectB(str) || !CheckError(str, len))
		return 0;
	return 1;
}
bool Calc::Equal(double a, double b){
	if (fabs(a - b)<eps)
		return 1;
	return 0;
}

double Calc::GetV(const char *str, int st, int ed){
	struct P{
		double x, flag;
		bool point;
		int sign;
		P(){ Init(); }
		void Init(){
			x = 0.0; flag = 1e-1;
			sign = 1; point = 0;
		}
	}Num;
	stack<double> S;
	double *Suffix = new double[ed - st + 1];
	int sz = 0;
	int i;
	for (i = st; i<ed; i++){
		if (Is_Num(*(str + i)) || *(str + i) == '.')
		if (*(str + i) == '.')
		if (Num.point == 1){
			printf("在%d位置出现多余小数点\n", i);
			return ERRORX;
		}
		else
			Num.point = 1;
		else
		if (Num.point == 1){
			Num.x += Num.flag*(*(str + i) - 48);
			Num.flag *= 1e-1;
		}
		else
			Num.x = Num.x*1e1 + (*(str + i) - 48);
		else{
			if (i>st && Is_Num(str[i - 1])){
				Suffix[sz++] = Num.x*Num.sign;
				Num.Init();
			}
			if (*(str + i) == 's' || *(str + i) == 'c' || *(str + i) == 't'){
				double ret = 0.0;
				switch (*(str + i)){
				case 's':ret = sin(GetV(str, i + 4, NextB[i + 3])); break;
				case 'c':ret = cos(GetV(str, i + 4, NextB[i + 3])); break;
				default:ret = tan(GetV(str, i + 4, NextB[i + 3]));
				}
				if (Equal(ret, ERRORX))
					return ERRORX;
				Num.x = ret;
				Suffix[sz++] = Num.x*Num.sign;
				Num.Init();
				i = NextB[i + 3];
			}
			else
			if (*(str + i) == ')'){
				while (!S.empty() && !Equal(HASHL, S.top())){
					Suffix[sz++] = S.top();
					S.pop();
				}
				S.pop();
			}
			else{
				char c = *(str + i);
				if (*(str + i) == '-'){
					Num.sign = -Num.sign;
					if (i>st && str[i - 1] != '(')
						c = '+';
					else
						continue;
				}
				while (!S.empty() && !Equal(S.top(), HASHL) && Prio(S.top()) >= Prio(hash(c))){
					Suffix[sz++] = S.top();
					S.pop();
				}
				S.push(hash(c));
			}
		}
	}
	if (Is_Num(str[ed - 1]))
		Suffix[sz++] = Num.x*Num.sign;
	while (!S.empty()){
		Suffix[sz++] = S.top();
		S.pop();
	}
	double a, b, cur;
	for (i = 0; i<sz; i++){
		cur = Suffix[i];
		if (cur>-inf + 10){
			S.push(cur);
		}
		else{
			b = S.top();
			S.pop();
			a = S.top();
			S.pop();
			if (Equal(HASHA, cur))
				S.push(a + b);
			else
			if (Equal(HASHS, cur))
				S.push(a - b);
			else
			if (Equal(HASHM, cur))
				S.push(a*b);
			else
			{
				if (Equal(b, 0.0))
				{
					puts("错误:除数出现0!");
					return ERRORX;
				}
				S.push(a / b);
			}
		}
	}
	delete[]Suffix;
	return S.top();
}

bool Calc::Cac(){
	DelandLower(Exp);
	int len = strlen(Exp);
	if (!Check(Exp, len)) return 0;
	Ans = GetV(Exp, 0, len);
	if (Equal(Ans, ERRORX))
		return 0;
	return 1;
}
