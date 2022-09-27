#include <iostream>
#include <string>
#include <algorithm>
#include <climits>
#include <ctype.h>
#include <iterator>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem> 
#include <unordered_set>
using namespace std;
namespace fs = std::filesystem;
#define KEYWORD 1
#define SYMBOL  2
#define IDENTIFIER 3
#define INT_CONST  4
#define STRING_CONST 5

#define CLASS 6
#define METHOD 7
#define FUNCTION 8
#define CONSTRUCTOR 9
#define INT 10
#define BOOLEAN 11
#define CHAR 12
#define VOID 13
#define VAR 14
#define STATIC 15
#define FIELD 16
#define STATEMENTS 17
#define ELSE 20
#define TRUE 23
#define FALSE 24
#define K_NULL 25
#define THIS 26

class Tokenizer {
public:
  Tokenizer(string name){
    input.open(name);
    currentL = "";
    i = 0;
    vector<string> array =
    {"class","constructor","function","method","field","static","var","int","char","boolean","void","true","false","null","this","let","do","if","else","while","return"};
    keywords = unordered_set<string>(array.begin(), array.end());
    vector<string> arr = {"{","}","(",")","[","]",".",";",",","+","-","*","/","<",">","=","~"};
    symbols = unordered_set<string>(arr.begin(), arr.end());
  };
  ~Tokenizer() {input.close();}

  bool hasMoreTokens(){
      int len = currentL.length();
      nexttoken = "";
      bool comment = false;
      while (i >= len || len == 0) {
        if (!getline(input, currentL))
	  return false;
	i = 0;
        currentL.erase(std::find(currentL.begin(), currentL.end(), '\r'), currentL.end());
        currentL = currentL.substr(0, currentL.find("//"));
//	cout<<"currentL: "<< currentL<<endl;
        len = currentL.length();
      if (currentL.find("/*") != string::npos) {
        if (currentL.find("*/") != string::npos) {
          currentL = currentL.substr(0, currentL.find("/*"));
          len = currentL.length();
	} else {
	  while (currentL.find("*/") == string::npos)
            getline(input, currentL);
          i = len;
	}
      }
      while (i < len && (currentL[i] == ' ' || currentL[i] == '\t' ||
      currentL[i] == '\r')) {i++;}
/*      if (i < len-1 && currentL[i] == '/' && currentL[i+1] == '*') {
        i++;
	while (i < len && currentL[i-1] != '*' && currentL[i] != '/') {
	  i++;
	  if (i == len){
            if (!getline(input, currentL))
	      return false;
	    i = 0;
            currentL.erase(std::find(currentL.begin(), currentL.end(), '\r'), currentL.end());
            len = currentL.length();
	  }
	}
	i++;
      }*/
      }

      if (isalpha(currentL[i])) {
        while (isalpha(currentL[i]) || isdigit(currentL[i]) || currentL[i] == '_')
	  nexttoken = nexttoken + currentL[i++];
      } else if (isdigit(currentL[i])) {
        while (isdigit(currentL[i]))
	  nexttoken = nexttoken + currentL[i++];
      } else if (currentL[i] == '\"') {
          nexttoken = currentL[i++];
	  while (currentL[i] != '\"')
            nexttoken = nexttoken + currentL[i++];
	  nexttoken = nexttoken + currentL[i++];
      } else
        nexttoken = currentL[i++];
      while (i < len && (currentL[i] == ' ' || currentL[i] == '\t')) {i++;}
      return !nexttoken.empty();
  }

  string getToken(){return token;}

  int tokenType() {
     if (keywords.count(token))
       return KEYWORD;
     if (symbols.count(token))
       return SYMBOL;
     if (isdigit(token[0]))
       return INT_CONST;
     if (isalpha(token[0]))
       return IDENTIFIER;
     if (token[0] == '\"')
       return STRING_CONST;
     return -1;
  }
  
  int advance() {
    if (!hasMoreTokens()) {
      cout<<"end: no more tokens"<<endl;
      return 0;
    }
    token = nexttoken;
    return tokenType();
  }

