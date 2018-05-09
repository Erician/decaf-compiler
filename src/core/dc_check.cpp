#ifndef DC_CORE_DC_CLASS_H_
#include "core/dc_class.h"
#endif

#ifndef DC_CORE_DC_SYMBOL_H_
#include "core/dc_symbol.h"
#endif

#ifndef DC_LEX_FLEX_H_
#include "core/dc_error.h"
#endif

#ifndef DC_CORE_DC_ENUM_H_
#include "core/dc_enum.h"
#endif

#ifndef DC_CORE_DC_CONST_H_
#include "core/dc_const.h"
#endif

#ifndef DC_YACC_LOCATION_H_
#include "yacc/location.h"
#endif

#include <map>

bool Program::checkStaticSemantic()
{
    return gloScope->check();
}

bool GloScope::check()
{
    if(checkClass() == false)
        return false;
    checkMain();
    return checkAttributesAndMethods();
}

GloScopeEntry* GloScope::findClass(std::string className)
{
    for(auto gloScopeEntry : entries)
    {
        if(((GloScopeEntry*)gloScopeEntry) -> getClassName() == className)
        {
            return (GloScopeEntry*)gloScopeEntry;
        }
    }
    return NULL;
}

//class
bool GloScope::checkClass()
{
    return checkUndefinedClass() \
            && checkRedefinedClass();
}

bool GloScope::checkUndefinedClass()
{
    bool noErrors = true;
    for(auto gloScopeEntry : entries)
    {
        noErrors = ((GloScopeEntry*)gloScopeEntry) -> checkUndefinedClass(this) ? noErrors : false;
    }
    return noErrors;
}

bool GloScopeEntry::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    if (claDes->getParentName() != "")
    {
        GloScopeEntry* parentGloScopeEntry = gloScope->findClass(claDes->getParentName());
        if(parentGloScopeEntry == NULL)
        {
            noErrors = false;
            IssueError::UndefinedClass(parentClassLocation, claDes->getParentName());
        }
        else
        {
            claDes->setParentClaDes(parentGloScopeEntry->getClaDes());
        }
    }
    noErrors = claDes->getClaScope()->checkUndefinedClass(gloScope) ? noErrors : false;
    return noErrors;
}

bool ClaScope::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    for(auto claScopeEntry : entries)
    {
        noErrors = ((ClaScopeEntry*)claScopeEntry)->checkUndefinedClass(gloScope) ? noErrors : false;
    }
    return noErrors;
}

bool ClaScopeEntry::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    std::string className = typeInfo->getClassName();
    if(className != "" && gloScope -> findClass(className) == NULL)
    {
        noErrors = false;
        IssueError::UndefinedClass(typeInfo->getLocation(), className);
    }
    if(category == DC::CATEGORY::DC_FUN)
    {
        noErrors = funDes->getForScope()->checkUndefinedClass(gloScope) ? noErrors : false;
    }
    return noErrors;    
}

bool ForScope::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    for(auto forScopeEntry : entries)
        noErrors = ((ForScopeEntry*)forScopeEntry)->checkUndefinedClass(gloScope) ? noErrors : false;
    if(locScopeEntry != NULL)
        noErrors = locScopeEntry->checkUndefinedClass(gloScope) ? noErrors : false;  
    return noErrors;
}

bool ForScopeEntry::checkUndefinedClass(GloScope* gloScope)
{
    std::string className = typeInfo->getClassName();
    if(className != "" && gloScope -> findClass(className) == NULL)
    {
        IssueError::UndefinedClass(typeInfo->getLocation(), className);
        return false;
    }
    else
    {
        return true;
    }
}

bool LocScope::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    for(auto locScopeEntry : entries)
        noErrors = ((LocScopeEntry*)locScopeEntry)->checkUndefinedClass(gloScope) ? noErrors : false;
    return noErrors;
}

bool LocScopeEntry::checkUndefinedClass(GloScope* gloScope)
{
    bool noErrors = true;
    if(name != "")
    {
        std::string className = typeInfo->getClassName();
        if(className != "" && gloScope -> findClass(className) == NULL)
        {
            IssueError::UndefinedClass(typeInfo->getLocation(), className);
            noErrors = false;
        }
    }
    if(subLocScope != NULL)
    {
        noErrors = subLocScope -> checkUndefinedClass(gloScope) ? noErrors : false;
    }
    return noErrors;
}

