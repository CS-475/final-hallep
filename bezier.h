#ifndef BEZIER_DEFINED
#define BEZIER_DEFINED

#include "include/GPathBuilder.h"

#include <cmath>
#include <vector>
#include <algorithm>

/* ========== POINTS ========== */

/* quadT()
 * q(t) = [(1-t)^2 * A] + [2 * (1-t) * t * B] + [t^2 * C]
 * grouped by t: [A - B + C]*t^2 + [-2A + B]*t + A
 *      a = A - 2B + C = A - B - B + C
 *      b = -2A + 2B = B - 2A = B + B - A - A
 *      c = A
 *   = at^2 + bt + c
 *   = (at + b)t + c
 */
inline GPoint quadT(const GPoint src[3], float t) {
    GPoint a = src[0] - src[1] - src[1] + src[2];
    GPoint b = src[1] + src[1] - src[0] - src[0];
    GPoint c = src[0];
    return ((a*t) + b)*t + c;
}

/* cubicT()
 * c(t) = [(1-t)^3 * A] + [3 * (1-t)^2 * t * B] + [3 * (1-t) * t^2 * C] + [t^3 * D]
 * grouped by t: [-A + B - C + D]t^3 + [3A - 2B + C]t^2 + [-3A + B]t + A
 *      a = -A + 3B - 3C + D = D - A + 3(B - C)
 *      b = 3A - 6B + 3C = 3(A - 2B + C)
 *      c = -3A + 3B = 3*(B - A)
 *      d = A
 *   = at^3 + bt^2 + ct + d
 *   = ((at + b)t + c)t + d
 */
inline GPoint cubicT(const GPoint src[4], float t) {
    GPoint a = src[3] - src[0] + (3 * (src[1] - src[2]));
    GPoint b = 3 * (src[0] - (2 * src[1]) + src[2]);
    GPoint c = 3 * (src[1] - src[0]);
    GPoint d = src[0];
    return (((a*t) + b)*t + c)*t + d;
}

/* ptT()
 * [(1-t) * A] + [t * B]
 *   = A + (B - A)t
 */
inline GPoint ptT(GPoint p0, GPoint p1, float t) {
    return p0 + ((p1 - p0)*t);
}

/* ========== BOUNDS ========== */

/* quadBounds() */
inline GRect quadBounds(GPoint pts[GPath::kMaxNextPoints]) {
    std::vector<float> xVals;
    std::vector<float> yVals;

    // add end points
    xVals.push_back(pts[0].x);
    xVals.push_back(pts[2].x);

    yVals.push_back(pts[0].y);
    yVals.push_back(pts[0].y);

    GPoint src[3] = {pts[0], pts[1], pts[2]};

    // x extremes
    float tx = (pts[0].x - pts[1].x) / (pts[0].x - (2 * pts[1].x) + pts[2].x);
    if ((tx >= 0) && (tx <= 1)) {
        GPoint xExtreme = quadT(src, tx);
        xVals.push_back(xExtreme.x);
    }

    // y extremes
    float ty = (pts[0].y - pts[1].y) / (pts[0].y - (2 * pts[1].y) + pts[2].y);
    if ((ty >= 0) && (ty <= 1)) {
        GPoint yExtreme = quadT(src, ty);
        yVals.push_back(yExtreme.y);
    }

    float top = *std::min_element(yVals.begin(), yVals.end());
    float bottom = *std::max_element(yVals.begin(), yVals.end());
    float left = *std::min_element(xVals.begin(), xVals.end());
    float right = *std::max_element(xVals.begin(), xVals.end());
    
    return GRect::LTRB(left,top,right,bottom);
}

/* cubicBounds() */
inline GRect cubicBounds(GPoint pts[GPath::kMaxNextPoints]) {
    std::vector<float> xVals;
    std::vector<float> yVals;

    // add end points
    xVals.push_back(pts[0].x);
    xVals.push_back(pts[3].x);

    yVals.push_back(pts[0].y);
    yVals.push_back(pts[3].y);

    GPoint src[4] = {pts[0], pts[1], pts[2], pts[3]};

    /* x extremes */
    // derivative quadratic
    float ax = (-3 * pts[0].x) + (9 * pts[1].x) + (-9 * pts[2].x) + (3 * pts[3].x);
    float bx = (6 * pts[0].x) + (-12 * pts[1].x) + (6 * pts[2].x);
    float cx = 3 * (pts[1].x - pts[0].x);

    // A = 0 (linear)
    if (ax == 0) {
        float tx = -cx / bx;
        if ((tx >= 0) && (tx <= 1)) {
            GPoint xExtreme = cubicT(src, tx);
            xVals.push_back(xExtreme.x);
        }
    }

    // if 2 real solutions
    else if ((bx * bx) >= (4 * ax * cx)) {
        float sqrtdetX = std::sqrt((bx * bx) - (4 * ax * cx));
        
        // (-b - sqrt(det)) / 2a
        float txm = (-bx - sqrtdetX) / (2 * ax);
        if ((txm >= 0) && (txm <= 1)) {
            GPoint xExtreme = cubicT(src, txm);
            xVals.push_back(xExtreme.x);
        }

        // (-b + sqrt(det)) / 2a
        float txp = (-bx + sqrtdetX) / (2 * ax);
        if ((txp >= 0) && (txp <= 1)) {
            GPoint xExtreme = cubicT(src, txp);
            xVals.push_back(xExtreme.x);
        }
    }

    /* y extremes */
    // derivative quadratic
    float ay = (-3 * pts[0].y) + (9 * pts[1].y) + (-9 * pts[2].y) + (3 * pts[3].y);
    float by = (6 * pts[0].y) + (-12 * pts[1].y) + (6 * pts[2].y);
    float cy = 3 * (pts[1].y - pts[0].y);

    // A = 0 (linear)
    if (ay == 0) {
        float ty = -cy / by;
        if ((ty >= 0) && (ty <= 1)) {
            GPoint yExtreme = cubicT(src, ty);
            yVals.push_back(yExtreme.y);
        }
    }

    // if 2 real solutions
    else if ((by * by) >= (4 * ay * cy)) {
        float sqrtdetY = std::sqrt((by * by) - (4 * ay * cy));
        
        // (-b - sqrt(det)) / 2a
        float tym = (-by - sqrtdetY) / (2 * ay);
        if ((tym >= 0) && (tym <= 1)) {
            GPoint yExtreme = cubicT(src, tym);
            yVals.push_back(yExtreme.y);
        }

        // (-b + sqrt(det)) / 2a
        float typ = (-by + sqrtdetY) / (2 * ay);
        if ((typ >= 0) && (typ <= 1)) {
            GPoint yExtreme = cubicT(src, typ);
            yVals.push_back(yExtreme.y);
        }
    }

    // get limits
    float top = *std::min_element(yVals.begin(), yVals.end());
    float bottom = *std::max_element(yVals.begin(), yVals.end());
    float left = *std::min_element(xVals.begin(), xVals.end());
    float right = *std::max_element(xVals.begin(), xVals.end());
    
    return GRect::LTRB(left,top,right,bottom);

}

#endif