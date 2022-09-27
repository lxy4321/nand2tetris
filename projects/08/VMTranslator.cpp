#include <iostream>
#include <string>
#include <algorithm>
#include <climits>
#include <iterator>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem> 
#include <stack>
#include <unordered_map>
using namespace std;
namespace fs = std::filesystem;
#define C_ARITHMETIC 1
#define C_PUSH 2
#define C_POP 3
#define C_LABEL 4
#define C_GOTO 5
#define C_IF 6
#define C_FUNCTION 7
#define C_RETURN 8
#define C_CALL 9

class Parser {
public:
  Parser(string name){input.open(name);};
  ~Parser() {input.close();}

  istream& hasMoreCommands(){
    return getline(input, currentL);
  }
  bool advance(){
    currentL.erase(std::find(currentL.begin(), currentL.end(), '\r'), currentL.end());
    currentL = currentL.substr(0, currentL.find("//"));
    string isEmpty = currentL;
    isEmpty.erase(std::find(isEmpty.begin(), isEmpty.end(), ' '), isEmpty.end());
    if (isEmpty.length() == 0)
      return false;
    return true;
  }
  int commandType(){
    int i = 0;
    cmd.clear();
    for (int idx = 0; idx < 3; idx++) {
      while (i < currentL.length() && currentL[i] == ' ')
        i++;
      string tmp = "";
      while (i < currentL.length() && currentL[i] != ' ') {
        tmp = tmp + currentL[i++];
      }
      if (tmp != "")
        cmd.push_back(tmp);
    }
    if (cmd[0] == "return")
      return C_RETURN;
    if (cmd.size() == 1)
      return C_ARITHMETIC;
    if (cmd[0] == "push")
      return C_PUSH;
    if (cmd[0] == "pop")
      return C_POP;
    if (cmd[0] == "label")
      return C_LABEL;
    if (cmd[0] == "call")
      return C_CALL;
    if (cmd[0] == "function")
      return C_FUNCTION;
    if (cmd[0] == "goto")
      return C_GOTO;
    if (cmd[0] == "if-goto")
      return C_IF;
    return -1;
  }
  vector<string> getCommand() {return cmd;}
private:
  ifstream input;
  string currentL;
  vector<string> cmd;
};

