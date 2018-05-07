<<<<<<< HEAD
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

// todo: 1. put it in a class
// todo: 2. make docstrings with doxygen
// todo: 3. write project's README.md with TeX
// todo: 4. describe weak points of the Laplace Extension:
// 				what happens for very small/0 determinants?
//				for what input is this version inaccurate?
// todo: 5. write functions to do Gaussian Elimination
// todo: 6. use GUI: wx / qt

using namespace std;


// from documentation:
// http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp) {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

vector<string> split(string txt, string delimiters) {
	/**
		Convert a string to list of substrings, between which
		in the original string occurs any char from <delimiters>
		Reject delimiters
		
		Example:
			txt: "g++ -std=c++17 matrix_calc.cpp -o a.out"
			delimiters: " "
			out: {"g++", "-std=c++17", "matrix_calc.cpp", "-o", "a.out"}
			
			("2x +5y -1z =13", " ") -> {"2x", "+5y", "-1z", "=13"}
			("1+2-3+4-5+6=5", "+-") -> {"1", "2", "3", "4", "5", "6", "5"}
			("1 + 2 - 3 + 4 = 4.0", " +-=") -> {"1", "2", "3", "4", "4.0"}
		
		@param txt string to split
		@param delimiters chain of chars to divide <txt" by
		@return list of substrings
	*/
	
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

vector<double> parse_equation(string equation) {
	/**
		Transforms equation to a vector of coefficients and result
		
		Example:
			"2x +5y -1z =13" -> {2, 5, -1, 13}
		
		@param equation string containing the equation to parse.
			format:
				"<+/-><int/double><char><space>" *x + "=<int/double>"
			example strings that matches format:
				"1x =1" (sign can be omitted if it's the first element
				"1a =1"
				"-p =1"
				"2x +5y -1z =13.71"
			and those doesn't:
				"x =1" (no coefficient before 'x')
				"1x=1" (no space before '=')
				"2x -y =1" (no coefficient given before 'y')
				"1x + 2y - 3z =1" (space between sign and coefficient)
		@return vector containing coefficients and equation's result at the end
	*/
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

vector< vector<double> > shrunk_matrix(vector <vector<double> > matrix, int column_num) {
	// returns original matrix, but without row 0 and column <column_num>
	// usage: calculating matrix determinant
	vector< vector<double> > shrunk;
	vector<double> row;
	int i;
	
	// i is initially 1, cause we want to remove row 0
	for(i = 1; i < matrix.size(); i++) {
		row = matrix[i]; // copy row
		row.erase(row.begin() + column_num); // remove column <column_num>
		shrunk.push_back(row);
	}
	
	return shrunk;
}

double matrix_determinant(vector< vector<double> > matrix) {
	// Laplace expansion
	int i;
	int n = matrix.size();
	
	if (n == 1) {return matrix[0][0];}
	
	double sum = 0.0;
	for (i = 0; i < n; i++) {
		vector< vector<double> > shrunk;
		shrunk = shrunk_matrix(matrix, i);
		
		sum += pow(-1, i) * matrix[0][i] * matrix_determinant(shrunk);
	}
	return sum;
}

vector< vector<double> > matrix_from_equations(vector<string> equations) {
	vector< vector<double> > matrix;
	
	for (int i=0; i < equations.size(); i++) {
		matrix.push_back(parse_equation(equations[i]));
	}
	
	return matrix;
}

vector< vector<double> > swap_with_results_column(vector< vector<double> > matrix, int col_num) {
	vector< vector<double> > new_matrix;
	vector<double> row;
	
	for(int i = 0; i < matrix.size(); i++) {
		row = matrix[i]; // copy row
		row[col_num] = row.back();  // insert equation's result on to col_num
		row.pop_back();  // discard equation result's column
		
		new_matrix.push_back(row);
	}
	
	return new_matrix;
}

vector<double> get_results(vector< vector<double> > matrix) {
	vector< vector<double> > temp_matrix;
	vector<double> row;
	vector<double> partial_dets;
	vector<double> results;
	double general_det;
	double res;
	
	// get general determinant
	// pop the result's column from the original matrix
	for (int i = 0; i < matrix.size(); i++) {
		row = matrix[i];
		row.pop_back();
		temp_matrix.push_back(row);
	}

	general_det = matrix_determinant(temp_matrix);
		
	// iterate and determine "smaller" determinants
	for (int i = 0; i < matrix.size(); i++) {
		temp_matrix = swap_with_results_column(matrix, i);
		partial_dets.push_back(matrix_determinant(temp_matrix));
	}
	
	for (int i = 0; i < partial_dets.size(); i++) {
		res = partial_dets[i] / general_det;
		results.push_back(res);
	}
	
	return results;
}


bool test_split() {
	string str, delim;
	vector<string> ok;
	bool a, b, c, d, res;
	
	str = "John Doe 29.02.1980 180cm dark";
	delim = " ";
	ok = {"John", "Doe", "29.02.1980", "180cm", "dark"};
	a = (split(str, delim) == ok);
	
	str = "";
	delim = "+-a";
	ok = {};
	b = (split(str, delim) == ok);
	
	str = "a + 8 = 17";
	delim = "+";
	ok = {"a ", " 8 = 17"};
	c = (split(str, delim) == ok);
	
	str = "1 + 2 - 3 + 4 = 4.0";
	delim = " +-=";
	ok = {"1", "2", "3", "4", "4.0"};
	d = (split(str, delim) == ok);
	
	res = a && b && c && d;
	return res;
}

bool test_parse_equation() {
	string eq;
	vector<double> ok;
	bool a, b, c, res;
	
	eq = "2x +5y -1z =13";
	ok = {2.0, 5.0, -1.0, 13.0};
	a = (ok == parse_equation(eq));
	
	eq = ".1x -.9y +.1z =.7";
	ok = {0.1, -0.9, 0.1, 0.7};
	b = (ok == parse_equation(eq));
	
	eq = "1x =3";
	ok = {1.0, 3.0};
	c = (ok == parse_equation(eq));

	res = a && b && c;
	return res;
}

bool test_shrunk_matrix() {
	vector< vector<double> > matrix;
	vector< vector<double> > ok;
	int column_num;
	bool a, b, res;
	
	matrix = {
		{1.0, 2.0, 3.0},
		{3.0, 1.0, 2.0},
		{2.0, 3.0, 1.0}
	};
	column_num = 1;
	ok = {
		{3.0, 2.0},
		{2.0, 1.0}
	};
	a = (ok == shrunk_matrix(matrix, column_num));
	
	matrix = {
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{5.0, 1.0, 2.0, 3.0, 4.0},
		{4.0, 5.0, 1.0, 2.0, 3.0},
		{3.0, 4.0, 5.0, 1.0, 2.0},
		{2.0, 3.0, 4.0, 5.0, 1.0}
	};
	column_num = 3;
	ok = {
		{5.0, 1.0, 2.0, 4.0},
		{4.0, 5.0, 1.0, 3.0},
		{3.0, 4.0, 5.0, 2.0},
		{2.0, 3.0, 4.0, 1.0}
	};
	b = (ok == shrunk_matrix(matrix, column_num));
	
	res = a && b;
	return res;
}

bool test_matrix_determinant() {
	vector< vector<double> > matrix;
	double ok;
	bool a, b, c, d, e, f, res;
	
	matrix = {{1.0}};
	ok = 1.0;
	a = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.1, 2.2},
		{3.3, 5.5}
	};
	ok = -1.21;
	b = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.0, 2.0, 3.0},
		{3.0, 1.0, 2.0},
		{2.0, 3.0, 1.0}
	};
	ok = 18.0;
	c = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{0.0, 1.9, 2.2},
		{0.0, 9.1, 2.2},
		{0.0, 7.1, 3.3}
	};
	ok = 0.0;
	d = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{5.0, 1.0, 2.0, 3.0, 4.0},
		{4.0, 5.0, 1.0, 2.0, 3.0},
		{3.0, 4.0, 5.0, 1.0, 2.0},
		{2.0, 3.0, 4.0, 5.0, 1.0}
	};
	ok = 1875.0;
	e = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{2, 1, -3, 6},
		{1, 6, -2, -1},
		{7, 8, 1, -4},
		{1, 5, -2, -8}
	};
	ok = -1034;
	f = almost_equal(ok, matrix_determinant(matrix), 1);

	res = a && b && c && d && e && f;
	return res;
}

