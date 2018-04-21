/*************************************************************************
	> File Name: error.h
	> Author: 
	> Mail: 
	> Created Time: 2016年12月05日 星期一 15时22分39秒
 ************************************************************************/
#include <iostream>
#include "../yacc/location.h"
using namespace std;
#ifndef _ERROR_H
#define _ERROR_H
class IssueError
{
private:
    static void flagErrorPlace(YYLTYPE *pyylloc);
	static string replaceTabByBlank(string str);

public:
    static void UnMachedChar(YYLTYPE *pyylloc,char c);
    static void Printyyerror(YYLTYPE *pyylloc,string str);
	
};

#endif
