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
#include <unordered_map>
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
#define POINTER 18
#define ELSE 20
#define THAT 19
#define CONST 21
#define LOCAL 22
#define TRUE 23
#define FALSE 24
#define K_NULL 25
#define THIS 26
#define TEMP 27
#define ARG 32
#define NONE 34
static unordered_set<string> voidSubroutines;
static unordered_set<string> methodSubroutines;

class Entry {
public:
  Entry(string n, string t, int k, int i) {
    name = n;
    type = t;
    kind = k;
    index = i;
  };
  string name;
  string type;
  int    kind;
  int    index;
};
class SymbolTable {
public:
  SymbolTable() {
    Entry* ele = new Entry("Output", "Output", 0, 0);
    table["Output"] = ele;
  };
  ~SymbolTable() {clear();};
  void clear() {
    m.clear();
    unordered_map<string, Entry*>::iterator it;
    for (it=table.begin(); it != table.end(); it++) {
      delete it->second;
    }
    table.clear();
  }
  void define(string name, string type,int kind) {
    int index = varCountInc(kind);
    Entry* ele = new Entry(name, type, kind, index);
    table[name] = ele;
  }
  int varCount(int kind) {
    return m[kind];
  }
  int varCountInc(int kind) {
    int cnt = m[kind];
    m[kind]++;
    return cnt;
  }
  int kindof(string name) {
    if (name == "this")
      return POINTER;
    if (table.count(name) == 0)
      return NONE;
    if (table[name]->kind == FIELD)
      return THIS;
    return table[name]->kind;
  }
  string typeof(string name) {
    if (table.count(name) == 0)
      return name;
    return table[name]->type;
  }
  int indexof(string name) {
    if (name == "this")
      return 0;
    if (table.count(name) == 0)
      return NONE;
    return table[name]->index;
  }

private:
  unordered_map<int, int> m; // kind -- cnt 
  unordered_map<string, Entry*> table; // symbol name -- pointer
};
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
	cout<<"currentL: "<< currentL<<endl;
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
class VMWriter {
public:
  VMWriter(string name, SymbolTable* cls, SymbolTable* sub) {
    outfile.open(name +".vm");
    stclass = cls;
    stsub = sub;
  };
  ~VMWriter() {outfile.close();};
  string segtos(int seg) {
    switch (seg) {
      case CONST:
        return "constant";
      case ARG:
        return "argument";
      case LOCAL:
        return "local";
      case STATIC:
        return "static";
      case THIS:
        return "this";
      case THAT:
        return "that";
      case POINTER:
        return "pointer";
      case TEMP:
        return "temp";
      default:
        return "";
    } 
  }
  void writePushToken(string token) {
    if (stclass->kindof(token) != NONE) {
      outfile << "push " + segtos(stclass->kindof(token)) + " " + to_string(stclass->indexof(token)) << endl;
      cout << "push " + segtos(stclass->kindof(token)) + " " + to_string(stclass->indexof(token)) << endl;
    }
    else if (stsub->kindof(token) != NONE) {
      outfile << "push " + segtos(stsub->kindof(token)) + " " + to_string(stsub->indexof(token)) << endl;
      cout << "push " + segtos(stsub->kindof(token)) + " " + to_string(stsub->indexof(token)) << endl;
    }
    else
      cout<< "token type not found"<<endl;
  } 
  void writePush(int seg, int idx) {
    outfile << "push " + segtos(seg) + " " + to_string(idx) << endl;
    cout << "push " + segtos(seg) + " " + to_string(idx) << endl;
  }
  void writePop(int seg, int idx) {
    outfile << "pop " + segtos(seg) + " " + to_string(idx) << endl;
    cout << "pop " + segtos(seg) + " " + to_string(idx) << endl;
  }
  void writePopToken(string token) {
    if (stclass->kindof(token) != NONE) {
      outfile << "pop " + segtos(stclass->kindof(token)) + " " + to_string(stclass->indexof(token)) << endl;
      cout << "pop " + segtos(stclass->kindof(token)) + " " + to_string(stclass->indexof(token)) << endl;
    }
    else if (stsub->kindof(token) != NONE) {
      outfile << "pop " + segtos(stsub->kindof(token)) + " " + to_string(stsub->indexof(token)) << endl;
      cout << "pop " + segtos(stsub->kindof(token)) + " " + to_string(stsub->indexof(token)) << endl;
    }
    else
      cout<< "token type not found"<<endl;
  }
  void writeArithmetic(string next) {
     if (next == "+") {
       outfile << "add" << endl;
       cout << "add" << endl;
     } else if (next == "-") {
       outfile << "sub" << endl;
       cout << "sub" << endl;
     } else if (next == "*") {
       outfile <<  "call Math.multiply 2" << endl;
       cout <<  "call Math.multiply 2" << endl;
     } else if (next == "/") {
       outfile << "call Math.divide 2" << endl;
       cout << "call Math.divide 2" << endl;
     }else if (next == "&") { 
       outfile << "and" << endl;
       cout << "and" << endl;
     }else if (next == "|") {
       outfile << "or" << endl;
       cout << "or" << endl;
     }else if (next == "<") {
       outfile << "lt" << endl;
       cout << "lt" << endl;
     }else if (next == ">") {
       outfile << "gt" << endl;
       cout << "gt" << endl;
     }else if (next == "=") { 
       outfile << "eq" << endl;
       cout << "eq" << endl;
     }else if (next == "~") {
       outfile << "not" << endl;
       cout << "not" << endl;
     }else{ 
       outfile << next << endl;
       cout << next << endl;
     }
  }
  void writeLabel(string label) {
  outfile << "label " + label << endl;
  cout << "label " + label << endl;
  }
  void writeCall(string name, int nArgs) {
    outfile << "call " + name + " " + to_string(nArgs) << endl;
    cout << "call " + name + " " + to_string(nArgs) << endl;
  }
  void writeReturn() {
    outfile << "return" << endl;
    cout << "return" << endl;
  }
  void writeFunc(string name, int nLocals) {
    outfile << "function " + name + " " + to_string(nLocals) << endl;
    cout << "function " + name + " " + to_string(nLocals) << endl;
  }
  void writeIf(string label) {
    outfile << "if-goto " + label << endl;  
    cout << "if-goto " + label << endl;  
  }
  void writeGo(string label) {
    outfile << "goto " + label << endl;
    cout << "goto " + label << endl;
  }
private:
  ofstream outfile;
  SymbolTable* stclass;
  SymbolTable* stsub;
};