bool test_matrix_from_equations() {
	vector<string> equations;
	vector< vector<double> > ok;
	bool a, b, c, res;
	
	equations = {
		"-1x =2"
	};
	ok = {{-1, 2}};
	a = (ok == matrix_from_equations(equations));
	
	equations = {
		"-2x +3y =17",
		"17x -1y =3"
	};
	ok = {
		{-2, 3, 17},
		{17, -1, 3}
	};
	b = (ok == matrix_from_equations(equations));
	
	equations = {
		"-1.0x -13.13y +17.8z =-0.0",
		"0.5x +0.1y -0.8z =0.9",
		"1x -2y +13.1z =-9",
	};
	ok = {
		{-1, -13.13, 17.8, 0},
		{0.5, 0.1, -0.8, 0.9},
		{1, -2, 13.1, -9}
	};
	c = (ok == matrix_from_equations(equations));
	
	res = a && b && c;
	return res;
}

bool test_swap_with_results_column() {
	vector< vector<double> > matrix, ok;
	int col_num;
	bool a, b, res;
	
	matrix = {
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12}
	};
	col_num = 0;
	ok = {
		{4, 2, 3},
		{8, 6, 7},
		{12, 10, 11}
	};
	a = (ok == swap_with_results_column(matrix, col_num));
	
	matrix = {
		{1, 2, 3, 4, 5},
		{6, 7, 8, 9, 10},
		{11, 12, 13, 14, 15},
		{16, 17, 18, 19, 20}
	};
	col_num = 3;
	ok = {
		{1, 2, 3, 5},
		{6, 7, 8, 10},
		{11, 12, 13, 15},
		{16, 17, 18, 20}
	};
	b = (ok == swap_with_results_column(matrix, col_num));
	
	res = a && b;
	return res;
}