class CodeWriter {
public:
CodeWriter(string outname) {outfile.open(outname + ".asm");
  this->outname = outname;
  cnt = 0;
  retAddrCnt = 0;
  staticCnt = 0;
  m["gt"] = "JGT";
  m["lt"] = "JLT";
  m["local"] = "LCL";
  m["argument"] = "ARG";
  m["this"] = "THIS";
  m["that"] = "THAT";
};
~CodeWriter() {outfile.close();};

void writeInit() {
  outfile << "@256" << endl;
  outfile << "D=A" << endl;
  outfile << "@SP" << endl;
  outfile << "M=D" << endl;
  vector<string> initCmd;
  initCmd.push_back("call");
  initCmd.push_back("Sys.init");
  initCmd.push_back("0");
  writeCall(initCmd);
}
void writeArithmetic(vector<string>& cmd) {
  if (cmd[0] == "not" || cmd[0] == "neg") {
    if (cmd[0] == "neg") {
      outfile << "@0" << endl;
      outfile << "D=A" << endl;
    }
    outfile << "@SP" << endl;
    outfile << "A=M" << endl;
    outfile << "A=A-1" << endl;
    if (cmd[0] == "not")
      outfile << "M=!M" << endl;
    else {
      outfile << "M=D-M" << endl;
    }
  } else {
    outfile << "@SP" << endl;
    outfile << "AM=M-1" << endl;
    outfile << "D=M" << endl;
    outfile << "A=A-1" << endl;
    if (cmd[0] == "gt" || cmd[0] == "lt" ||  cmd[0] == "eq") {
	    outfile << "D=M-D" << endl;
	    outfile << "@isTrue" + std::to_string(cnt) << endl;
	    string jump = "JEQ";
	    if (cmd[0] == "gt")
	      jump = "JGT";
	    else if (cmd[0] == "lt")
	      jump = "JLT";
	    outfile << "D;" + jump << endl;
	    outfile << "@SP" << endl;
	    outfile << "A=M-1" << endl;
	    outfile << "M=0" << endl;
	    outfile << "@exit" + to_string(cnt) << endl;
	    outfile << "0; JMP" << endl;
	    outfile << "(isTrue" + std::to_string(cnt) + ")" << endl;
	    outfile << "@SP" << endl;
	    outfile << "A=M-1" << endl;
	    outfile << "M=-1" << endl;
	    outfile << "(exit" + to_string(cnt) + ")" << endl;
	    cnt++;
    } else {
      if (cmd[0] == "add")
        outfile << "D=M+D" << endl;
      else if (cmd[0] == "sub")
        outfile << "D=M-D" << endl;
      else if (cmd[0] == "and")
        outfile << "D=M&D" << endl;
      else if (cmd[0] == "or")
        outfile << "D=M|D" << endl;
      outfile << "@SP" << endl;
      outfile << "A=M-1" << endl;
      outfile << "M=D" << endl;
    }
  }
}

void writePushPop(vector<string>& cmd) {
  if (cmd[1] != "constant") {
    if (cmd[1] == "static") {
      outfile << "@static" + to_string(staticCnt) + "." + cmd[2] << endl;
      outfile << "D=A" << endl;
      staticCnt++;
    } else if (cmd[1] == "pointer") {
      if (cmd[2] == "0") 
        outfile << "@THIS" << endl;
      else
        outfile << "@THAT" << endl;
      outfile << "D=A" << endl;
    } else {
      if (cmd[1] == "temp") {
          outfile << "@5" << endl;
	  outfile << "D=A" << endl;
      } else {
	  outfile << "@" + m[cmd[1]] << endl;
          outfile << "D=M" << endl;
      }
      outfile << "@" + cmd[2] << endl;
      outfile << "D=D+A" << endl;
    }
    outfile << "@R13" << endl;
    outfile << "AM=D" << endl;
    outfile << "D=M" << endl;
  } else {
    outfile << "@" + cmd[2] << endl;
    outfile << "D=A" << endl;
  }
  if (cmd[0] == "push") {
    pushCode();
  } else {
   outfile << "@SP" << endl; 
   outfile << "M=M-1" << endl;
   outfile << "@SP" << endl; 
   outfile << "A=M" << endl; 
   outfile << "D=M" << endl; 
   outfile << "@R13" << endl;
   outfile << "A=M" << endl;
   outfile << "M=D" << endl;
  }
} 
void pushCode() {
   outfile << "@SP" << endl; 
   outfile << "A=M" << endl; 
   outfile << "M=D" << endl; 
   outfile << "@SP" << endl; 
   outfile << "M=M+1" << endl; 

}
void writeLabel(vector<string>& cmd) {
  outfile << "(" + cmd[1] + ")" << endl;
}

void writeCall(vector<string>& cmd) {
  string retAddr = cmd[1] + "$ret" + to_string(retAddrCnt);
  vector<string> pushAddrCmd;
  pushAddrCmd.push_back("push");
  pushAddrCmd.push_back("constant");
  // push return-address
  outfile << "@" + retAddr << endl; 
  outfile << "D=A" << endl; 
  pushCode();
  // push LCL
  outfile << "@LCL" << endl; 
  outfile << "D=M" << endl; 
  pushCode(); 
  // push ARG 
  outfile << "@ARG" << endl; 
  outfile << "D=M" << endl; 
  pushCode(); 
  // push THIS
  outfile << "@THIS" << endl; 
  outfile << "D=M" << endl; 
  pushCode(); 
  // push THAT
  outfile << "@THAT" << endl; 
  outfile << "D=M" << endl; 
  pushCode(); 
  // ARG = SP-n-5 
  outfile << "@5" <<endl;
  outfile << "D=A" <<endl;
  outfile << "@" + cmd[2] <<endl;
  outfile << "D=D+A" <<endl;
  outfile << "@SP" <<endl;
  outfile << "D=M-D" <<endl;
  outfile << "@ARG" <<endl;
  outfile << "M=D" <<endl;
  // LCL = SP
  outfile << "@SP" <<endl;
  outfile << "D=M" <<endl;
  outfile << "@LCL" <<endl;
  outfile << "M=D" <<endl;
  // goto function
  outfile << "@" + cmd[1] <<endl;
  outfile << "0;JMP" <<endl;
  // (return address)
  outfile << "(" + retAddr + ")" <<endl;
  retAddrCnt++;
}
void writeFunction(vector<string>& cmd) {
  outfile << "(" + cmd[1] + ")" << endl;
  int k = std::stoi(cmd[2]);
  vector<string> pushCmd;
  pushCmd.push_back("push");
  pushCmd.push_back("constant");
  pushCmd.push_back("0");
  while (k--) {
    writePushPop(pushCmd); 
  }
}
void writeReturn() {
  // FRAME=LCL
  outfile << "@LCL" << endl;
  outfile << "D=M" << endl;
  outfile << "@FRAME" << endl;
  outfile << "M=D" << endl;
  // RET=*(FRAME-5)
  outfile << "@5" << endl;
  outfile << "A=D-A" << endl;
  outfile << "D=M" << endl;
  outfile << "@RET" << endl;
  outfile << "M=D" << endl;
  // *ARG=pop()
  outfile << "@SP" << endl;
  outfile << "AM=M-1" << endl;
  outfile << "D=M" << endl;
  outfile << "@ARG" << endl;
  outfile << "A=M" << endl;
  outfile << "M=D" << endl;
  // SP=ARG+1
  outfile << "@ARG" << endl;
  outfile << "D=M+1" << endl;
  outfile << "@SP" << endl;
  outfile << "M=D" << endl;
  // THAT=*(FRAME-1)
  outfile << "@FRAME" << endl;
  outfile << "AM=M-1" << endl;
  outfile << "D=M" << endl;
  outfile << "@THAT" << endl;
  outfile << "M=D" << endl;
  outfile << "@FRAME" << endl;
  outfile << "AM=M-1" << endl;
  outfile << "D=M" << endl;
  outfile << "@THIS" << endl;
  outfile << "M=D" << endl;
  outfile << "@FRAME" << endl;
  outfile << "AM=M-1" << endl;
  outfile << "D=M" << endl;
  outfile << "@ARG" << endl;
  outfile << "M=D" << endl;
  outfile << "@FRAME" << endl;
  outfile << "AM=M-1" << endl;
  outfile << "D=M" << endl;
  outfile << "@LCL" << endl;
  outfile << "M=D" << endl;
  // goto RET
  outfile << "@RET" << endl;
  outfile << "A=M" << endl;
  outfile << "0;JMP" << endl;
}
void writeGoto(vector<string>& cmd) {
  outfile << "@" + cmd[1] << endl;
  outfile << "0;JMP" << endl;
}
void writeIf(vector<string>& cmd) {
  outfile << "@SP" << endl;
  outfile << "M=M-1" << endl;
  outfile << "A=M" << endl;
  outfile << "D=M" << endl;
  outfile << "@" + cmd[1] << endl;
  outfile << "D;JNE" << endl;
}

private:
  ofstream outfile; 
  string outname;
  int cnt;
  unordered_map<string, string> m;
  int retAddrCnt;
  int staticCnt;
};

