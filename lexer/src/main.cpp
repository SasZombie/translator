#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

bool transTable = false;
bool classBlock = false;
bool lineRequired = false;
bool mainFunction = false;

int fileDepth = 0;
int classIndent = 0;

std::string keyword;
std::string currPath;
std::string currClassName;

std::vector<char> indentChars;

constexpr static void writing(const std::string &name) noexcept;

constexpr static void removeSpace(std::string &str) noexcept;
constexpr static void transformIf(std::string &str) noexcept;
constexpr static void removeSemCol(std::string &str) noexcept;
constexpr static void transformFor(std::string &str) noexcept;
constexpr static void transformElse(std::string &str) noexcept;
constexpr static void transformWhile(std::string &str) noexcept;
constexpr static void transfomrArray(std::string &str) noexcept;
constexpr static void transformForC(std::string &token) noexcept;
constexpr static void transformElseIf(std::string &str) noexcept;
constexpr static void transfomClass(std::string& token) noexcept;
constexpr static void transformInclude(std::string &token) noexcept;
constexpr static void transformForEach(std::string &token) noexcept;
constexpr static void removeSpacesFront(std::string &name) noexcept;
constexpr static void transfomConstructor(std::string& token) noexcept;
constexpr static void transformCalculations(std::string &str) noexcept;


constexpr static std::string isToken(const std::string& token) noexcept;
constexpr static std::string isKeyword(const std::string& token) noexcept;
constexpr static std::string isFunction(const std::string& token) noexcept;
constexpr static std::string isVariable(const std::string& token) noexcept;
constexpr static std::string isParameter(const std::string& token) noexcept;


static std::unordered_map<std::string, std::string> translationTable = {{"std::cout<<", "print("},{"std::endl", "'\\n'"}, {"std::cin>>", "input("}, {"this->", "self."},
                                                                        {"//", "#"}, {"return", "return "}, {"x=this", " =+ "}, {"std::pow(", "math.pow( "}};

static std::unordered_map<std::string, std::string> headerTable = {{"#include<iostream>", ""}, {"#include<cmath>", "import math"}};                                                                        


static std::unordered_map<std::string, std::function<void(std::string &token)>> translationTableKeys = {{"for", transformFor}, 
                                                            {"while", transformWhile}, {"if", transformIf}, {"else", transformElse},
                                                            {"++", transformCalculations}, {"--", transformCalculations}, {"class", transfomClass}, {"#", transformInclude},
                                                            {"[", transfomrArray}};

static std::vector<std::string> keyWords = {"for", "while", "else", "if", "++", "--", "class", "#", "["};
static std::vector<std::string> unTranslatableKeyWords = {"const", "constexper", "static", "inline", "swich", "public:", "private:"};
static std::vector<std::string> primitives = {"int", "float", "double", "void", "char", "std::string"};

static std::vector<std::string> customClasses;


static std::vector<std::function<std::string(const std::string& token)>> functions = {isToken, isFunction, isVariable, 
                                                                                    isKeyword}; 

int main(int argc, char **argv)
{
    writing(argv[1]);
}

