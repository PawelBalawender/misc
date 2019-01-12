#include <iostream>
#include <typeinfo>
#include <cassert>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>
#include <type_traits>
#include <algorithm>

using namespace std;

/// Compare two floating-point numbers
/** Function copy-pasted from the documentation:
 *  http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 */
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp) {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

/// Implementation of a 2D array
/** It's based on std::vector
 *  The extensions are methods used to perform math operations on matrices,
 *  such as to calculate the determinant or to solve the equation system
 *  that the matrix is created from.
 *  The class is totally unsafe in respect to exposure on undefined behaviour,
 *  exceptions or threads. You should use it just in the correct way,
 *  described in the documentation of a particular function.
 *  
 *  The matrix' rows has to be the same length for argument
 *  height to make sense. Additionally, method get_det() has to
 *  work on a square matrix and get_results() on a (N x N+1] matrix;
 *  it has to represent the whole equation system, so N equations
 *  of N unknowns and one additional column for the results.
 *  get_results() transforms the matrix correctly before passing it
 *  to get_det()
 */
class EqMatrix {
	public:
		/// The main container
		vector< vector<double> > matrix;
		
		/// Declaration constructor
		EqMatrix(void) {}
		
		/// Copying constructor
		/** Warning! Do not use it in a way like below:
		 *  EqMatrix({{1}})
		 *  it leads to an ambiguity whether is the argument
		 *  a vector< vector<int> > or just an int.
		 *  Of course it leads to an Ambiguity Error
		 */
		EqMatrix(vector< vector<double> > mtrx) {
			matrix = mtrx;
		}
		
		/// Compare equality with another EqMatrix
		bool operator==(EqMatrix operand) {
			return matrix == operand.matrix;
		}
		
		/// Compare equality with raw matrix
		bool operator==(vector< vector<double> > operand) {
			return matrix == operand;
		}
		
		/// Append a row to the matrix
		void add_row(vector<double> row) {
			matrix.push_back(row);
		}
		
		/// Remove a row from the matrix
		void del_row(int row_num) {
			matrix.erase(matrix.begin() + row_num);
		}
		
		/// Move elements from the last column to the col_num column
		/** Usage: Cramer's rule */
		void move_results_col(int col_num) {
			// todo height
			for (int i = 0; i < matrix.size(); i++) {
				matrix[i][col_num] = matrix[i][matrix[i].size() - 1];
				matrix[i].pop_back();
			}
		}
		
		/// Remove a column from the matrix
		/** It supports standard indexing, but also index [-1],
		 *  which stands for the last column
		 */
		void del_col(int col_num) {
			if (col_num >= 0) {
				// todo height
				for (int i = 0; i < matrix.size(); i++) {
					matrix[i].erase(matrix[i].begin() + col_num);
				}
			} else if (col_num == -1) {
				// todo height
				for (int i = 0; i < matrix.size(); i++) {
					matrix[i].pop_back();
				}
			}
		}
		
		/// Calculate and return matrix' determinant
		/** method: Laplace expansion
		 *  It has to work on a square matrix, otherwise the behaviour
		 *  of the function is undefined.
		 */
		double get_det() {
			// todo height
			int n = matrix.size();
	
			if (n == 1) {return matrix[0][0];}
	
			double sum = 0.0;
			// todo height
			for (int i = 0; i < n; i++) {
				EqMatrix smaller_matrix (matrix);
				smaller_matrix.del_row(0);
				smaller_matrix.del_col(i);
		
				sum += pow(-1, i) * matrix[0][i] * smaller_matrix.get_det();
			}

			return sum;
		}
		
		/// Solve the equation system represented by the matrix
		/** method: Cramer's rule */
		vector<double> get_results() {
			EqMatrix temp_matrix;
			vector<double> partial_dets;
			vector<double> results;
			double general_det;
			
			// get general determinant
			temp_matrix = EqMatrix(matrix);
			temp_matrix.del_col(-1);
			general_det = temp_matrix.get_det();
				
			// calculate determinants of the matrices with swapped columns
			for (int i = 0; i < matrix.size(); i++) {
				temp_matrix = EqMatrix(matrix); // refresh matrix
				temp_matrix.move_results_col(i);
				partial_dets.push_back(temp_matrix.get_det());
			}
			
			// calculate unknowns
			for (int i = 0; i < partial_dets.size(); i++) {
				results.push_back(partial_dets[i] / general_det);
			}
			return results;
		}
};

bool test_EqMatrix_init() {
	EqMatrix data_m;
	vector< vector<double> > data_v, matrix, ok;
	bool a, b, res;

	// test: init without arguments
	matrix = EqMatrix().matrix;
	ok = {};
	a = (ok == matrix);

	// test: init with raw matrix as argument
	data_v = {
		{1.1, -2.7, 3},
		{-3.1, 2, 1.9},
		{0, 1.0, -1.7},
	};
	matrix = EqMatrix(data_v).matrix;
	ok = data_v;
	b = (ok == matrix);

	res = a && b;
	return res;
}
// todo: add test for the second version
bool test_EqMatrix_operatorequal() {
	EqMatrix matrix, data;
	bool ok, a, b, res;
	
	matrix = EqMatrix({
		{1, 2},
		{2, 1}
	});
	data = EqMatrix({
		{1, 2},
		{2, 1}
	});
	ok = true;
	a = (ok == (data == matrix));
	
	matrix = EqMatrix({
		{0, 1, 2},
		{0, 1, 3},
		{0, 1, 4}
	});
	data = EqMatrix({
		{0, 1, 2},
		{0, 1, 3},
		{0, 1, 0}
	});
	ok = false;
	b = (ok == (matrix == data));
	
	res = a && b;
	return res;
}

bool test_EqMatrix_add_row() {
	EqMatrix matrix, ok;
	vector<double> row;
	bool a, b, c, res;
	
	matrix = EqMatrix();
	row = {1, 2, 3};
	matrix.add_row(row);
	ok = EqMatrix({{1, 2, 3}});
	a = (ok == matrix);

	matrix = EqMatrix({{1, 2, 3}});
	row = {3, 2, 1};
	matrix.add_row(row);
	ok = EqMatrix({{1, 2, 3}, {3, 2, 1}});
	b = (ok == matrix);

	matrix = EqMatrix({{0, 1}});
	row = {3, 2, 1};
	matrix.add_row(row);
	ok = EqMatrix({{0, 1}, {3, 2, 1}});
	c = (ok == matrix);

	res = a && b && c;
	return res;
}

bool test_EqMatrix_del_row() {
	EqMatrix matrix, ok;
	bool a, b, res;

	matrix = EqMatrix({{1, 2, 3}, {3, 2, 1}});
	matrix.del_row(1);
	ok = EqMatrix({{1, 2, 3}});
	a = (ok == matrix);

	matrix = EqMatrix({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}});
	matrix.del_row(0);
	ok = EqMatrix({{5, 6, 7, 8}, {9, 10, 11, 12}});
	b = (ok == matrix);
	
	res = a && b;
	return res;
}