bool test_get_results() {
	vector< vector<double> > matrix;
	vector<double> ok;
	bool a, b, res;
	
	matrix = {
		{2, 1, 3}, // 2x + y = 3
		{2, -1, 5} // 2x - y = 5, so 2y = -2, y = -1, x = 2
	};
	ok = {2, -1};
	a = (ok == get_results(matrix));
	
	matrix = {
		{2, 1, -3, 6, 2},  // 2a + b - 3c + 6d = 2
		{1, 6, -2, -1, 3},  // a + 6b - 2c - d = 3
		{7, 8, 1, -4, -1}, // 7a + 8b + c - 4d = -1
		{-1, 5, -2, -8, 1} // -a + 5b - 2c - 8d = 1
	};
	// determinant (without last column (the results column)): -1034
	// a = -5/8, b=5/8, c =-1/8, d=3/8
	ok = {-0.625, 0.625, -0.125, 0.375};
	b = (ok == get_results(matrix));

	res = a && b;
	return res;
}

int main() {
	vector< vector<double> > matrix;
	vector<string> equations;
	vector<double> results;
	string user_input;
	int number_of_equations;
	
	// run tests
	assert(test_split());
	assert(test_parse_equation());
	assert(test_shrunk_matrix());
	assert(test_matrix_determinant());
	assert(test_matrix_from_equations());
	assert(test_swap_with_results_column());
	assert(test_get_results());
	
	// take input from user
	cout << "How many equations?\n";
	getline(cin, user_input);
	number_of_equations = stoi(user_input);
	
	for (int i = 0; i < number_of_equations; i++) {
		cout << "Equation " << i << ":\n";
		getline(cin, user_input);
		equations.push_back(user_input);
	}
	cout << '\n'; // make newline after input
	
	// calculate unknowns
	matrix = matrix_from_equations(equations);
	results = get_results(matrix);
	
	for (int i = 0; i < results.size(); i++) {
		cout << (char)(97 + i) << ": " << results[i] << '\n';
	}
	
	return 0;
}
=======
#include <iostream>
#include <typeinfo>
#include <cassert>
#include <vector>
#include <string>
#include <math.h>
#include <limits>
#include <iomanip>
#include <type_traits>
#include <algorithm>