constexpr void writing(const std::string &name) noexcept
{
    std::ifstream fileReader(name);

    std::string buff;
    for(size_t i = 0; i < name.size(); ++i)
    {
        if(name[i] == '/')
        {
            currPath = currPath + buff;
            buff.clear();
        }
        else
            buff = buff + name[i];
    }


    std::string fileExtention = name;
    fileExtention.erase(std::remove_if(fileExtention.begin(), fileExtention.end(), [](char c) { return c == '.'; }), fileExtention.end());

    fileExtention.append(".py");

    std::ofstream fileWriter(fileExtention);

    if(!fileReader.is_open())
    {
        std::cerr << "Error Opening the file!";
        return;
    }

    char ch;
    std::string currKeyWord;
    std::string word;

    std::vector<char> specialChars;


    while (fileReader.get(ch))
    {   

        if(lineRequired)
        {
            if(ch != '\n')
            {
                word = word + ch;
                continue;
            }
            
            lineRequired = false;
            translationTableKeys[keyword](word);
        }  
        
        switch (ch)
        {
        case '\n':
            
            removeSemCol(word);
            removeSpacesFront(word);
            
            fileWriter << word;
            if(!specialChars.empty())
            {

                fileWriter << specialChars.back();
                specialChars.pop_back();
            }
            fileWriter << '\n';

            for(const char c : indentChars)
                fileWriter << c;

            
            if(keyword == "else" || keyword == "if" || keyword == "while" || keyword == "for")
            {
                indentChars.pop_back();
                keyword.clear();
            }
            word.clear();
            continue;

        case '{':
            indentChars.push_back('\t');

            if(classBlock)
                ++classIndent;
            break;
        case '}':
            indentChars.pop_back();
            if(classIndent > 0)
                --classIndent;
            if(classIndent == 0)
            {
                classBlock = false;
                currClassName.clear();
            }
            break;
        default:
            word = word + ch;
            break;
        }
    
        size_t index = 0;
        std::string token = functions[0](word);

        if(word.size() > 4)
        {
            std::string temp = word;
            removeSpacesFront(temp);
            for(const std::string& str : unTranslatableKeyWords)
            {
                if(str == temp)
                {
                    word.clear();
                }
            }
        }
        while (token.empty() &&  index < functions.size() - 1)
        {
            ++index;
            token = functions[index](word);
        }


        if(!token.empty())
        {
            if(token.back() == '(')
            {

                specialChars.push_back(')');
            }
            fileWriter << token;

            word.clear();
        }
    }

    if(mainFunction)
    {
        fileWriter << "\nif __name__ == \"__main__\":\n\tmain()";
    }

    fileReader.close();
    fileWriter.close();
}


constexpr std::string isToken(const std::string &token) noexcept
{    

    std::string noSpaces;
    std::remove_copy_if(token.begin(), token.end(), std::back_inserter(noSpaces), ::isspace);

    if(noSpaces.find('=') != std::string::npos && noSpaces.size() < 3)
        return noSpaces;
    if(translationTable.find(noSpaces) != translationTable.end())
        return translationTable[noSpaces];
    return std::string();
}

constexpr std::string isKeyword(const std::string &token) noexcept
{
    if(token.find('"') != std::string::npos)
        return std::string();

    size_t posPrim = 0;
    std::string newToken;

    for(const auto &str : keyWords)
    {
        
        if((posPrim = token.find(str)) != std::string::npos)
        {
            keyword = str;
            lineRequired = true;
        }
    }
    return std::string();
}

constexpr std::string isFunction(const std::string &token) noexcept
{

    for(const std::string &str : primitives)
    {

        std::string newToken = token;
        removeSpacesFront(newToken);
        size_t posPrim = newToken.find(str);
        size_t startPos = newToken.find('(');
        size_t endPos = newToken.find(')');

        if(posPrim != std::string::npos && startPos != std::string::npos && endPos != std::string::npos && posPrim < startPos )
        {
            for(const std::string &str2 : customClasses)
                if(newToken.find(str2) != std::string::npos && !classBlock)
                    return std::string();
            if(str == currClassName)
                newToken.replace(posPrim, str.length(), "def __init__");
            else
                newToken.replace(posPrim, str.length(), "def");
            newToken.append(":");
        
            startPos = newToken.find('(');
            endPos = newToken.find(')');

            std::string substring = isParameter(newToken.substr(startPos + 1, endPos - startPos - 1));

            if(!substring.empty())
                newToken.replace(startPos + 1, endPos - startPos - 1, substring);
            
            if(classBlock)
                newToken.insert(newToken.find('(') + 1, "self,");

            if(newToken == "def main():")
                mainFunction = true;

            return newToken;
        }
    }

    return std::string();
}

