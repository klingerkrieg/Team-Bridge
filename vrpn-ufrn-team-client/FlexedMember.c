#include "FlexedMember.h"



int FlexedMember::flexed2d(std::map<int, std::vector<double>> points, int paramAngle) {
	
	double angle = atan2(det(points.at(0), points.at(1)), dot(points.at(0), points.at(1))) * 1000; //2d
	
	if ( paramAngle > angle ) {
		return 1;
	} else {
		return 0;
	}
}


int FlexedMember::flexed3d(std::map<int, std::vector<double>> points, int paramAngle, int angleMod) {
	
	

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
	if ( angleMod == -1 ) {
		comp = paramAngle > angle;
	} else
	if ( angleMod == 1 ) {
		comp = paramAngle < angle;
	} else
	if ( angleMod == 0 ) {
		comp = paramAngle == angle;
	}
	return comp;
}

int FlexedMember::flexed3d(std::map<int, std::vector<double>> points, int paramAngle, int angleMod, int direction) {

	bool comp = flexed3d(points, paramAngle, angleMod);
	if ( comp ) {

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






std::map<int, std::vector<double>> FlexedMember::getPoints(const vrpn_TRACKERCB t, int sensor1, int sensor2, int sensor3, std::map<int, std::vector<double>> &positions) {

	std::map<int, std::vector<double>> points;

	if ( t.sensor == sensor1 || t.sensor == sensor2 || t.sensor == sensor3 ) {
		std::vector<double> actPos = { t.pos[0], t.pos[1], t.pos[2] };
		positions.insert_or_assign(t.sensor, actPos);
	} else {
		return points;
	}

	if ( positions.find(sensor1) == positions.end() || positions.find(sensor2) == positions.end() || positions.find(sensor3) == positions.end() ) {
		return points;
	}

	points.insert_or_assign(0, positions.at(sensor1));
	points.insert_or_assign(1, positions.at(sensor2));
	points.insert_or_assign(2, positions.at(sensor3));

	return points;
}