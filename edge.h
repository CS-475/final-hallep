#ifndef EDGE_DEFINED
#define EDGE_DEFINED

#include "bezier.h"
#include "include/GPoint.h"
#include "include/GPath.h"

#include <cmath>
#include <vector>
#include <algorithm>

struct Edge {
    int top, bottom;
    float m, b;
    int w;

    // return the X value for a given Y value
    float eval_x(float y) const {
        return (m * y) + b;
    }

    bool isValid(int y) {
        return (y >= top) && (y <= bottom);
    }
    
};

/* make_edge() */
inline Edge make_edge(GPoint p1, GPoint p2) {
    Edge e;

    e.m = (p1.x - p2.x) / (p1.y - p2.y);
    e.b = p1.x - (e.m * p1.y);

    e.top = int(round(std::min(p1.y, p2.y)));
    e.bottom = int(round(std::max(p1.y, p2.y)));

    if (p1.y < p2.y) {
        e.w = -1;
    } else {
        e.w = 1;
    }
    
    return e;
}

inline Edge make_edge(GPoint p1, GPoint p2, GPoint p1_ori, GPoint p2_ori) {
    Edge e;

    e.m = (p1.x - p2.x) / (p1.y - p2.y);
    e.b = p1.x - (e.m * p1.y);

    e.top = int(round(std::min(p1.y, p2.y)));
    e.bottom = int(round(std::max(p1.y, p2.y)));

    if (p1_ori.y < p2_ori.y) {
        e.w = -1;
    } else {
        e.w = 1;
    }
    
    return e;
}

/* x_intersect()
 * returns the point (p0) where the line between two points (p1, p2) intersects with the line x = x0
 */
inline GPoint x_intersect(GPoint p1, GPoint p2, float x0) {
    float y = ((x0 - p1.x) * (p2.y - p1.y) / (p2.x - p1.x)) + p1.y;
    return {x0, y};
}

/* y_intersect()
 * returns the point (p0) where the line between two points (p0, p1) intersects with the line y = y0
 */
inline GPoint y_intersect(GPoint p1, GPoint p2, float y0) {
    float x = ((y0 - p1.y) * (p2.x - p1.x) / (p2.y - p1.y)) + p1.x;
    return {x, y0};
}

/* ========== VALIDATE ========== */

/* valid_points()
 * returns whether an edge should be made between 2 points
 */
inline bool valid_points(GPoint p1, GPoint p2, int width, int height) {
    
    // invalid edge: completely above the canvas
    if ((p1.y < 0) && (p2.y < 0)) {
        return false;
    }

    // invalid edge: completely below the canvas
    if ((p1.y >= height) && (p2.y >= height)) {
        return false;
    }

    // invalid edge: perfectly horizontal
    if (p1.y == p2.y) {
        return false;
    }

    // invalid edge: functionally horizontal
    if (int(round(p1.y)) == int(round(p2.y))) {
        return false;
    }

    // valid edge
    return true;
}

/* valid_bounds()
 * return whether a GRect is within the bitmap
 */
inline bool valid_bounds(GRect bounds, int width, int height) {
    // invalid bounds: completely to the left
    if ((bounds.left < 0) && (bounds.right < 0)) {
        return false;
    }

    // invalid bounds: completely to the right
    if ((bounds.left >= width) && (bounds.right >= width)) {
        return false;
    }

    // invalid bounds: completely above
    if ((bounds.top < 0) && (bounds.bottom < 0)) {
        return false;
    }

    // invalid bounds: completely below
    if ((bounds.top >= height) && (bounds.bottom >= height)) {
        return false;
    }

    // valid bounds
    return true;
}

/* ========== PROCESS SEGMENTS ========== */

/* process_points()
 * process 2 GPoints (p1, p2)
 * add necessary edges to edge vector (edges)
 */