constexpr std::string isParameter(const std::string &token) noexcept
{    
   
    std::string returnString;
    for(size_t i = 0; i < token.size(); ++i)
    {
        while (!std::isspace(token[i]))
        {
           ++i; 
        }

        while (std::isspace(token[i]))
        {
           ++i; 
        }
        
        while ((!std::isspace(token[i])) && token[i-1] != ',' && i < token.size()) 
        {
            returnString = returnString + token[i];
            ++i;
        }
    }

    return returnString;
}

constexpr std::string isVariable(const std::string &token) noexcept
{    
    for(const std::string &str2 : customClasses)
    {
        size_t posPrim = 0;
      

        if((posPrim = token.find(str2) != std::string::npos) && ((token.find(";") != std::string::npos) || (token.find("=") != std::string::npos)))
        {

            std::string newToken = token;
            removeSpacesFront(newToken);
            newToken.erase(posPrim - 1, str2.length());
            newToken.insert(newToken.find('('), "=" + str2);
            removeSemCol(newToken);
            removeSpacesFront(newToken);

            return newToken;
        }

    }

    for(const std::string &str : primitives)
    {
        size_t posPrim = 0;

       

        if((posPrim = token.find(str) != std::string::npos) && ((token.find(";") != std::string::npos) || (token.find("=") != std::string::npos)))
        {
            if(classBlock)
                return " ";
            std::string newToken = token;
            removeSpace(newToken);
            newToken.erase(posPrim - 1, str.length());
            removeSemCol(newToken);
            return newToken;
        }
    }
    return std::string();
}


constexpr void removeSpace(std::string &str) noexcept
{
    if(str.empty())
        return;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if(str[i] == '"')
        {
            ++i;
            while (str[i] != '"')
                ++i;
        }

        if(std::isspace(str[i]))
        {
            str.erase(i, 1);
            --i;
        }
    }
        
}

constexpr void removeSemCol(std::string &str) noexcept
{   
    if(str.find(';') != std::string::npos)
        str.erase(str.find(';'), 1);
}

constexpr void transformIf(std::string &str) noexcept
{
    std::replace_if(str.begin(), str.end(),
                            [](char c) { return c == '(' || c == ')'; }, ' ');
    str.append(":");

    indentChars.push_back('\t');
}

constexpr void transformElse(std::string &str) noexcept
{
    if(str.find("if") != std::string::npos)
        transformElseIf(str);
    else
    {
        str.append(":");
        indentChars.push_back('\t');
    }
}

constexpr void transformElseIf(std::string &str) noexcept
{
    removeSpacesFront(str);
    std::replace_if(str.begin(), str.end(),
                            [](char c) { return c == '(' || c == ')'; }, ' ');
    str.append(":");
    str.replace(str.find("else if"), 7, "elif");

    indentChars.push_back('\t');
}

constexpr void transformWhile(std::string &str) noexcept
{
    std::replace_if(str.begin(), str.end(),
                            [](char c) { return c == '(' || c == ')'; }, ' ');
    str.append(":");
    indentChars.push_back('\t');
}

constexpr void transformFor(std::string &str) noexcept
{
    if(str.find(':') != std::string::npos)
        transformForEach(str);
    else
        transformForC(str);

    indentChars.push_back('\t');
}

constexpr void transformForC(std::string &token) noexcept
{
    std::string varName, first, last, step;
    int add = 0;
    for(size_t i = 0; i < token.length(); ++i)
    {
        
        if(token[i] == '=')
        {   
            int j = i - 1;
            while (std::isspace(token[j]))
            {
                --j;
            }
            
            while (!std::isspace(token[j]) && token[j] != '(')
            {
                varName.insert(varName.begin(), token[j]);
                --j;
            }
            
            ++i;
            while (std::isspace(token[i]))
                ++i;
                    
            while(token[i] != ';')
            {
                first = first + token[i];
                ++i;
            }
        }

        if(token[i] == '<' || token[i] == '>')
        {       
            ++i;
            if(token[i] == '=')
            {
                if(token[i-1] == '<')
                    add = 1;
                if(token[i-1] == '>')
                    add = -1;
                ++i;
            }
            while (std::isspace(token[i]))
            {
                ++i;
            }
                
            while(token[i] != ';')
            {
                last = last + token[i];
                ++i;
            }
        }        
    }

    int i = token.size() - 1;
    --i;

    while (token[i] != ';')
    {
        step.insert(step.begin(), token[i]);
        --i;
    }

    removeSpacesFront(step);
    std::string newStep;

    if(step == "++i" || step == "i++")
    {
        newStep = '1';
    }
    else if(step == "--i" || step == "i--")
    {
        newStep = "-1";
    }
    else
    {        
        for(const char c : step)
        {
            if(std::isdigit(c) || c == '-' || c == '+' || c == '*' || c == '/')
                newStep = newStep + c;
        }
    }

    std::string result = "for " + varName + " in range(" + first + ',' + last + '+' + std::to_string(add) + ',' + newStep + "):\n";
    token = result;
}



