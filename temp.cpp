#include "canvas.h"
#include "blend.h"
#include "edge.h"

#include <vector>
#include <algorithm>
#include <iostream>

/* edge_sort_x() */
bool edge_sort_x(const Edge& e1, const Edge& e2, float y) {
    return e1.eval_x(float(y + 0.5)) <= e2.eval_x(float(y + 0.5));
}

/* drawPath() */
void MyCanvas::drawPath(const GPath& path, const GPaint& paint) {
    // GBlendMode optBlendMode = get_optimized_blend(paint);
    GBlendMode optBlendMode = paint.getBlendMode();

    if (optBlendMode != GBlendMode::kDst) {
        GMatrix ctm = matrices.top();
        std::vector<Edge> edges;
        
        // std::cout << "\n=========================\n";
        get_edges(&edges,path,fDevice.width(),fDevice.height(),ctm);
    	
        // std::cout << "\nEDGES (" << edges.size() << "): ";
        // for (int i = 0; i < edges.size(); i++) {
        //     std::cout << "(" << edges[i].top << "," << edges[i].bottom << "," << edges[i].w << ") ";
        // }
        // std::cout << "\n";

        // if still a polygon...
        if (edges.size() >= 2) {

            // get top (first row) and bottom (last row)
            int top = fDevice.height();
            int bottom = 0;
            for (int i = 0; i < edges.size(); i++) {
                
                // assign top
                if (edges[i].top < top) {
                    top = edges[i].top;
                }

                // assign bottom
                if (edges[i].bottom > bottom) {
                    bottom = edges[i].bottom;
                }
            }

            GPixel* curr_row = nullptr;
            GShader* shader = paint.peekShader();

            // use shader: paint.peekShader()
            if (shader) {

                bool validContext = (*shader).setContext(matrices.top());
                if (validContext) {

                    for (int y = top; y < bottom; y++) {
                        // std::cout << "\n===== y = " << y << " =====\n";
                        int i = 0;
                        int w = 0;
                        int left, right, count;

                        // for all active edges...
                        while ((i < edges.size()) && (edges[i].isValid(y))) {
                            int thisX = int(round(edges[i].eval_x(float(y + 0.5))));

                            // std::cout << "[i = " << i << "]: thisX = " << thisX << "\n";

                            if (w == 0) {
                                left = thisX;
                            }
                            w += edges[i].w;
                            if (w == 0) {
                                right = thisX;
                                count = right - left;
                                
                                // std::cout << "\tx = [" << left << "," << right << "]\n";

                                if (left < right && left < fDevice.width()) {
                                    curr_row = fDevice.getAddr(left,y);

                                    GPixel src[count];
                                    (*shader).shadeRow(left,y,count,src);
                                    
                                    for (int x = 0; x < count; x++) {

                                        // curr_row[x] = GPixel_PackARGB(1,1,0,0);
                                        // curr_row[x] = src[x];
                                        // /*
                                        switch (optBlendMode) {
                                            case GBlendMode::kClear:
                                                curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                                break;
                                            
                                            case GBlendMode::kSrc:
                                                curr_row[x] = src[x];
                                                break;

                                            case GBlendMode::kSrcOver:
                                                curr_row[x] = blend_kSrcOver(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kDstOver:
                                                curr_row[x] = blend_kDstOver(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kSrcIn:
                                                curr_row[x] = blend_kSrcIn(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kDstIn:
                                                curr_row[x] = blend_kDstIn(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kSrcOut:
                                                curr_row[x] = blend_kSrcOut(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kDstOut:
                                                curr_row[x] = blend_kDstOut(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kSrcATop:
                                                curr_row[x] = blend_kSrcATop(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kDstATop:
                                                curr_row[x] = blend_kDstATop(&src[x],&curr_row[x]);
                                                break;

                                            case GBlendMode::kXor:
                                                curr_row[x] = blend_kXor(&src[x],&curr_row[x]);
                                                break;
                                        }
                                        // */
                                    }
                                }

                            }

                            // check edge validity;
                            // remove edge from vector if no longer valid
                            if (edges[i].isValid(y+1)) {
                                i += 1;
                            } else {
                                edges.erase(edges.begin()+i);
                            }
                        }

                        // assert(w == 0);

                        while ((i < edges.size()) && (edges[i].isValid(y+1))) {
                            i += 1;
                        }

                        std::sort(edges.begin(),edges.begin()+i,[y](const Edge& e1, const Edge& e2) {
                            return edge_sort_x(e1, e2, y);
                        });

                    }
                }
            }

            // use color: paint.getColor()
            else {

                for (int y = top; y < bottom; y++) {
                    curr_row = fDevice.getAddr(0,y);

                    int i = 0;
                    int w = 0;
                    int left, right;

                    // for all active edges...
                    while ((i < edges.size()) && (edges[i].isValid(y))) {
                        int thisX = int(round(edges[i].eval_x(float(y + 0.5))));
                        if (w == 0) {
                            left = thisX;
                        }

                        w += edges[i].w;
                        if (w == 0) {
                            right = thisX;

                            for (int x = left; x < right; x++) {
                                
                                switch (optBlendMode) {
                                    case GBlendMode::kClear:
                                        curr_row[x] = GPixel_PackARGB(0,0,0,0);
                                        break;
                                    
                                    case GBlendMode::kSrc:
                                        curr_row[x] = convertColor2Pixel(paint.getColor());
                                        break;

                                    case GBlendMode::kSrcOver:
                                        curr_row[x] = blend_kSrcOver(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOver:
                                        curr_row[x] = blend_kDstOver(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcIn:
                                        curr_row[x] = blend_kSrcIn(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstIn:
                                        curr_row[x] = blend_kDstIn(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcOut:
                                        curr_row[x] = blend_kSrcOut(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstOut:
                                        curr_row[x] = blend_kDstOut(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kSrcATop:
                                        curr_row[x] = blend_kSrcATop(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kDstATop:
                                        curr_row[x] = blend_kDstATop(paint.getColor(),&curr_row[x]);
                                        break;

                                    case GBlendMode::kXor:
                                        curr_row[x] = blend_kXor(paint.getColor(),&curr_row[x]);
                                        break;
                                }
                                
                            }
                        }

                        // check edge validity:
                        // remove edge from vector if no longer valid
                        if (edges[i].isValid(y+1)) {
                            i += 1;
                        } else {
                            edges.erase(edges.begin()+i);
                        }
                    }

                    // assert(w == 0);

                    while ((i < edges.size()) && (edges[i].isValid(y+1))) {
                        i += 1;
                    }

                    std::sort(edges.begin(),edges.begin()+i,[y](const Edge& e1, const Edge& e2) {
                        return edge_sort_x(e1, e2, y+1);
                    });

                }

            }
        }
    }
}