using namespace std;

// from documentation:
// http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp) {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

vector<string> split(string txt, string delimiters) {
	/**
		Convert a string to list of substrings, between which
		in the original string occurs any char from <delimiters>
		Reject delimiters
		
		Example:
			txt: "g++ -std=c++17 matrix_calc.cpp -o a.out"
			delimiters: " "
			out: {"g++", "-std=c++17", "matrix_calc.cpp", "-o", "a.out"}
			
			("2x +5y -1z =13", " ") -> {"2x", "+5y", "-1z", "=13"}
			("1+2-3+4-5+6=5", "+-") -> {"1", "2", "3", "4", "5", "6", "5"}
			("1 + 2 - 3 + 4 = 4.0", " +-=") -> {"1", "2", "3", "4", "4.0"}
		
		@param txt string to split
		@param delimiters chain of chars to divide <txt" by
		@return list of substrings
	*/
	
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

vector<double> parse_equation(string equation) {
	/**
		Transforms equation to a vector of coefficients and result
		
		Example:
			"2x +5y -1z =13" -> {2, 5, -1, 13}
		
		@param equation string containing the equation to parse.
			format:
				"<+/-><int/double><char><space>" *x + "=<int/double>"
			example strings that matches format:
				"1x =1" (sign can be omitted if it's the first element
				"1a =1"
				"-p =1"
				"2x +5y -1z =13.71"
			and those doesn't:
				"x =1" (no coefficient before 'x')
				"1x=1" (no space before '=')
				"2x -y =1" (no coefficient given before 'y')
				"1x + 2y - 3z =1" (space between sign and coefficient)
		@return vector containing coefficients and equation's result at the end
	*/
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

vector< vector<double> > shrunk_matrix(vector <vector<double> > matrix, int column_num) {
	// returns original matrix, but without row 0 and column <column_num>
	// usage: calculating matrix determinant
	vector< vector<double> > shrunk;
	vector<double> row;
	int i;
	
	// i is initially 1, cause we want to remove row 0
	for(i = 1; i < matrix.size(); i++) {
		row = matrix[i]; // copy row
		row.erase(row.begin() + column_num); // remove column <column_num>
		shrunk.push_back(row);
	}
	
	return shrunk;
}

double matrix_determinant(vector< vector<double> > matrix) {
	int i;
	int n = matrix.size();
	
	if (n == 1) {return matrix[0][0];}
	
	double sum = 0.0;
	for (i = 0; i < n; i++) {
		vector< vector<double> > shrunk;
		shrunk = shrunk_matrix(matrix, i);
		
		sum += pow(-1, i) * matrix[0][i] * matrix_determinant(shrunk);
	}
	return sum;
}

vector< vector<double> > matrix_from_equations(vector<string> equations) {
	vector< vector<double> > matrix;
	
	for (int i=0; i < equations.size(); i++) {
		matrix.push_back(parse_equation(equations[i]));
	}
	
	return matrix;
}

vector< vector<double> > swap_with_results_column(vector< vector<double> > matrix, int col_num) {
	vector< vector<double> > new_matrix;
	vector<double> row;
	
	for(int i = 0; i < matrix.size(); i++) {
		row = matrix[i]; // copy row
		row[col_num] = row.back();  // insert equation's result on to col_num
		row.pop_back();  // discard equation result's column
		
		new_matrix.push_back(row);
	}
	
	return new_matrix;
}

vector<double> get_results(vector< vector<double> > matrix) {
	vector< vector<double> > temp_matrix;
	vector<double> row;
	vector<double> partial_dets;
	vector<double> results;
	double general_det;
	double res;
	
	// get general determinant
	// pop the result's column from the original matrix
	for (int i = 0; i < matrix.size(); i++) {
		row = matrix[i];
		row.pop_back();
		temp_matrix.push_back(row);
	}

	general_det = matrix_determinant(temp_matrix);
		
	// iterate and determine "smaller" determinants
	for (int i = 0; i < matrix.size(); i++) {
		temp_matrix = swap_with_results_column(matrix, i);
		partial_dets.push_back(matrix_determinant(temp_matrix));
	}
	
	for (int i = 0; i < partial_dets.size(); i++) {
		res = partial_dets[i] / general_det;
		results.push_back(res);
	}
	
	return results;
}


bool test_split() {
	string str, delim;
	vector<string> ok;
	bool a, b, c, d, res;
	
	str = "John Doe 29.02.1980 180cm dark";
	delim = " ";
	ok = {"John", "Doe", "29.02.1980", "180cm", "dark"};
	a = (split(str, delim) == ok);
	
	str = "";
	delim = "+-a";
	ok = {};
	b = (split(str, delim) == ok);
	
	str = "a + 8 = 17";
	delim = "+";
	ok = {"a ", " 8 = 17"};
	c = (split(str, delim) == ok);
	
	str = "1 + 2 - 3 + 4 = 4.0";
	delim = " +-=";
	ok = {"1", "2", "3", "4", "4.0"};
	d = (split(str, delim) == ok);
	
	res = a && b && c && d;
	return res;
}

bool test_parse_equation() {
	string eq;
	vector<double> ok;
	bool a, b, c, res;
	
	eq = "2x +5y -1z =13";
	ok = {2.0, 5.0, -1.0, 13.0};
	a = (ok == parse_equation(eq));
	
	eq = ".1x -.9y +.1z =.7";
	ok = {0.1, -0.9, 0.1, 0.7};
	b = (ok == parse_equation(eq));
	
	eq = "1x =3";
	ok = {1.0, 3.0};
	c = (ok == parse_equation(eq));

	res = a && b && c;
	return res;
}

bool test_shrunk_matrix() {
	vector< vector<double> > matrix;
	vector< vector<double> > ok;
	int column_num;
	bool a, b, res;
	
	matrix = {
		{1.0, 2.0, 3.0},
		{3.0, 1.0, 2.0},
		{2.0, 3.0, 1.0}
	};
	column_num = 1;
	ok = {
		{3.0, 2.0},
		{2.0, 1.0}
	};
	a = (ok == shrunk_matrix(matrix, column_num));
	
	matrix = {
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{5.0, 1.0, 2.0, 3.0, 4.0},
		{4.0, 5.0, 1.0, 2.0, 3.0},
		{3.0, 4.0, 5.0, 1.0, 2.0},
		{2.0, 3.0, 4.0, 5.0, 1.0}
	};
	column_num = 3;
	ok = {
		{5.0, 1.0, 2.0, 4.0},
		{4.0, 5.0, 1.0, 3.0},
		{3.0, 4.0, 5.0, 2.0},
		{2.0, 3.0, 4.0, 1.0}
	};
	b = (ok == shrunk_matrix(matrix, column_num));
	
	res = a && b;
	return res;
}

bool test_matrix_determinant() {
	vector< vector<double> > matrix;
	double ok;
	bool a, b, c, d, e, f, res;
	
	matrix = {{1.0}};
	ok = 1.0;
	a = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.1, 2.2},
		{3.3, 5.5}
	};
	ok = -1.21;
	b = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.0, 2.0, 3.0},
		{3.0, 1.0, 2.0},
		{2.0, 3.0, 1.0}
	};
	ok = 18.0;
	c = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{0.0, 1.9, 2.2},
		{0.0, 9.1, 2.2},
		{0.0, 7.1, 3.3}
	};
	ok = 0.0;
	d = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{5.0, 1.0, 2.0, 3.0, 4.0},
		{4.0, 5.0, 1.0, 2.0, 3.0},
		{3.0, 4.0, 5.0, 1.0, 2.0},
		{2.0, 3.0, 4.0, 5.0, 1.0}
	};
	ok = 1875.0;
	e = almost_equal(ok, matrix_determinant(matrix), 1);
	
	matrix = {
		{2, 1, -3, 6},
		{1, 6, -2, -1},
		{7, 8, 1, -4},
		{1, 5, -2, -8}
	};
	ok = -1034;
	f = almost_equal(ok, matrix_determinant(matrix), 1);

	res = a && b && c && d && e && f;
	return res;
}