inline void process_points(std::vector<Edge>* edges, GPoint op1, GPoint op2, int width, int height, GMatrix ctm) {
    GPoint oriPoints[2] = {op1, op2};
    GPoint newPoints[2];

    ctm.mapPoints(newPoints,oriPoints,2);

    GPoint p1 = newPoints[0];
    GPoint p2 = newPoints[1];

    // if edge(s) between p1 and p2 would be valid...
    if (valid_points(p1, p2, width, height)) {

        // PROCESS VERTICALLY
        GPoint top, bottom;
        if (p1.y < p2.y) {
            top = p1;
            bottom = p2;
        } else {
            top = p2;
            bottom = p1;
        }

        // edge crosses top border
        if (top.y < 0) {
            top = y_intersect(top, bottom, 0);
        }

        // edge crosses bottom border
        if (bottom.y >= height) {
            bottom = y_intersect(top, bottom, height-1);
        }

        // PROCESS HORIZONTALLY
        GPoint left, right;
        if (top.x < bottom.x) {
            left = top;
            right = bottom;
        } else {
            left = bottom;
            right = top;
        }

        // edge completely to the left
        if ((left.x < 0) && (right.x < 0)) {
            GPoint lIntersect, rIntersect;
            lIntersect.x = 0;
            lIntersect.y = left.y;

            rIntersect.x = 0;
            rIntersect.y = right.y;

            Edge this_edge = make_edge(lIntersect,rIntersect,p1,p2);
            (*edges).push_back(this_edge);
        }
        
        // edge completely to the right
        else if ((left.x >= width) && (right.x >= width)) {

            GPoint lIntersect, rIntersect;
            lIntersect.x = width;
            lIntersect.y = left.y;

            rIntersect.x = width;
            rIntersect.y = right.y;

            Edge this_edge = make_edge(lIntersect,rIntersect,p1,p2);
            (*edges).push_back(this_edge);

        }
        
        // edge crosses both borders
        else if ((left.x < 0) && (right.x >= width)) {
            GPoint lIntersect = x_intersect(left, right, 0);

            GPoint lBend;
            lBend.x = 0;
            lBend.y = left.y;
            
            GPoint rIntersect = x_intersect(left, right, width);

            GPoint rBend;
            rBend.x = width;
            rBend.y = right.y;

            Edge across = make_edge(lIntersect, rIntersect, p1, p2);
            (*edges).push_back(across);

            Edge lVertical = make_edge(lIntersect, lBend, p1, p2);
            (*edges).push_back(lVertical);

            Edge rVertical = make_edge(rIntersect, rBend, p1, p2);            
            (*edges).push_back(rVertical);
        }
        
        // edge crosses left border
        else if (left.x < 0) {
            GPoint lIntersect = x_intersect(left, right, 0);

            GPoint lBend;
            lBend.x = 0;
            lBend.y = left.y;

            Edge vertical = make_edge(lIntersect, lBend, p1, p2);
            (*edges).push_back(vertical);

            Edge across = make_edge(lIntersect, right, p1, p2);
            (*edges).push_back(across);
        }
        
        // edge crosses right border
        else if (right.x >= width) {
            GPoint rIntersect = x_intersect(left, right, width - 1);

            GPoint rBend;
            rBend.x = width - 1;
            rBend.y = right.y;

            Edge across = make_edge(left, rIntersect, p1, p2);
            (*edges).push_back(across);

            Edge vertical = make_edge(rIntersect, rBend, p1, p2);
            (*edges).push_back(vertical);
        }
        
        // edge completely valid
        else {
            Edge this_edge = make_edge(left, right, p1, p2);
            (*edges).push_back(this_edge);
        }
        
    }
}

/* process_quad()
 * process quadratic bezier curve defined by 3 points (a, b, c)
 * add necessary edges to edge vector (edges)
 */