constexpr void removeSpacesFront(std::string &str) noexcept
{
    size_t i = 0;
    while (std::isspace(str[i]))
    {
        str.erase(i, 1);
    }
}

constexpr void transformForEach(std::string &token) noexcept
{
    std::string name, place;

    int position = token.find(':');
    int position2 = position;

    --position;
    while (std::isspace(token[position]))
    {
        --position;
    }
    
    while (!std::isspace(token[position]))
    {
        name.insert(name.begin(), token[position]);
        --position;
    }

    ++position2;
    while (std::isspace(token[position2]))
    {
        ++position2;
    }
    while (!std::isspace(token[position2]) && token[position2] != ')')
    {
        place = place + token[position2];
        ++position2;
    }

    token = std::string("for " + name + " in " + place + " :\n");
}

constexpr void transformCalculations(std::string &str) noexcept
{
    removeSpace(str);
    std::string name;
    for(size_t i = 0; i < str.size(); ++i)
    {
        if(str[i] != '+' && str[i] != '-')
        {
            name = name + str[i];
        }
    }

    if(str.find('+') != std::string::npos)
        str = name + '=' + name + " + 1\n";
    else
        str = name + '=' + name + " - 1\n";

    removeSemCol(str);
}

constexpr void transfomClass(std::string &token) noexcept
{
    currClassName = token;
    currClassName.erase(currClassName.find("class"), 5/*size of the word "class" :3*/);
    removeSpacesFront(currClassName);
    primitives.push_back(currClassName);
    customClasses.push_back(currClassName);
    classBlock = true;
    token.append(":");
}

constexpr void transfomConstructor(std::string &token) noexcept
{
    token.replace(token.find(currClassName), currClassName.size(), "def __init__(self, ");
}

constexpr void transformInclude(std::string &token) noexcept
{
    if(token.find(';') != std::string::npos) // #include<iostream> doesnt have ';'. So this must come from a string literal!
        return;
    removeSpace(token);

    if(token.find('<') != std::string::npos)
    {
        if(headerTable.find(token) != headerTable.end())
            token = headerTable[token];
        else
            token = "#Library " + token + " not supported";
    }
    else
    {
        ++fileDepth;
        size_t startPos = token.find("\"");
        size_t endPos = token.find("\"", startPos + 1);
        std::string substr = token.substr(startPos + 1, endPos - startPos - 1);
        substr.erase(std::remove_if(substr.begin(), substr.end(), [](char c) { return c == '.'; }), substr.end());
        writing(currPath + '/' + token.substr(token.find("\"") + 1, endPos - startPos - 1));
        token = "import " + substr; 
    }
}

constexpr void transfomrArray(std::string &str) noexcept
{
    for(const std::string& prim : primitives)
    {
        size_t posPrim = 0;
        if((posPrim = str.find(prim)) != std::string::npos)
        {
            str.erase(posPrim, prim.length());
            removeSpace(str);
            int i = 0;
            while (str[i] != '[')
            {
                ++i;
            }
            
            while (str[i] != ']')
            {
                str.erase(i, 1);
            }
            
            str.erase(i, 1);


            std::replace(str.begin(), str.end(), '{', '[');
            std::replace(str.begin(), str.end(), '}', ']');

            return;
        } 
    }
}