bool test_EqMatrix_move_results_col() {
	EqMatrix matrix, ok;
	bool a, b, res;
	
	matrix = EqMatrix({
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	});
	ok = EqMatrix({
		{3, 2, 3},
		{6, 5, 6},
		{9, 8, 9}
	});
	matrix.move_results_col(0);
	a = (ok == matrix);
	
	matrix = EqMatrix({
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	});
	ok = EqMatrix({
		{1, 2, 3},
		{4, 5, 6},
		{7, 8, 9}
	});
	matrix.move_results_col(2);
	b = (ok == matrix);
	
	res = a && b;
	return res;
}

bool test_EqMatrix_del_col() {
	EqMatrix matrix, ok;
	bool a, b, res;
	
	/*
	// DO NOT MAKE THIS TEST
	// will result in ambiguity error during init;
	// {{1}} can be interpreted as both vector<vector<int>> and int
	matrix = EqMatrix({{1}});
	matrix.del_col(0);
	ok = EqMatrix();
	a = (ok == matrix);
	*/

	matrix = EqMatrix({{1, 2}, {3, 4}});
	matrix.del_col(0);
	ok = EqMatrix({{2}, {4}});
	a = (ok == matrix);
	
	matrix = EqMatrix({{1, 2, 3, 4, 5}, {5, 4, 3, 2, 1}, {6, 7, 8, 9, 10}});
	matrix.del_col(3);
	ok = EqMatrix({{1, 2, 3, 5}, {5, 4, 3, 1}, {6, 7, 8, 10}});
	b = (ok == matrix);

	res = a && b;
	return res;
}