bool GloScope::checkRedefinedClass()
{
    bool noErrors = true;
    std::map<std::string, YYLTYPE*> doesClassExistMap;
    for(auto gloScopeEntry : entries)
    {
        std::string className = ((GloScopeEntry*)gloScopeEntry) -> getClassName();
        if(doesClassExistMap.find(className) == doesClassExistMap.end())
        {
            doesClassExistMap[className] = ((GloScopeEntry*)gloScopeEntry) -> getLocation();
        }
        else
        {
            noErrors = false;
            IssueError::RedefinedClass(((GloScopeEntry*)gloScopeEntry) -> getLocation(), ((GloScopeEntry*)gloScopeEntry) -> getClassName(), doesClassExistMap[className]);
        }
    }
    return noErrors;
}

//main
bool GloScope::checkMain()
{
    bool noErrors = true;
    bool isFindMain = false;
    YYLTYPE* firstDefinedLocation; 
    for(auto gloScopeEntry : entries)
    {
        ClaScope* claScope = ((GloScopeEntry*)gloScopeEntry)->getClaDes()->getClaScope();
        for(auto claScopeEntry : claScope->getEntries())
        {
            if(((ClaScopeEntry*)claScopeEntry)->getCategory() == DC::CATEGORY::DC_FUN \
                && ((ClaScopeEntry*)claScopeEntry)->getName() == DC::MAIN)
            {
                if (isFindMain == true)
                {
                    noErrors = false;
                    IssueError::RedefinedMain(((ClaScopeEntry*)claScopeEntry) -> getLocation(), firstDefinedLocation);      
                }
                else
                {
                    isFindMain = true;
                    firstDefinedLocation = ((ClaScopeEntry*)claScopeEntry) -> getLocation();
                    FunDes *funDes = ((ClaScopeEntry*)claScopeEntry) -> getFunDes();
                    if(funDes->getIsStatic() == false)
                    {
                        noErrors = false;
                        IssueError::MainIsNotStatic(((ClaScopeEntry*)claScopeEntry) -> getLocation());
                    }
                }
            }
        }
    }
    if(isFindMain == false)
    {
        noErrors = false;
        IssueError::UndefinedMain();
    }
    return noErrors;
}
//attributes and methods
bool GloScope::checkAttributesAndMethods()
{
    bool noErrors = true;
    std::map<std::string, YYLTYPE*> doesAttributeOrMethodExistMap; 
    std::map<std::string, int> attributeOrMethodCategory; 
    for(auto gloScopeEntry : entries)
    {
        doesAttributeOrMethodExistMap.clear();
        attributeOrMethodCategory.clear();
        ClaScope* claScope = ((GloScopeEntry*)gloScopeEntry)->getClaDes()->getClaScope();
        for(auto claScopeEntry : claScope->getEntries())
        {
            std::string name =  ((ClaScopeEntry*)claScopeEntry)->getName();
            if(doesAttributeOrMethodExistMap.find(name) == doesAttributeOrMethodExistMap.end())
            {
                doesAttributeOrMethodExistMap[name] = ((ClaScopeEntry*)claScopeEntry) -> getLocation();
                attributeOrMethodCategory[name] = ((ClaScopeEntry*)claScopeEntry) -> getCategory();
            }
            else
            {
                noErrors = false;
                if(((ClaScopeEntry*)claScopeEntry) -> getCategory() == attributeOrMethodCategory[name])
                {
                    IssueError::RedefinedVarOrMethod(((ClaScopeEntry*)claScopeEntry) -> getLocation(),\
                     name, doesAttributeOrMethodExistMap[name], attributeOrMethodCategory[name]);
                }
                else
                {
                    IssueError::AttributeAndMethodWithTheSameName(((ClaScopeEntry*)claScopeEntry)->getLocation(),\
                     name, doesAttributeOrMethodExistMap[name]);
                }
            }
        }
    }
    return noErrors;
}





