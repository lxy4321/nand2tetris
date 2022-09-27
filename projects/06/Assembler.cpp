#include <iostream>
#include <unordered_map>
#include <fstream>
using namespace std;
#define A_COMMAND 1
#define C_COMMAND 2
#define L_COMMAND 3

class Parser {
public:
  Parser(string name){input.open(name);};
  ~Parser(){input.close();};

  bool hasMoreCommands(){
    return getline(input, currentL);
  }
  bool advance(){
    currentL.erase(std::find(currentL.begin(), currentL.end(), '\r'), currentL.end());
    currentL.erase(remove(currentL.begin(), currentL.end(), ' '), currentL.end());
    command = currentL.substr(0, currentL.length()-1);
    command = currentL.substr(0, currentL.find("//"));
    mydest = "";
    mycomp = "";
    myjump = "";
    mysymbol = "";
    if (command.length() == 0)
      return false;
    return true;
  }
  int commandType(){
    switch(command[0]) {
      case '@':
        mysymbol = command.substr(1);
        return A_COMMAND;
      case '(':
        for (int i = 1; i < command.length(); i++) {
	  if (command[i] == ')')
	    break;
	  mysymbol = mysymbol + command[i];
	}
        return L_COMMAND;
      default:
        bool hasJump = false;
        for (int i =0; i < command.length(); i++) {
	  if (command[i] == '='){
	    mydest = mycomp;
	    mycomp = "";
	    continue;
	  }
	  if (command[i] == ';') {
            hasJump = true;
	    i++;
	  }
	  if (hasJump)
	    myjump = myjump + command[i];
	  else
	    mycomp = mycomp + command[i];
	}
        return C_COMMAND;
    }
  }
  string comp() {return mycomp;}
  string dest() {return mydest;}
  string jump() {return myjump;}
  string symbol() {return mysymbol;}
  string getCommand() {return command;}
private:
  ifstream input;
  string currentL;
  string command;
  string mycomp;
  string myjump;
  string mydest;
  string mysymbol;
};

class Code {
public:
  string dest(string d){
    string out = "000";
    if (d.find("A") != string::npos)
      out[0] = '1';
    if (d.find("D") != string::npos)
      out[1] = '1';
    if (d.find("M") != string::npos)
      out[2] = '1';
    return out;
  }
  string jump(string j) {
    if (j == "JGT")
        return "001";
    if (j == "JEQ")
        return "010";
    if (j == "JGE")
        return "011";
    if (j == "JLT")
        return "100";
    if (j == "JNE")
        return "101";
    if (j == "JLE")
        return "110";
    if (j == "JMP")
        return "111";
    return "000";
  }
  string comp(string c) {
    string a = c.find("M") != string::npos ? "1" : "0";
    if (c.find("|") != string::npos)
      return a + "010101";
    if (c.find("&") != string::npos)
      return a + "000000";
    if (c == "0")
      return a + "101010";
    if (c == "1")
      return a + "111111";
    if (c == "-1")
      return a + "111010";
    if (c == "D")
      return a + "001100";
    if (c.compare("A") == 0 || c == "M")
      return a + "110000";
    if (c == "!D")
      return a + "001111";
    if (c == "!A" || c == "!M")
      return a + "110001";
    if (c == "-D")
      return a + "001111";
    if (c == "-A" || c == "-M")
      return a + "110011";
    if (c == "D+1")
      return a + "011111";
    if (c == "A+1" || c == "M+1")
      return a + "110111";
    if (c == "D-1")
      return a + "001110";
    if (c == "A-1" || c == "M-1")
      return a + "110010";
    if (c == "D+A" || c == "D+M")
      return a + "000010";
    if (c == "D-A" || c == "D-M")
      return a + "010011";
    if (c == "A-D" || c == "M-D")
      return a + "000111";
    return "invalid comp";
  }
};
class SymbolTable {
public:
  SymbolTable(){
    m["SP"] = 0;
    m["LCL"] = 1;
    m["ARG"] = 2;
    m["THIS"] = 3;
    m["THAT"] = 4;
    m["SCREEN"] = 16384;
    m["KBD"] = 24576;
    for (int i = 0; i < 16; i++){
      string sy = "R" + to_string(i);
      m[sy] = i;
    }
  };
  void addEntry(string symbol, int address){m[symbol] = address;}
  bool contains(string symbol) {return m.count(symbol);}
  int getAddress(string symbol) {return m[symbol];}
private:
  unordered_map<string, int> m;
};

int main(int argc, char** argv) {
  string inName = argv[1];
  Parser fparser(inName), sparser(inName);
  Code code;
  SymbolTable st;
  int rom = 0;
  int ram = 16;
  string outname = inName.substr(0, inName.find(".")) + ".hack";
  ofstream outfile(outname);

  // first pass
  while (fparser.hasMoreCommands()) {
    if (!fparser.advance())
      continue;
    int type = fparser.commandType();
    if (type == L_COMMAND) {
      string symbol = fparser.symbol();
      st.addEntry(symbol, rom);
    } else 
      rom++;
  }
 
  // second pass
  while (sparser.hasMoreCommands()) {
    if (!sparser.advance())
      continue;
    string symbol = "";
    string out = "";
    int type = sparser.commandType();
    if (type == A_COMMAND) {
      symbol = sparser.symbol(); 
      string val;
      if (symbol[0] > '9' || symbol[0] < '0') {
        if (!st.contains(symbol)) {
	    st.addEntry(symbol, ram);
	    ram++;
        }
        out = bitset<16>(st.getAddress(symbol)).to_string();
      }
      else
        out = bitset<16>(std::stoi(symbol)).to_string();
    } else if (type == L_COMMAND)  
      continue;
    else {
      string c = sparser.comp();
      string d = sparser.dest();
      string j = sparser.jump();
      string cc = code.comp(c);
      string dd = code.dest(d);
      string jj = code.jump(j);
      out = "111" + cc + dd + jj;
    }
    outfile << out << endl; 
  }
  outfile.close();
  return 0;
}
