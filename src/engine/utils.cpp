#include <stdint.h>
#include <cstdlib>
#include <random>
#include <cmath>
#include <algorithm>
#include "engine/Utils.h"

Point::Point(int px, int py):
    x(px), y(py){}         //initialise fiehlds based on arguments 

float Point::getMagnitude() const{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    return sqrt((fx*fx)+(fy*fy));
}

bool Point::operator==(const Point& p) const{
    return (p.x==x)&&(p.y==y);
}

bool Point::operator<(const Point& p) const{
    // if they are same magnitude, compare x and then y
    if(getMagnitude()!=p.getMagnitude()){
        return (getMagnitude()<p.getMagnitude());
    }
    if(x!=p.x){
        return (x<p.x);
    }
    return (y<p.y);
}

bool Point::operator>(const Point& p) const{
    // if they are same magnitude, compare x and then y
    if(getMagnitude()!=p.getMagnitude()){
        return (getMagnitude()>p.getMagnitude());
    }
    if(x!=p.x){
        return (x>p.x);
    }
    return (y>p.y);
}

Colour::Colour(uint8_t pr, uint8_t pg, uint8_t pb, uint8_t pa):
    r(pr), g(pg), b(pb), a(pa){}

Rect::Rect(int px, int py, int pw, int ph):
    x(px), y(py), w(pw), h(ph){}

State2D::State2D(bool px, bool py):
    x(px), y(py){}

int randint(int start, int max){           
    // std::srand(std::time(NULL));        // seed random number generator

    // int range = (max-start) + 1;
    // int random = start + (rand() % range);        // offset + (rand() % range)
    // return random;

    std::random_device os_seed;
    const uint_least32_t seed = os_seed();

    std::mt19937 generator( seed );
    std::uniform_int_distribution<uint_least32_t> distribute(start, max);
    return distribute(generator);
}

double dist (const Point& a, const Point& b){
    int dx = a.x-b.x;
    int dy = a.y-b.y;
    double h = sqrt(static_cast<double>(dx*dx+dy*dy));
    return h;
}

int trueMod(int dividend, int divisor){
    return (dividend%divisor + divisor)%divisor;
}

bool isOdd(int num){
    return (num%2!=0);
}

// binary search for number
bool vecContains(std::vector<int> v, int num){
    if(v.size()==0){return false;}
    std::sort(v.begin(),v.end());
    while(true){
        int mid = static_cast<int>(v.size()/2);
        if(v.at(mid)==num){return true;}
        if(v.size()==1){return false;}
        if(num>mid){
            v.erase(v.begin(), v.begin()+(v.size()/2));
        } else{
            v.erase(v.begin()+(v.size()/2),v.end());
        }
    }
}