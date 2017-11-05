#pragma once
#include <string>
//#include <sstream>
#include <vector>
//#include <iterator>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

/*template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);*/

std::vector<std::string> split(const std::string &s, std::string delim);

bool contains(std::string str, std::string with);
bool ends_with(std::string str, std::string with);
bool starts_with(std::string str, std::string with);

void replace_all(std::string &in_place, std::string from, std::string to);

double dot(std::vector<double> a, std::vector<double> b);

double mag(std::vector<double> a);

double det(std::vector<double> a, std::vector<double> b);