inline void process_quad(std::vector<Edge>* edges, GPoint a, GPoint b, GPoint c, int width, int height, GMatrix ctm) {
    GPoint e = (a - (b + b) + c) * 0.25;
    float mag = std::sqrt((e.x * e.x) + (e.y * e.y));
    
    int num_segs = int(ceil(std::sqrt(mag * 4)));
    float dt = 1 / float(num_segs);
    
    GPoint src[3] = {a, b, c};

    GPoint p0 = a;
    GPoint p1;
    float t = dt;
    
    for (int i = 1; i < num_segs; i++) {
        p1 = quadT(src, t);
        process_points(edges, p0, p1, width, height, ctm);

        // update state
        p0 = p1;
        t += dt;
    }

    p1 = c;
    process_points(edges, p0, p1, width, height, ctm);
}

/* process_cubic()
 * process cubic bezier curve defined by 4 points (a, b, c, d)
 * add necessary edges to edge vector
 */
inline void process_cubic(std::vector<Edge>* edges, GPoint a, GPoint b, GPoint c, GPoint d, int width, int height, GMatrix ctm) {
    GPoint e0 = a - (b + b) + c;
    GPoint e1 = b - (c + c) + d;
    GPoint e;
    e.x = std::max(std::abs(e0.x), std::abs(e1.x));
    e.y = std::max(std::abs(e0.y), std::abs(e1.y));
    float mag = std::sqrt((e.x * e.x) + (e.y * e.y));
    
    int num_segs = int(ceil(std::sqrt(mag * 3)));
    float dt = 1 / float(num_segs);

    GPoint src[4] = {a, b, c, d};

    GPoint p0 = a;
    GPoint p1;
    float t = dt;
    
    for (int i = 1; i < num_segs; i++) {
        p1 = cubicT(src, t);
        process_points(edges, p0, p1, width, height, ctm);

        // update state
        p0 = p1;
        t += dt;
    }

    p1 = d;
    process_points(edges, p0, p1, width, height, ctm);
}

/* ========== PROCESS EDGES ========== */

/* edge_sort() */
inline bool edge_sort(const Edge& e1, const Edge& e2) {
    if (e1.top == e2.top) {
        return e1.eval_x(float(e1.top + 0.5)) <= e2.eval_x(float(e2.top + 0.5));
    }
    return e1.top < e2.top;
}

/* get_edges() from GPoint* */
inline void get_edges(std::vector<Edge>* edges, const GPoint* points, int count, int width, int height, GMatrix ctm) {

    GPoint p1, p2;

    // edge between point (i) and point (i+1)
    for (int i = 0; i < count - 1; i ++) {
        p1 = points[i];
        p2 = points[i+1];

        process_points(edges, p1, p2, width, height, ctm);
    }

    // first point + last point
    p1 = points[count-1];
    p2 = points[0];
    process_points(edges, p1, p2, width, height, ctm);
    
    // sort edges
    std::sort((*edges).begin(),(*edges).end(),&edge_sort);
}

/* get_edges() from GPath*/
inline void get_edges(std::vector<Edge>* edges, const GPath& path, int width, int height, GMatrix ctm) {
    GRect bounds = path.bounds();
    if (valid_bounds(bounds, width, height)) {

        GPath::Edger e(path);
        GPoint pts[GPath::kMaxNextPoints];

        while (auto v = e.next(pts)) {
            switch (v.value()) {
                
                // line
                case GPathVerb::kLine:
                    process_points(edges, pts[0], pts[1], width, height, ctm);
                    break;

                // quadratic bezier
                case GPathVerb::kQuad:
                    process_quad(edges, pts[0], pts[1], pts[2], width, height, ctm);
                    break;
                
                // cubic bezier
                case GPathVerb::kCubic:
                    process_cubic(edges, pts[0], pts[1], pts[2], pts[3], width, height, ctm);
                    break;
            }
        }

        // sort edges
        std::sort((*edges).begin(), (*edges).end(), &edge_sort);
    }

}

#endif