  int getKeyword() {
    if (token  == "class")
      return CLASS;
    if (token  == "constructor")
      return CONSTRUCTOR;
    if (token  == "function")
      return FUNCTION;
    if (token  == "method")
      return METHOD;
    if (token  == "field")
      return FIELD;
    if (token  == "static")
      return STATIC;
    if (token  == "var")
      return VAR;
    if (token  == "int")
      return INT;
    if (token  == "char")
      return CHAR;
    if (token  == "boolean")
      return BOOLEAN;
    if (token  == "void")
      return VOID;
    if (token  == "true")
      return TRUE;
    if (token  == "false")
      return FALSE;
    if (token  == "null")
      return K_NULL;
    if (token  == "this")
      return THIS;
    if (token  == "let" || token == "do" || token == "if" || token == "while" ||
    token == "return")
      return STATEMENTS;
    return -1;
  }

private:
  ifstream input;
  string currentL;
  string token;
  string nexttoken;
  int i;
  unordered_set<string> keywords;
  unordered_set<string> symbols;
};

class Parser {
public:
Parser(string outname, Tokenizer* tkz) {this->tkz = tkz; outfile.open(outname +
".xml");};
~Parser() {outfile.close();};

  bool compileClassVarDec() {
    // static | field
    if (tkz->tokenType() != KEYWORD)
      return false;
    int kw = tkz->getKeyword();
    if (kw == STATIC) {
      outfile << "<classVarDec>" <<endl;
      outfile << "<keyword> static </keyword>" <<endl;
    } else if (kw == FIELD) {
      outfile << "<classVarDec>" <<endl;
      outfile << "<keyword> field </keyword>" <<endl;
    } else
      return false;
    // type
    tkz->advance();
    types();
    // varName
    tkz->advance();
    if (tkz->tokenType() == IDENTIFIER)
        oprint("identifier", "");
    else
      return false;
    // (, varName)*
    varNames();
    // ;
    oprint("symbol", ";");
    tkz->advance();
    outfile << "</classVarDec>" <<endl;
    return true;
  }
  void types() {
    if (tkz->tokenType() == KEYWORD) {
      int tp = tkz->getKeyword(); 
      if (tp == INT || tp == CHAR || tp == BOOLEAN)
        oprint("keyword", "");
      else
        return;
    } else if (tkz->tokenType() == IDENTIFIER)
        oprint("identifier", "");
    else
      return;
  }
  void varNames() {
    tkz->advance();
    while (tkz->getToken() == ",") {
      oprint("symbol", "");
      tkz->advance();
      if (tkz->tokenType() == IDENTIFIER)
        oprint("identifier", "");
      else
        return;
     tkz->advance();
    }
  }
  void oprint(string ele, string expectedToken) {
    string token = tkz->getToken();
    if (expectedToken != "" && expectedToken != token) {
      cout<<"wrong token: " << token<<endl;
      return;
    }
    if (token == "<")
      token = "&lt;";
    if (token == ">")
      token = "&gt;";
    if (token == "\"")
      token = "&quot;";
    if (token == "&")
      token = "&amp;";
    outfile << "<" +ele + "> " + token + " </" + ele + ">" <<endl;
  }
  bool compileSubroutine() {
    if (tkz->tokenType() != KEYWORD)
      return false;
    int kw = tkz->getKeyword();
    if (kw == CONSTRUCTOR) {
      outfile << "<subroutineDec>"<<endl;
      outfile << "<keyword> constructor </keyword>" <<endl;
    }
    else if (kw == FUNCTION) {
      outfile << "<subroutineDec>"<<endl;
      outfile << "<keyword> function </keyword>" <<endl;
    } else if (kw == METHOD) {
      outfile << "<subroutineDec>"<<endl;
      outfile << "<keyword> method </keyword>" <<endl;
    } else 
      return false;
    tkz->advance();
    if (tkz->getToken() == "void")
      oprint("keyword", "");
    else
      types();
    tkz->advance();
    oprint("identifier", "");
    tkz->advance();
    oprint("symbol", "(");
    tkz->advance();
    compileParameterList();
    oprint("symbol", ")");
    tkz->advance();
    outfile << "<subroutineBody>" << endl;
    oprint("symbol", "{");
    tkz->advance();
    while (tkz->getToken() == "var")
      compileVarDec();
    compileStatements();
    oprint("symbol", "}");
    outfile << "</subroutineBody>" << endl;
    outfile << "</subroutineDec>"<<endl;
    tkz->advance();
    return true;
  }
  void compileClass() {
    outfile << "<class>" <<endl;
    outfile << "<keyword> class </keyword>" <<endl;
    if (tkz->advance() != IDENTIFIER)
      return;
    oprint("identifier", "");
    tkz->advance();
    oprint("symbol", "{");
    tkz->advance();
    while (compileClassVarDec()) {}
    while (compileSubroutine()) {}
    oprint("symbol", "}");
    outfile << "</class>" <<endl;
  }

