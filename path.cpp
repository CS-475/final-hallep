#include "edge.h"
#include "bezier.h"
#include "include/GPathBuilder.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

/* ========== GPathBuilder ========== */

/* addRect() */
void GPathBuilder::addRect(const GRect& r, GPathDirection dir) {
    GPoint tl = {r.left,r.top};
    GPoint tr = {r.right,r.top};
    GPoint bl = {r.left,r.bottom};
    GPoint br = {r.right,r.bottom};

    // starting point: top left
    moveTo(tl);

    switch (dir) {

        // CLOCKWISE
        case GPathDirection::kCW:
            lineTo(tr);
            lineTo(br);
            lineTo(bl);
            break;
        
        // COUNTER-CLOCKWISE
        case GPathDirection::kCCW:
            lineTo(bl);
            lineTo(br);
            lineTo(tr);
            break;
    }
}

/* addPolygon() */
void GPathBuilder::addPolygon(const GPoint pts[], int count) {
    // starting point
    moveTo(pts[0]);

    for (int i = 1; i < count; i++) {
        lineTo(pts[i]);
    }
}

/* addCircle() */
void GPathBuilder::addCircle(const GPoint center, float radius, GPathDirection dir) {
    float v = 0.551915f;
    GPoint unit[13] = { {1,0}, {1,v}, {v,1}, {0,1}, {-v,1}, {-1,v}, {-1,0}, {-1,-v}, {-v,-1}, {0,-1}, {v,-1}, {1,-v}, {1,0}};
    GPoint pts[13];

    GMatrix mx = GMatrix::Translate(center.x, center.y) * GMatrix::Scale(radius, radius);
    mx.mapPoints(pts,unit,13);

    // starting point
    moveTo(pts[0]);

    switch (dir) {

        // CLOCKWISE
        case GPathDirection::kCW:
            for (int i = 1; i < 12; i+=3) {
                cubicTo(pts[i], pts[i+1], pts[i+2]);
            }
            break;

        // COUNTER-CLOCKWISE
        case GPathDirection::kCCW:
            for (int i = 11; i >= 0; i-= 3) {
                cubicTo(pts[i], pts[i-1], pts[i-2]);
            }
            break;
    }
}

/* ========== GPath ========== */

/* ChopQuadAt()
 * dst[0] = A
 * dst[1] = ptT(A, B, t)
 * dst[2] = quadT(t)
 * dst[3] = ptT(B, C, t)
 * dst[4] = C
 */
void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    dst[0] = src[0];
    dst[1] = ptT(src[0], src[1], t);
    dst[2] = quadT(src, t);
    dst[3] = ptT(src[1], src[2], t);
    dst[4] = src[2];
}

/* ChopCubicAt()
 * dst[0] = A
 * dst[1] = ptT(A, B, t)
 * dst[2] = ptT(AB, BC, t)
 * dst[3] = cubicT(t)
 * dst[4] = ptT(BC, CD, t)
 * dst[5] = ptT(C, D, t)
 * dst[6] = D
 */
void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    dst[0] = src[0];
    dst[1] = ptT(src[0], src[1], t);

    dst[3] = cubicT(src, t);
    
    dst[5] = ptT(src[2], src[3], t);
    dst[6] = src[3];

    GPoint bc = ptT(src[1], src[2], t);

    dst[2] = ptT(dst[1], bc, t);
    dst[4] = ptT(bc, dst[5], t);
}

/* bounds() */
GRect GPath::bounds() const {
    // no points
    if (fPts.size() == 0) {
        return {0,0,0,0};
    }

    float top, bottom, left, right;
    top = bottom = fPts[0].y;
    left = right = fPts[0].x;

    GPath::Edger e(*this);
    GPoint pts[GPath::kMaxNextPoints];
    
    // for all segments all segments
    while (auto v = e.next(pts)) {

        // line
        if (v.value() == GPathVerb::kLine) {
            // check top
            if (pts[1].y < top) {
                top = pts[1].y;
            }

            // check bottom
            if (pts[1].y > bottom) {
                bottom = pts[1].y;
            }
            
            // check left
            if (pts[1].x < left) {
                left = pts[1].x;
            }

            // check right
            if (pts[1].x > right) {
                right = pts[1].x;
            }
        }

        // bezier curve
        else {
            
            GRect b;

            // get bounds of curve
            switch (v.value()) {

                // quadratic bezier
                case GPathVerb::kQuad:
                    b = quadBounds(pts);
                    break;

                // cubic bezier
                case GPathVerb::kCubic:
                    b = cubicBounds(pts);
                    break;
            }
            
            // check top
            if (b.top < top) {
                top = b.top;
            }

            // check bottom
            if (b.bottom > bottom) {
                bottom = b.bottom;
            }

            // check left
            if (b.left < left) {
                left = b.left;
            }

            // check right
            if (b.right > right) {
                right = b.right;
            }

        }
    }
    
    return GRect::LTRB(left,top,right,bottom);
}
