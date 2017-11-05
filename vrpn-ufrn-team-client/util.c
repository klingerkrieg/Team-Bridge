#include "util.h"
/*
template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while ( std::getline(ss, item, delim) ) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}
*/

std::vector<std::string> split(const std::string &s, std::string delim) {
	std::vector<std::string> strs;
	//boost::split(strs, s, boost::match_str(delim));
	boost::iter_split(strs, s, boost::first_finder(delim));
	return strs;
}


bool contains(std::string str, std::string with) {
	return boost::contains(str, with);
}

bool ends_with(std::string str, std::string with) {
	return boost::ends_with(str, with);
}

bool starts_with(std::string str, std::string with) {
	return boost::starts_with(str, with);

}

void replace_all(std::string &in_place, std::string from, std::string to) {
	boost::replace_all(in_place, from, to);
}


double dot(std::vector<double> a, std::vector<double> b) {
	if ( a.size() == 3 && b.size() == 3 ) {
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; //3d
	} else {
		return a[0] * b[0] + a[1] * b[1]; //2d
	}
}

double det(std::vector<double> a, std::vector<double> b) {
	return a[0] * b[1] - a[1] * b[0];
}

double mag(std::vector<double> a) {
	return a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
}

