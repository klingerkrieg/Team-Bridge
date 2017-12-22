#include "FlexedMember.h"

bool FlexedMember::assignChecker(CheckerSubject *checker, KeyMap *keyMap) {
	return false;
}


int FlexedMember::flexed2d(std::map<int, std::vector<double>> points, int paramAngle) {
	
	double angle = atan2(det(points.at(0), points.at(1)), dot(points.at(0), points.at(1))) * 1000; //2d
	
	if ( paramAngle > angle ) {
		return 1;
	} else {
		return 0;
	}
}


int FlexedMember::flexed3d(std::map<int, std::vector<double>> points, KeyMap * keyMap) {
	
	if ( points.size() < 3 ) {
		return -1;
	}
	
	std::vector<double> v1 = { points.at(0)[0] - points.at(1)[0], points.at(0)[1] - points.at(1)[1], points.at(0)[2] - points.at(1)[2] };
	std::vector<double> v2 = { points.at(2)[0] - points.at(1)[0], points.at(2)[1] - points.at(1)[1], points.at(2)[2] - points.at(1)[2] };

	double v1mag = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
	std::vector<double> v1norm = { v1[0] / v1mag, v1[1] / v1mag, v1[2] / v1mag };

	double v2mag = sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
	std::vector<double> v2norm = { v2[0] / v2mag, v2[1] / v2mag, v2[2] / v2mag };


	double res = v1norm[0] * v2norm[0] + v1norm[1] * v2norm[1] + v1norm[2] * v2norm[2];

	double angle = acos(res) * 180.0 / 3.14159265;


	//printf("%.2f\n", angle);
	bool comp;
	if ( keyMap->getAngleMod() == -1 ) {
		comp = keyMap->getAngle() > angle;
	} else
	if ( keyMap->getAngleMod() == 1 ) {
		comp = keyMap->getAngle() < angle;
	} else
	if ( keyMap->getAngleMod() == 0 ) {
		comp = keyMap->getAngle() == angle;
	}

#ifdef THERAPY_MODULE
	if ( comp ) {
		if ( keyMap->getSaveData().compare("") ) {
			storage->saveToFile(keyMap->getDev().c_str(), keyMap->getSaveData(), angle);
		}
	}
#endif
	
	return comp;
}

int FlexedMember::flexed3d(std::map<int, std::vector<double>> points, KeyMap * keyMap, int direction) {

	

	int comp = flexed3d(points, keyMap);
	if ( comp == -1 ) {
		return -1;
	} else
	if ( comp == 1 ) {

		std::vector<double> p1 = points.at(1);
		std::vector<double> p2 = points.at(2);

		if ( direction == UP ) {
			return p1[1] < p2[1];
		} else 
		if ( direction == DOWN ) {
			return p1[1] > p2[1];
		}

		return -1;
	} else {
		return 0;
	}
}


std::map<int, std::vector<double>> FlexedMember::getPoints(SkeletonPart skelPart0, SkeletonPart skelPart1, SkeletonPart skelPart2) {

	std::map<int, std::vector<double>> points;

	if ( skelPart0.defined )
		points.insert_or_assign(0, std::vector<double>({ skelPart0.x, skelPart0.y, skelPart0.z }));
	if ( skelPart1.defined )
		points.insert_or_assign(1, std::vector<double>({ skelPart1.x, skelPart1.y, skelPart1.z }));
	if ( skelPart2.defined )
		points.insert_or_assign(2, std::vector<double>({ skelPart2.x, skelPart2.y, skelPart2.z }));

	return points;
}
