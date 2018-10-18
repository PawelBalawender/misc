/** Prosty kalkulator z czterama podstawowymi operacjami
 * Kalkulator obsługuje:
 *  -dodawanie
 *  -odejmowanie
 *  -mnożenie
 *  -dzielenie
 */
#include <iostream>

int main()
{
  double res;
  int a, b;
  char c;
  
  std::cout << "Czynnik 1:\n >";
  std::cin >> a;
  std::cout << "Czynnik 2:\n >";
  std::cin >> b;
  std::cout << "Dzialanie [+ - * /]:\n >";
  std::cin >> c;
  
  switch (c) {
      case '+':
          res = a+b;
          break;
      case '-':
          res = a-b;
          break;
      case '*':
          res = a*b;
          break;
      case '/':
          res = (double)a / b;
          break;
      default:
          break;
  }
  
  std::cout << res << std::endl;
  return 0;
}