bool test_EqMatrix_get_det() {
	EqMatrix matrix;
	double ok;
	bool a, b, c, d, e, f, res;
	
	matrix = EqMatrix((vector< vector<double> >){{1.0, }, });
	ok = 1.0;
	a = almost_equal(ok, matrix.get_det(), 1);
	
	matrix = EqMatrix({
		{1.1, 2.2},
		{3.3, 5.5}
	});
	ok = -1.21;
	b = almost_equal(ok, matrix.get_det(), 1);
	
	matrix = EqMatrix({
		{1.0, 2.0, 3.0},
		{3.0, 1.0, 2.0},
		{2.0, 3.0, 1.0}
	});
	ok = 18.0;
	c = almost_equal(ok, matrix.get_det(), 1);
	
	matrix = EqMatrix({
		{0.0, 1.9, 2.2},
		{0.0, 9.1, 2.2},
		{0.0, 7.1, 3.3}
	});
	ok = 0.0;
	d = almost_equal(ok, matrix.get_det(), 1);
	
	matrix = EqMatrix({
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{5.0, 1.0, 2.0, 3.0, 4.0},
		{4.0, 5.0, 1.0, 2.0, 3.0},
		{3.0, 4.0, 5.0, 1.0, 2.0},
		{2.0, 3.0, 4.0, 5.0, 1.0}
	});
	ok = 1875.0;
	e = almost_equal(ok, matrix.get_det(), 1);
	
	matrix = EqMatrix({
		{2, 1, -3, 6},
		{1, 6, -2, -1},
		{7, 8, 1, -4},
		{1, 5, -2, -8}
	});
	ok = -1034;
	f = almost_equal(ok, matrix.get_det(), 1);

	res = a && b && c && d && e && f;
	return res;
}

bool test_EqMatrix_get_results() {
	EqMatrix matrix;
	vector<double> ok;
	bool a, b, res;
	
	matrix = EqMatrix({
		{2, 1, 3}, // 2x + y = 3
		{2, -1, 5} // 2x - y = 5, so 2y = -2, y = -1, x = 2
	});
	ok = {2, -1};
	a = (ok == matrix.get_results());
	
	matrix = EqMatrix({
		{2, 1, -3, 6, 2},  // 2a + b - 3c + 6d = 2
		{1, 6, -2, -1, 3},  // a + 6b - 2c - d = 3
		{7, 8, 1, -4, -1}, // 7a + 8b + c - 4d = -1
		{-1, 5, -2, -8, 1} // -a + 5b - 2c - 8d = 1
	});
	// determinant (without last column (the results column)): -1034
	// a = -5/8, b=5/8, c =-1/8, d=3/8
	ok = {-0.625, 0.625, -0.125, 0.375};
	b = (ok == matrix.get_results());

	res = a && b;
	return res;
}

void test_EqMatrix() {
	assert(test_EqMatrix_init());
	assert(test_EqMatrix_operatorequal());
	assert(test_EqMatrix_add_row());
	assert(test_EqMatrix_del_row());
	assert(test_EqMatrix_del_col());
	assert(test_EqMatrix_get_det());
	assert(test_EqMatrix_get_results());
}

/// Class to handle equation systems
/** Capabilities:
 *  	*prepare equation system from user input in the right format
		*parse it
		*create a matrix representing a system of equations
		
	You can either only declare a EqSystem and then use method
	take_input_from_cin() to create the system or create it from
	the already owned data, using the EquationSystem(vector<string>)
	constructor. Then raw strings will be kept in equations var.
	The only useful thing you can do with them is to create the
	equation system matrix with method get_matrix(), which automatically
	applies method parse_equation() to every equation in equations vector.
	
	The class has two static methods, split() and parse_equation(), from which
	the latter is not so useful, but the former can be easily used to split
	any strings - they do not have to match the equation format.
 *  
 */