int main(int argc, char** argv) {
  string inName = argv[1];
  vector<string> inputList;
  const fs::path path(inName);
  std::error_code ec;
  string outname;
  if (fs::is_regular_file(path, ec))
    outname = inName.substr(0, inName.find("."));
  else if (fs::is_directory(path, ec)) {
    string filename = "";
    int i = inName.length() -1;
    if (inName[i] == '/') i--;
    while (i >= 0 && inName[i] != '/')
      filename = inName[i--] + filename;
    if (inName[inName.length()-1] == '/')
      outname = inName + filename;
    else
      outname = inName + '/' + filename;
  }
  CodeWriter cw(outname);
  if (fs::is_regular_file(path, ec))
      inputList.push_back(inName);
  else if (fs::is_directory(path, ec)) {
      cw.writeInit();
      DIR *dir = opendir(inName.c_str());
      struct dirent *entry;
      while ((entry = readdir(dir)) != NULL) {
        string tmp = entry->d_name;
        tmp = tmp.substr(tmp.find("."));
	if (tmp == ".vm")
          if (inName[inName.length()-1] == '/')
            inputList.push_back(inName + entry->d_name);
	  else
            inputList.push_back(inName + "/" + entry->d_name);
      }
      closedir(dir);
  }
  
  for (int i = 0; i < inputList.size(); i++) {
    Parser parser(inputList[i]);
  // first pass
  while (parser.hasMoreCommands()) {
    if (!parser.advance())
      continue;
    int type = parser.commandType();
    vector<string> cmd = parser.getCommand();
    switch(type) {
      case C_ARITHMETIC:
        cw.writeArithmetic(cmd);
	break;
      case C_PUSH:
        cw.writePushPop(cmd);
	break;
      case C_POP:
        cw.writePushPop(cmd);
	break;
      case C_LABEL:
        cw.writeLabel(cmd);
	break;
      case C_GOTO:
        cw.writeGoto(cmd);
	break;
      case C_IF:
        cw.writeIf(cmd);
	break;
      case C_FUNCTION:
        cw.writeFunction(cmd);
	break;
      case C_RETURN:
        cw.writeReturn();
	break;
      case C_CALL:
        cw.writeCall(cmd);
	break;
      default:
        cout << "unrecognized type" << endl;
	break;
    }
    }
  }
  return 0;
}
