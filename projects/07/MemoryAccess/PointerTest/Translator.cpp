#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
using namespace std;
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
  ~Parser(){input.close();};

  bool hasMoreCommands(){
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
    if (cmd.size() == 1)
      return C_ARITHMETIC;
    if (cmd[0] == "push")
      return C_PUSH;
    if (cmd[0] == "pop")
      return C_POP;
    return C_CALL;
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
/*  outfile << "@256" << endl;
  outfile << "D=A" << endl;
  outfile << "@SP" << endl;
  outfile << "M=D" << endl;
  outfile << "@1500" << endl;
  outfile << "D=A" << endl;
  outfile << "@LCL" << endl;
  outfile << "M=D" << endl;
  outfile << "@2500" << endl;
  outfile << "D=A" << endl;
  outfile << "@ARG" << endl;
  outfile << "M=D" << endl;
*/ 
  m["gt"] = "JGT";
  m["lt"] = "JLT";
  m["local"] = "LCL";
  m["argument"] = "ARG";
  m["this"] = "THIS";
  m["that"] = "THAT";
};
~CodeWriter() {outfile.close();};

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
      outfile << "@" + outname + "." + cmd[2] << endl;
      outfile << "D=A" << endl;
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
   outfile << "@SP" << endl; 
   outfile << "A=M" << endl; 
   outfile << "M=D" << endl; 
   outfile << "@SP" << endl; 
   outfile << "M=M+1" << endl; 
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
private:
  ofstream outfile; 
  string outname;
  int cnt;
  unordered_map<string, string> m;
};

int main(int argc, char** argv) {
  string inName = argv[1];
  Parser parser(inName);
  string outname = inName.substr(0, inName.find("."));
  CodeWriter cw(outname);

  // first pass
  while (parser.hasMoreCommands()) {
    if (!parser.advance())
      continue;
    int type = parser.commandType();
    vector<string> cmd = parser.getCommand();
    if (type == C_ARITHMETIC)
      cw.writeArithmetic(cmd);
    else if (type == C_PUSH || type == C_POP)
      cw.writePushPop(cmd);

  }
  return 0;
}