  void compileParameterList() {
    outfile << "<parameterList>" <<endl;
    if (tkz->tokenType() != KEYWORD && tkz->tokenType() != IDENTIFIER){
      outfile << "</parameterList>" <<endl;
      return;
    }
    types();
    tkz->advance();
    oprint("identifier", "");
    tkz->advance();
    while (tkz->getToken() == ",") {
      oprint("symbol", "");
      tkz->advance();
      types();
      tkz->advance();
      if (tkz->tokenType() == IDENTIFIER)
        oprint("identifier", "");
      else
        return;
     tkz->advance();
    }
    outfile << "</parameterList>" <<endl;
  }
  void compileVarDec() {
    outfile << "<varDec>" <<endl;
    oprint("keyword", "var");
    tkz->advance();
    types();
    tkz->advance();
    oprint("identifier", "");
    varNames();
    oprint("symbol", ";");
    tkz->advance();
    outfile << "</varDec>" <<endl;
  }
  void compileStatements() {
    outfile << "<statements>" <<endl;
    int type = tkz->getKeyword();
    if (type != STATEMENTS) {
      outfile << "</statements>" <<endl;
      return;
    }
    while (type == STATEMENTS) {
      string token = tkz->getToken();
      if (token == "let") {
        compileLet();
      } else if (token == "if") {
        compileIf();
        type = tkz->getKeyword();
	continue;
      } else if (token == "while") {
        compileWhile();
      } else if (token == "do") {
        compileDo();
      } else if (token == "return") {
        compileReturn();
      }
      tkz->advance();
      type = tkz->getKeyword();
    }
    outfile << "</statements>" <<endl;
  }
  void compileDo() {
    outfile << "<doStatement>" <<endl;
    oprint("keyword", "do");
    tkz->advance();
    oprint("identifier", "");
    tkz->advance();
    compileSubroutineCall();
    tkz->advance();
    oprint("symbol", ";");
    outfile << "</doStatement>" <<endl;
  }
  void compileLet() {
      outfile << "<letStatement>" <<endl;
      oprint("keyword", "let");
      tkz->advance();
      if (tkz->tokenType() == IDENTIFIER)
        oprint("identifier", "");
      else
        return;
      tkz->advance();
      if (tkz->getToken() == "[") {
        oprint("symbol", "[");
        tkz->advance();
	compileExpression();
        oprint("symbol", "]");
        tkz->advance();
      }
      oprint("symbol", "=");
      tkz->advance();
      compileExpression();
      oprint("symbol", ";");
      outfile << "</letStatement>" <<endl;
  }
  void compileWhile() {
    outfile << "<whileStatement>" <<endl;
    oprint("keyword", "while");
    tkz->advance();
    oprint("symbol", "(");
    tkz->advance();
    compileExpression();
    oprint("symbol", ")");
    tkz->advance();
    oprint("symbol", "{");
    tkz->advance();
    compileStatements();
    oprint("symbol", "}");
    outfile << "</whileStatement>" <<endl;
  }
  void compileReturn() {
    outfile << "<returnStatement>" <<endl;
    oprint("keyword", "return");
    tkz->advance();
    if (tkz->getToken() != ";")
      compileExpression();
    oprint("symbol", ";");
    outfile << "</returnStatement>" <<endl;
  }
  void compileIf() {
    outfile << "<ifStatement>" <<endl;
      oprint("keyword", "if");
      tkz->advance();
      oprint("symbol","(");
      tkz->advance();
      compileExpression();
      oprint("symbol", ")");
      tkz->advance();
      oprint("symbol", "{");
      tkz->advance();
      compileStatements();
      oprint("symbol", "}");
      tkz->advance();
      if (tkz->getToken() == "else") {
        oprint("keyword", "");
        tkz->advance();
        oprint("symbol", "{");
        tkz->advance();
        compileStatements();
        oprint("symbol", "}");
        tkz->advance();
      }
      outfile << "</ifStatement>" <<endl;
  }
  void compileExpression() {
    outfile << "<expression>"<<endl;
    compileTerm();
    string next = tkz->getToken();
    while (next == "+" || next == "-" || next == "*" || next == "/" || next ==
    "&" || next == "|" || next == "<" || next == ">" || next == "=") {
      oprint("symbol", "");
      tkz->advance();
      compileTerm();
      next = tkz->getToken();
    }
    outfile << "</expression>"<<endl;
  }
  void compileTerm() {
    string currentToken = tkz->getToken();
    outfile << "<term>" <<endl;
    if (tkz->tokenType() == INT_CONST)
      oprint("integerConstant", "");
    else if (tkz->tokenType() == STRING_CONST) {
      string sc = tkz->getToken();
      sc = sc.substr(1, sc.length()-2);
      outfile << "<stringConstant> " + sc + " </stringConstant>" << endl; 
    }
    else if (tkz->tokenType() == KEYWORD) {
      int getkw = tkz->getKeyword();
      if (getkw == TRUE || getkw == FALSE || getkw == K_NULL || getkw == THIS) {
        oprint("keyword","");   
      }
    } else if (currentToken == "-" || currentToken == "~") {
      oprint("symbol", "");
      tkz->advance();
      compileTerm();
      outfile << "</term>" <<endl;
      return;
    } else if (currentToken == "(") {
      oprint("symbol", "(");
      tkz->advance();
      compileExpression();
      oprint("symbol", ")");
    } else {
      tkz->advance();
      string next = tkz->getToken(); 
      if (next == "(" || next == ".") {
        outfile << "<identifier>" + currentToken + "</identifier>" << endl;   
        compileSubroutineCall();
      }
      else if (next == "[") {
        outfile << "<identifier>" + currentToken + "</identifier>" << endl;   
        oprint("symbol", "[");
        tkz->advance();
	compileExpression();
        oprint("symbol", "]");
      } else {
        outfile << "<identifier>" + currentToken + "</identifier>" << endl; 
        outfile << "</term>" <<endl;
	return;
      }
    }
    outfile << "</term>" <<endl;
    tkz->advance();
  }
  void compileSubroutineCall() {
    if (tkz->getToken() == "(") {
      oprint("symbol", "(");
      tkz->advance();
      compileExpressionList(); 
      oprint("symbol", ")");
    } else {
      oprint("symbol", ".");
      tkz->advance();
      oprint("identifier", "");
      tkz->advance();
      oprint("symbol", "(");
      tkz->advance();
      compileExpressionList(); 
      oprint("symbol", ")");
    } 
  }
  void compileExpressionList() {
    outfile << "<expressionList>"<<endl;
    if (tkz->getToken() == ")") {
      outfile << "</expressionList>" << endl;
      return;
    }
    compileExpression();
    while (tkz->getToken() == ",") {
      oprint("symbol", "");
      tkz->advance();
      compileExpression();
    }
    outfile << "</expressionList>"<<endl;
  }
private:
  ofstream outfile; 
  Tokenizer* tkz;
};

int main(int argc, char** argv) {
  string inName = argv[1];
  vector<string> inputList;
  const fs::path path(inName);
  std::error_code ec;
  string outname;
  if (fs::is_regular_file(path, ec)) 
    inputList.push_back(inName);
  else if (fs::is_directory(path, ec)) {
      DIR *dir = opendir(inName.c_str());
      struct dirent *entry;
      while ((entry = readdir(dir)) != NULL) {
        string tmp = entry->d_name;
        tmp = tmp.substr(tmp.find("."));
	if (tmp == ".jack") {
          if (inName[inName.length()-1] == '/')
            inputList.push_back(inName + entry->d_name);
	  else 
            inputList.push_back(inName + "/" + entry->d_name);
	}
      }
      closedir(dir);
  }
  
  for (int i = 0; i < inputList.size(); i++) {
    string outname = inputList[i].substr(0, inputList[i].find("."));
    Tokenizer tkz(inputList[i]);
    Parser    parser(outname, &tkz);
    tkz.advance();
    parser.compileClass();	      
   }
  return 0;
}