bool test_matrix_from_equations() {
	vector<string> equations;
	vector< vector<double> > ok;
	bool a, b, c, res;
	
	equations = {
		"-1x =2"
	};
	ok = {{-1, 2}};
	a = (ok == matrix_from_equations(equations));
	
	equations = {
		"-2x +3y =17",
		"17x -1y =3"
	};
	ok = {
		{-2, 3, 17},
		{17, -1, 3}
	};
	b = (ok == matrix_from_equations(equations));
	
	equations = {
		"-1.0x -13.13y +17.8z =-0.0",
		"0.5x +0.1y -0.8z =0.9",
		"1x -2y +13.1z =-9",
	};
	ok = {
		{-1, -13.13, 17.8, 0},
		{0.5, 0.1, -0.8, 0.9},
		{1, -2, 13.1, -9}
	};
	c = (ok == matrix_from_equations(equations));
	
	res = a && b && c;
	return res;
}

bool test_swap_with_results_column() {
	vector< vector<double> > matrix, ok;
	int col_num;
	bool a, b, res;
	
	matrix = {
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12}
	};
	col_num = 0;
	ok = {
		{4, 2, 3},
		{8, 6, 7},
		{12, 10, 11}
	};
	a = (ok == swap_with_results_column(matrix, col_num));
	
	matrix = {
		{1, 2, 3, 4, 5},
		{6, 7, 8, 9, 10},
		{11, 12, 13, 14, 15},
		{16, 17, 18, 19, 20}
	};
	col_num = 3;
	ok = {
		{1, 2, 3, 5},
		{6, 7, 8, 10},
		{11, 12, 13, 15},
		{16, 17, 18, 20}
	};
	b = (ok == swap_with_results_column(matrix, col_num));
	
	res = a && b;
	return res;
}

