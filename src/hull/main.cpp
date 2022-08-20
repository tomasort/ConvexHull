// Tomas Ortega Rojas
// Computer Graphics Assignment 1 Fall 2021
// Algorithm for finding the convex hull of a point cloud in 2D.
//
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
////////////////////////////////////////////////////////////////////////////////

typedef std::complex<double> Point;
typedef std::vector<Point> Polygon;

// Function that returns the determinant of the matrix containing the points form the arguments
// We don't use this function for this problem.
double inline det(const Point &u, const Point &v) {
    return u.real()*v.imag() - v.real()*u.imag();
}

struct Compare {
	Point p0; // Leftmost point of the poly
    // Calculate which point has a larger angle with respect the point P0 and the x-axis.
    bool operator ()(const Point &p1, const Point &p2) {
		Point a = p1 - p0;
        Point b = p2 - p0;
		float z_component = a.real()*b.imag() - a.imag()*b.real();
		if(z_component > 0){
		    return true;
		}else if(z_component == 0){
		    if(std::sqrt(std::pow(a.real(), 2) + std::pow(a.imag(), 2)) < std::sqrt(std::pow(b.real(), 2) + std::pow(b.imag(), 2))){
		        return true;
		    }
		}
		return false;
	}
};

// This function returns true if the angle between these points is smaller than 180 degrees.
// We need this function to determine if the next point over is a right turn or a left turn.
bool inline salientAngle(Point &a, Point &b, Point &c) {
    Point v = b - a;
    Point w = c - a;
    float z_component = v.real()*w.imag() - v.imag()*w.real();
    if (z_component >= 0){
        return true;
    }
	return false;
}

////////////////////////////////////////////////////////////////////////////////

Polygon convex_hull(std::vector<Point> &points) {
	Compare order;
	Point lowest_leftmost = points[0];
	// find the lowest leftmost point in out data
    for(Point p : points){
        if (p.imag() <= lowest_leftmost.imag()){
            if(p.imag() == lowest_leftmost.imag() && lowest_leftmost.real() < p.real()){
               continue;
            }
            lowest_leftmost = p;
        }
    }
    order.p0 = lowest_leftmost;
	std::sort(points.begin(), points.end(), order);
	Polygon hull;
	// go through each point and see if we made a right turn or a left turn.
	for(Point p : points){
        while(hull.size() > 1 && !salientAngle(hull[hull.size()-2], hull[hull.size()-1], p)){
            hull.pop_back();
        }
        // just a check to avoid adding a duplicate element to our solution.
        if (hull.size() > 1 && hull[hull.size()-1] == p){
            continue;
        }
        hull.push_back(p);
    }
	return hull;
}

////////////////////////////////////////////////////////////////////////////////

// Function that reads a file containing points. We need to go through the file one line at a time getting the points one by one
std::vector<Point> load_xyz(const std::string &filename) {
	std::vector<Point> points;
	std::ifstream in(filename);
	// The first line indicates the number of points N. The following N lines contains a space-separated list of point coordinates
	// (we ignore the 3rd coordinate when reading the data)
	if (in.is_open()){
        int num_of_points;
        in >> num_of_points;
        for(int i = 0; i < num_of_points; i++){
            double x, y, z;
            in >> x >> y >> z;
            points.push_back(Point(x, y));
        }
    }
    return points;
}

void save_obj(const std::string &filename, Polygon &poly) {
	std::ofstream out(filename);
	if (!out.is_open()) {
		throw std::runtime_error("failed to open file " + filename);
	}
	out << std::fixed;
	for (const auto &v : poly) {
		out << "v " << v.real() << ' ' << v.imag() << " 0\n";
	}
	for (size_t i = 0; i < poly.size(); ++i) {
		out << "l " << i+1 << ' ' << 1+(i+1)%poly.size() << "\n";
	}
	out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[]) {
	if (argc <= 2) {
		std::cerr << "Usage: " << argv[0] << " points.xyz output.obj" << std::endl;
	}
	// read the file containing the points
	std::vector<Point> points = load_xyz(argv[1]);
    // return the polygon after running the algorithm
	Polygon hull = convex_hull(points);
	save_obj(argv[2], hull);
	return 0;
}
