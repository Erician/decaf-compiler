/*************************************************************************
	> File Name: error.cpp
	> Author: 
	> Mail: 
	> Created Time: 2016年12月05日 星期一 15时24分25秒
 ************************************************************************/
#ifndef DC_CORE_DC_CLASS_H_
#include "core/dc_class.h"
#endif 

#include <iostream>
#include <vector>
#include <string>

#include "yacc/y.tab.h"

#ifndef DC_LEX_FLEX_H_
#include "lex/flex.h"
#endif

#ifndef DC_CORE_DC_ERROR_H_
#include "core/dc_error.h"
#endif

std::string IssueError::ReplaceTabByBlank(std::string str)
{
    do{
        long long pos = str.find('\t');
        if (pos >= 0 || pos < str.length()){
            str = str.substr(0, pos) + "    " + str.substr(pos+1, str.length());
        }
        else{
            break;
        }
    }while(true);
    return str;
}

void IssueError::FlagErrorPlace(const YYLTYPE *pyylloc)
{
    std::cout<<"    ";
    for(int i=0;i<pyylloc->first_column-1;i++)
        std::cout<<' ';
    std::cout<<'^'<<std::endl;
}

void IssueError::PrintLocation(const YYLTYPE *pyylloc)
{
    if (pyylloc == NULL)
        return;
    std::cout<<pyylloc->first_line<<":"<<pyylloc->first_column<<":"<<std::endl;
    std::cout<<"    "<<ReplaceTabByBlank(savedlines[pyylloc->first_line-1])<<std::endl;
    FlagErrorPlace(pyylloc);
}

void IssueError::UnMachedChar(const YYLTYPE *pyylloc,char c)
{
    std::cout<<"error: "<<"\'"<<c<<"\' "<<"unmatched char ";
    IssueError::PrintLocation(pyylloc);
}

void IssueError::Printyyerror(const YYLTYPE *pyylloc,std::string str)
{
    std::cout<<"syntax error: "<<"\'"<<str<<"\' ";
    IssueError::PrintLocation(pyylloc);
}
//class
void IssueError::UndefinedClass(const YYLTYPE *pyylloc, std::string idname)
{
    std::cout << "undefined class " << idname <<" ";
    IssueError::PrintLocation(pyylloc);
}

void IssueError::RedefinedClass(const YYLTYPE *pyylloc, std::string idname, const YYLTYPE *firstDefinedLocation)
{
    std::cout << "redefined class " << idname <<" ";
    IssueError::PrintLocation(pyylloc);
    std::cout << "first defined here: ";
    IssueError::PrintLocation(firstDefinedLocation);
}

//main
void IssueError::UndefinedMain()
{
    std::cout << "    undefined main "<<std::endl;
    std::cout << std::endl;
}
void IssueError::RedefinedMain(const YYLTYPE *pyylloc, const YYLTYPE *firstDefinedLocation)
{
    std::cout << "redefined main ";
    IssueError::PrintLocation(pyylloc);
    std::cout << "first defined here: ";
    IssueError::PrintLocation(firstDefinedLocation);
}
void IssueError::MainIsNotStatic(const YYLTYPE *pyylloc)
{
    std::cout << "main is not static ";
    IssueError::PrintLocation(pyylloc);
}