class EquationSystem {
	public:
		/// Container of raw, string, user input
		vector<string> equations;
		/// The container to drop the effect of our work into
		vector< vector<double> > matrix;
		
		/// Declaration constructor
		EquationSystem() {}
		
		/// Constructor that is useful when you already have your equations
		EquationSystem(vector<string> eq) {
			equations = eq;
		}
		
		/// Take equations from the standard input
		/** It places raw strings in the equations vector
		 *  
		 *  Equation format:
		 *  NUMBERa NUMBERb NUMBERc ... NUMBERn =RESULT
		
		 *  where NUMBER is either int or double, with the following sign
		 *  (can be omitted if it's + and it's the first element of eq.)(but
		 *  NUMBER cannot be omitted, even if it's 1 or 0)
		 *  and a, b, c ... n is any char, can be both uppercase and lowercase,
		 *  but not less and not more, but one single char. Without it, the
		 *  stod() conversion error should be thrown at the beginning.
		
		 *  Examples (good):
		 *  2a -3b +5t =9
		 *  1x -0.9y =1
		 *  -1z =9.0
		
		 *  Examples (bad):
		 *  a -0.9b =1 (no coefficient before a)
		 *  2a - 3b + 5t = 9 (spaces between SIGN/"=" and NUMBER)
		 *  8a +10 =-17 (no unknown variable after "+10")
		 */
		void take_input_from_cin() {
			string user_input;
			int equations_amount;
			
			cout << "How many equations?\n";
			getline(cin, user_input);
			equations_amount = stoi(user_input);
			
			for (int i = 0; i < equations_amount; i++) {
				cout << "Equation " << i << ":\n";
				getline(cin, user_input);
				equations.push_back(user_input);
			}
			
			// make newline after inputing
			cout << '\n';
		}
		
		/// Split a string on an occurence of any char from delimiters
		/**
		 *  Convert a string to list of substrings, between which
		 *  in the original string occurs any char from delimiters
		 *  Reject delimiters
		 *  
		 *  Example:
		 *  	txt: "g++ -std=c++17 matrix_calc.cpp -o a.out"
		 *  	delimiters: " "
		 *  	out: {"g++", "-std=c++17", "matrix_calc.cpp", "-o", "a.out"}
		 *  
		 *  	("2x +5y -1z =13", " ") -> {"2x", "+5y", "-1z", "=13"}
		 *  	("1+2-3+4-5+6=5", "+-") -> {"1", "2", "3", "4", "5", "6", "5"}
		 *  	("1 + 2 - 3 + 4 = 4.0", " +-=") -> {"1", "2", "3", "4", "4.0"}
		 *  
		 *  @param txt string to split
		 *  @param delimiters chain of chars to divide txt by
		 *  @return list of substrings
		 */
		vector<string> split(string txt, string delimiters) {			
			short unsigned index_end = 0;
			short unsigned index_begin = 0;
			short unsigned span;
			vector<string> strings;
			string next_part;
			
			while(index_end < txt.size()) {
				// find next occurence of any of the delimiters
				index_end = txt.find_first_of(delimiters, index_begin);
				
				span = index_end - index_begin;
				next_part = txt.substr(index_begin, span);
				if (next_part.size()) strings.push_back(next_part);
				
				index_begin = index_end + 1;
			}
			
			return strings;
		}
		
		/// Transform an equation to a vector of coefficients and result
		/** Parameter equation has to be in the right format, described
		 *  in the docstring of take_input_from_cin() method
		 */
		vector<double> parse_equation(string equation) {
			int i;
			vector<string> monomials;
			vector<double> coefficients;
			string element;
			
			// equation: "2x +5y -1z =13"
			monomials = split(equation, " ");
			// monomials: {2x, +5y, -1z, =13}
			
			// make transformation from vector<string> to vector<double>
			// do not iterate over last element; it's format is "=n"
			for (i = 0; i < monomials.size() - 1; i++) {
				element = monomials[i];
				element.pop_back(); // remove unknown's name; 2x -> 2
				coefficients.push_back(stod(element));
			}
			
			// now append the result of format "=n"
			// cut "=" before stod
			double result = stod(monomials.back().substr(1));
			coefficients.push_back(result);
			
			return coefficients;
		}
		