bool test_get_results() {
	vector< vector<double> > matrix;
	vector<double> ok;
	bool a, b, res;
	
	matrix = {
		{2, 1, 3}, // 2x + y = 3
		{2, -1, 5} // 2x - y = 5, so 2y = -2, y = -1, x = 2
	};
	ok = {2, -1};
	a = (ok == get_results(matrix));
	
	matrix = {
		{2, 1, -3, 6, 2},  // 2a + b - 3c + 6d = 2
		{1, 6, -2, -1, 3},  // a + 6b - 2c - d = 3
		{7, 8, 1, -4, -1}, // 7a + 8b + c - 4d = -1
		{-1, 5, -2, -8, 1} // -a + 5b - 2c - 8d = 1
	};
	// determinant (without last column (the results column)): -1034
	// a = -5/8, b=5/8, c =-1/8, d=3/8
	ok = {-0.625, 0.625, -0.125, 0.375};
	b = (ok == get_results(matrix));

	res = a && b;
	return res;
}

int main() {
	vector< vector<double> > matrix;
	vector<string> equations;
	vector<double> results;
	string user_input;
	int number_of_equations;
	
	// run tests
	assert(test_split());
	assert(test_parse_equation());
	assert(test_shrunk_matrix());
	assert(test_matrix_determinant());
	assert(test_matrix_from_equations());
	assert(test_swap_with_results_column());
	assert(test_get_results());
	
	// take input from user
	cout << "How many equations?\n";
	getline(cin, user_input);
	number_of_equations = stoi(user_input);
	
	for (int i = 0; i < number_of_equations; i++) {
		cout << "Equation " << i << ":\n";
		getline(cin, user_input);
		equations.push_back(user_input);
	}
	cout << '\n'; // make newline after input
	
	// calculate unknowns
	matrix = matrix_from_equations(equations);
	results = get_results(matrix);
	
	for (int i = 0; i < results.size(); i++) {
		cout << (char)(97 + i) << ": " << results[i] << '\n';
	}
	
	return 0;
}
>>>>>>> 7e848a66168af12ca3c636585b750e18a8604363
