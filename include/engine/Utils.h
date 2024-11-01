// FILE WITH RANDOM USEFUL STRUCTS
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <cstdlib>
#include <random>
#include <cmath>
#include <algorithm>

struct Point{
    int x, y;
    Point(int px, int py);
    float getMagnitude() const;  
    // operator overloads
    bool operator==(const Point& p) const;
    bool operator>(const Point& p) const;
    bool operator<(const Point& p) const;
};

struct Colour{
    uint8_t r, g, b, a;
    Colour(uint8_t pr, uint8_t pg, uint8_t pb, uint8_t pa);
};

struct Rect{
    int x, y, w, h;
    Rect(int px, int py, int pw, int ph);
};

struct State2D{                 //used to track state of an object in 2d (e.g record image flip state)
    bool x, y;
    State2D(bool px, bool py);
};

// random functions
int randint(int start, int max);

double dist(const Point& a, const Point& b);

// true mathematical mod with a pythonic implementation
int trueMod(int divident, int divisor);

bool isOdd(int num);

// checks if an int is in a vector<int>
bool vecContains(std::vector<int> v, int num);

#endif