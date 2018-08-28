#include <string>  // std::string; read the source code
#include <fstream>  // std::ifstream; read the source code
#include <map>  // std::map; translation table
#include <set>
#include <iostream>
#include <sstream>  // std::stringstream, read the source code


int main(int argc, char* argv[]) {  
  // read the source code from the file given in argv
  std::ifstream source_file (argv[1]);
  std::stringstream buffer;
  buffer << source_file.rdbuf();
  std::string bf_code = buffer.str();
  
  std::map<char, std::string> dict = {
	  {'+', "(*p)++;"},
	  {'-', "(*p)--;"},
	  {'>', "p++;"},
	  {'<', "p--;"},
	  {'.', "putchar(*p);"},
	  {',', "*p = getchar();"},
	  {'[', "while (*p) {"},
	  {']', "}"}
  };
  
  std::set<char> keys = {'+', '-', '<', '>', '.', ',', '[', ']'};
  std::string cpp_code = "#include<iostream>\n";
  cpp_code += "int main() {\n";
  cpp_code += "uint8_t arr[256] = {0};\n";
  cpp_code += "uint8_t* p = &arr[0];\n";
  char c;
  for (int i=0; i < bf_code.length(); i++) {
	  c = bf_code[i];
	  if (keys.find(c) != keys.end()) cpp_code += dict[c] + '\n';
  }
  cpp_code += "return 0;}";
  
  std::ofstream out("_trans.cpp");
  out << cpp_code;
  out.close();
  
  system("g++ _trans.cpp -o _bf.exe");
  system("_bf.exe");
  return 0;
}