		/// Parse it all and combine into one matrix
		EqMatrix get_matrix() {
			EqMatrix matrix;
			
			for (int i=0; i < equations.size(); i++) {
				matrix.add_row(parse_equation(equations[i]));
			}
			
			return matrix;
		}
};

// todo
bool test_EquationSystem_init() {
	return true;
}

bool test_EquationSystem_split() {
	EquationSystem es;
	string str, delim;
	vector<string> ok;
	bool a, b, c, d, res;
	
	str = "John Doe 29.02.1980 180cm dark";
	delim = " ";
	ok = {"John", "Doe", "29.02.1980", "180cm", "dark"};
	a = (es.split(str, delim) == ok);
	
	str = "";
	delim = "+-a";
	ok = {};
	b = (es.split(str, delim) == ok);
	
	str = "a + 8 = 17";
	delim = "+";
	ok = {"a ", " 8 = 17"};
	c = (es.split(str, delim) == ok);
	
	str = "1 + 2 - 3 + 4 = 4.0";
	delim = " +-=";
	ok = {"1", "2", "3", "4", "4.0"};
	d = (es.split(str, delim) == ok);
	
	res = a && b && c && d;
	return res;
}

bool test_EquationSystem_parse() {
	EquationSystem es;
	string eq;
	vector<double> ok;
	bool a, b, c, res;
	
	eq = "2x +5y -1z =13";
	ok = {2.0, 5.0, -1.0, 13.0};
	a = (ok == es.parse_equation(eq));
	
	eq = ".1x -.9y +.1z =.7";
	ok = {0.1, -0.9, 0.1, 0.7};
	b = (ok == es.parse_equation(eq));
	
	eq = "1x =3";
	ok = {1.0, 3.0};
	c = (ok == es.parse_equation(eq));

	res = a && b && c;
	return res;
}

bool test_EquationSystem_get_matrix() {
	EquationSystem es;
	vector<string> equations;
	vector< vector<double> > ok;
	bool a, b, c, res;
	
	equations = {
		"-1x =2"
	};
	es = EquationSystem(equations);
	ok = {{-1, 2}};
	a = (es.get_matrix() == ok);
	
	equations = {
		"-2x +3y =17",
		"17x -1y =3"
	};
	es = EquationSystem(equations);
	ok = {
		{-2, 3, 17},
		{17, -1, 3}
	};
	b = (es.get_matrix() == ok);
	
	equations = {
		"-1.0x -13.13y +17.8z =-0.0",
		"0.5x +0.1y -0.8z =0.9",
		"1x -2y +13.1z =-9",
	};
	es = EquationSystem(equations);
	ok = {
		{-1, -13.13, 17.8, 0},
		{0.5, 0.1, -0.8, 0.9},
		{1, -2, 13.1, -9}
	};
	c = (es.get_matrix() == ok);
	
	res = a && b && c;
	return res;
}

// untested: test input from user
void test_EquationSystem() {
	assert(test_EquationSystem_init());
	assert(test_EquationSystem_split());
	assert(test_EquationSystem_parse());
	assert(test_EquationSystem_get_matrix());
}

int main() {
	EquationSystem eq_system; //!< to store and process user's input
	vector<double> results;
	
	// run tests
	test_EqMatrix();
	test_EquationSystem();
	
	eq_system.take_input_from_cin();
	// transform user input to numbers and crunch them
	results = eq_system.get_matrix().get_results();
	
	// write nice output; a is first unknown, b is second etc.
	for (int i = 0; i < results.size(); i++) {
		cout << (char)(97 + i) << ": " << results[i] << '\n';
	}
	
	return 0;
}
