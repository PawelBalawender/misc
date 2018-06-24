#include <stack>  // std::stack; loop handling
#include <vector>  // std::vector; program's memory array handling
#include <string>  // std::string; read the source code
#include <fstream>  // std::ifstream; read the source code
#include <iostream>
#include <sstream>  // std::stringstream, read the source code
/*
 * maximal code length: 64 bytes
 * maximal number of loops: 64
 * program array (memory) size: 32768 cells
 * each cell's range: int8_t; signed char
 */

const uint8_t max_loop_number = 64;
const uint16_t arr_size = 32768;
std::vector<int8_t> arr (arr_size, 0);

int main(int argc, char* argv[]) {  
  // read the source code given
  std::ifstream source_file (argv[1]);
  std::stringstream buffer;
  buffer << source_file.rdbuf();
  
  std::string code = buffer.str();
  
  std::stack<char*> loops;
  
  char* exec_pntr = &code[0];
  int8_t* arr_pntr = &arr[0];
  
  while (*exec_pntr != '\0') {
    switch (*exec_pntr) {
	  case '>':
	    arr_pntr++;
		break;
	  case '<':
	    arr_pntr--;
		break;
	  case '+':
	    (*arr_pntr)++;
		break;
	  case '-':
	    (*arr_pntr)--;
		break;
	  case '.':
	    putchar(*arr_pntr);
		break;
	  case ',':
	    *arr_pntr = getchar();
		break;
	  case '[':
	    if (*arr_pntr != 0) {
		  // save position and enter the loop
		  loops.push(exec_pntr);
		} else {
		  // jump to the enclosing ]; incrementation after
		  // the switch clause will pass it
		  while (*exec_pntr != ']') exec_pntr++;
		}
		break;
	  case ']':
	    exec_pntr = loops.top();
		// decrement, cause it will be incremented in a moment
		// and we want it where it is to provide condition check at the [ sign
		exec_pntr--;
		break;
	  default:
	    break;
	}
	exec_pntr++;
  }
  return 0;
}