class Parser {
public:
Parser(VMWriter* writer, Tokenizer* tkz, SymbolTable* clas, SymbolTable* sub) {
   this->tkz = tkz;   stclass = clas;
   stsub = sub; w = writer;
   labelCnt = 0;
};

  void compileClass() {
    if (tkz->advance() != IDENTIFIER)
      return;
    //className
    className = tkz->getToken();
    stclass->define(className, className, 0);
    tkz->advance();
    tkz->advance();
    while (compileClassVarDec()) {}
    while (compileSubroutine()) {}
  }
  bool compileClassVarDec() {
    // static | field
    if (tkz->tokenType() != KEYWORD)
      return false;
    int kw = tkz->getKeyword();
    if (kw == STATIC || kw == FIELD) {} 
    else
      return false;
    // type
    tkz->advance();
    string tp = types();
    // varName
    tkz->advance();
    if (tkz->tokenType() == IDENTIFIER) {
        stclass->define(tkz->getToken(), tp, kw);
    }
    else
      return false;
    // (, varName)*
    varNames(stclass, tp, kw);
    // ;
    tkz->advance();
    return true;
  }
  string types() {
    if (tkz->tokenType() == KEYWORD) {
      int tp = tkz->getKeyword(); 
      if (tp == INT || tp == CHAR || tp == BOOLEAN) {}
      else
        return "";
    } else if (tkz->tokenType() == IDENTIFIER) {
    }
    else
      return "";
    return tkz->getToken();
  }
  void varNames(SymbolTable* st, string tp, int kind) {
    tkz->advance();
    while (tkz->getToken() == ",") {
      tkz->advance();
      if (tkz->tokenType() == IDENTIFIER) {
	st->define(tkz->getToken(), tp, kind);
      }
      else
        return;
     tkz->advance();
    }
  }
  bool compileSubroutine() {
    if (tkz->tokenType() != KEYWORD)
      return false;
    // subroutineDec
    int kw = tkz->getKeyword();
    stsub->clear();
        tkz->advance();
    string returnType;
    if (tkz->getToken() == "void")
      returnType = "void";
    else
      returnType = types();
    tkz->advance();
    // subroutineName
    string subroutineName = className + "." + tkz->getToken();
    if (returnType == "void") {
      voidSubroutines.insert(subroutineName);
    }
    if (kw == METHOD) {
      methodSubroutines.insert(subroutineName);
      stsub->define("this", className, ARG);
    }
    tkz->advance();
    //oprint("symbol", "(");
    tkz->advance();
    compileParameterList();
    tkz->advance();
    // subroutineBody
    tkz->advance();
    while (tkz->getToken() == "var")
      compileVarDec();
    w->writeFunc(subroutineName, stsub->varCount(LOCAL));
    if (kw == CONSTRUCTOR) {
      int size = stclass->varCount(FIELD);
      w->writePush(CONST, size);
      w->writeCall("Memory.alloc", 1);
      w->writePop(POINTER, 0);
    } else if (kw == METHOD) {
      w->writePush(ARG, 0);
      w->writePop(POINTER, 0);
    }
    compileStatements();
    tkz->advance();
    return true;
  }
  void compileParameterList() {
    if (tkz->tokenType() != KEYWORD && tkz->tokenType() != IDENTIFIER){
      return;
    }
    string tp = types();
    tkz->advance();
    stsub->define(tkz->getToken(), tp, ARG);
    tkz->advance();
    while (tkz->getToken() == ",") {
      tkz->advance();
      tp = types();
      tkz->advance();
      if (tkz->tokenType() != IDENTIFIER)
        return;
      stsub->define(tkz->getToken(), tp, ARG);
     tkz->advance();
    }
  }
  void compileVarDec() {
    //oprint("keyword", "var");
    tkz->advance();
    string tp = types();
    tkz->advance();
    stsub->define(tkz->getToken(), tp, LOCAL);
    varNames(stsub, tp, LOCAL);
    //oprint("symbol", ";");
    tkz->advance();
  }
  void compileStatements() {
    int type = tkz->getKeyword();
    if (type != STATEMENTS) {
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
  }
  void compileDo() {
    //oprint("keyword", "do");
    tkz->advance();
    string currentToken = tkz->getToken();
    tkz->advance();
    string name = compileSubroutineCall(currentToken);
    w->writePop(TEMP, 0);
    tkz->advance();
    //oprint("symbol", ";");
  }
  void compileLet() {
      tkz->advance();
      string leftVar;
      bool isArray = false;
      if (tkz->tokenType() == IDENTIFIER)
        leftVar = tkz->getToken(); 
      else
        return;
      tkz->advance();
      if (tkz->getToken() == "[") {
        isArray = true;
        w->writePushToken(leftVar);
        //oprint("symbol", "[");
        tkz->advance();
	compileExpression();
	w->writeArithmetic("+");
        //oprint("symbol", "]");
        tkz->advance();
      }
      //oprint("symbol", "=");
      tkz->advance();
      compileExpression();
      //oprint("symbol", ";");
      if (isArray) {
      w->writePop(TEMP, 0);
      w->writePop(POINTER, 1);
      w->writePush(TEMP, 0);
      w->writePop(THAT, 0);
      } else
        w->writePopToken(leftVar);
  }
  void compileWhile() {
    //oprint("keyword", "while");
    string l1 = "lbl_" + to_string(labelCnt) + "_lbl";
    w->writeLabel(l1);
    labelCnt++;
    tkz->advance();
    //oprint("symbol", "(");
    tkz->advance();
    compileExpression();
    w->writeArithmetic("~");
    //oprint("symbol", ")");
    tkz->advance();
    string l2 = "lbl_" + to_string(labelCnt) + "_lbl";
    w->writeIf(l2);
    labelCnt++;
    tkz->advance();
    compileStatements();
    w->writeGo(l1);
    w->writeLabel(l2);
  }
  void compileReturn() {
    tkz->advance();
    if (tkz->getToken() != ";") {
      compileExpression();
    }else{
      w->writePush(CONST, 0);
    }
    w->writeReturn();
    //oprint("symbol", ";");
  }
  void compileIf() {
      //oprint("keyword", "if");
      tkz->advance();
      tkz->advance();
      compileExpression();
      w->writeArithmetic("~");
      string l1 = "lbl_" + to_string(labelCnt) + "_lbl";
      w->writeIf(l1);
      labelCnt++;
      tkz->advance();
      tkz->advance();
      compileStatements();
      tkz->advance();
      if (tkz->getToken() == "else") {
        tkz->advance();
        string l2 = "lbl_" + to_string(labelCnt) + "_lbl";
        labelCnt++;
	w->writeGo(l2);
	w->writeLabel(l1);
        tkz->advance();
        compileStatements();
	w->writeLabel(l2);
        tkz->advance();
      } else
	w->writeLabel(l1);
  }
  void compileExpression() {
    compileTerm();
    string next = tkz->getToken();
    while (next == "+" || next == "-" || next == "*" || next == "/" || next ==
    "&" || next == "|" || next == "<" || next == ">" || next == "=") {
      tkz->advance();
      compileTerm();
      w->writeArithmetic(next);
      next = tkz->getToken();
    }
  }
  void compileTerm() {
    string currentToken = tkz->getToken();
    if (tkz->tokenType() == INT_CONST)
      w->writePush(CONST, std::stoi(currentToken));
    else if (tkz->tokenType() == STRING_CONST) {
      string sc = tkz->getToken();
      sc = sc.substr(1, sc.length()-2);
      w->writePush(CONST, sc.length());
      w->writeCall("String.new", 1);
      for (int i = 0; i < sc.length(); i++) {
        w->writePush(CONST, int(sc[i]));
	w->writeCall("String.appendChar", 2);
      }
    }
    else if (tkz->tokenType() == KEYWORD) {
      int getkw = tkz->getKeyword();
      if (getkw == FALSE || getkw == K_NULL) {
        w->writePush(CONST, 0);
      } else if (getkw == TRUE) {
        w->writePush(CONST, 1);
        w->writeArithmetic("neg");
      } else if (getkw == THIS) 
        w->writePushToken("this");
    } else if (currentToken == "-" || currentToken == "~") {
      tkz->advance();
      compileTerm();
      if (currentToken == "-")
        w->writeArithmetic("neg");
      else
        w->writeArithmetic(currentToken);
      return;
    } else if (currentToken == "(") {
      tkz->advance();
      compileExpression();
    } else {
      tkz->advance();
      string next = tkz->getToken(); 
      if (next == "(" || next == ".") {
        compileSubroutineCall(currentToken);
      }
      else if (next == "[") {
        // varName[]
        //oprint("symbol", "[");
        tkz->advance();
	w->writePushToken(currentToken);
	compileExpression();
	w->writeArithmetic("+");
	w->writePop(POINTER, 1);
	w->writePush(THAT, 0);
        //oprint("symbol", "]");
      } else {
        // varName
	w->writePushToken(currentToken);
	return;
      }
    }
    tkz->advance();
  }
  string compileSubroutineCall(string name) {
    string subroutineName;
    int nargs = 0;
    if (tkz->getToken() == "(") {
      subroutineName = className + "." + name;
      if (methodSubroutines.count(subroutineName) != 0) {
        nargs = 1;
	w->writePush(POINTER, 0);
      }
      tkz->advance();
      nargs += compileExpressionList(); 
      w->writeCall(subroutineName, nargs);
    } else {
      tkz->advance();
      if (stclass->kindof(name) != NONE)
        subroutineName = stclass->typeof(name) + "." + tkz->getToken();
      else 
        subroutineName = stsub->typeof(name) + "." + tkz->getToken();
      if (methodSubroutines.count(subroutineName) != 0) {
        nargs = 1;
	w->writePushToken(name);
      }
      tkz->advance();
      //oprint("symbol", "(");
      tkz->advance();
      nargs += compileExpressionList(); 
      w->writeCall(subroutineName, nargs);
      //oprint("symbol", ")");
    } 
    return subroutineName;
  }
  int compileExpressionList() {
    if (tkz->getToken() == ")") {
      return 0;
    }
    int cnt = 1;
    compileExpression();
    while (tkz->getToken() == ",") {
      tkz->advance();
      compileExpression();
      cnt++;
    }
    return cnt;
  }
private:
  ofstream outfile; 
  VMWriter* w;
  Tokenizer* tkz;
  SymbolTable* stclass;
  SymbolTable* stsub;
  string className;
  int labelCnt;
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
	if (tmp.find(".") == string::npos)
	  continue;
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
  
  voidSubroutines.insert("Output.printInt");
  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < inputList.size(); i++) {
    cout<<"file: "<<endl;
    string outname = inputList[i].substr(0, inputList[i].find("."));
    SymbolTable classSB, subSB;
    Tokenizer tkz(inputList[i]);
    VMWriter writer(outname, &classSB, &subSB);
    Parser    parser(&writer, &tkz, &classSB, &subSB);
    tkz.advance();
    parser.compileClass();	      
    }
  }
  return 0;
}
