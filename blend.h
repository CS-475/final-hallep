#ifndef BLEND_DEFINED
#define BLEND_DEFINED

#include "include/GColor.h"
#include "include/GPaint.h"
#include "include/GPixel.h"
#include "include/GShader.h"

#include <iostream>

/* div255()
 * divides unsigned int (x) by 255
 */
inline unsigned div255(unsigned x) {
    unsigned K = (1 << 16) + (1 << 8) + (1 << 0);
    return (x * K + (1 << 23)) >> 24;
}

/* convertColor2Pixel()
 * converts GColor (color) to GPixel (pixel)
 */
inline GPixel convertColor2Pixel(const GColor& color) {
    unsigned a = unsigned(round(color.a * 255));
    unsigned r = unsigned(round(color.r * color.a * 255));
    unsigned g = unsigned(round(color.g * color.a * 255));
    unsigned b = unsigned(round(color.b * color.a * 255));

    if (b > a) {
        std::cout << "convertColor2Pixel(): ";
        std::cout << "(" << color.a << "," << color.r << "," << color.g << "," << color.b << ")\n";
    }
    GPixel pixel = GPixel_PackARGB(a,r,g,b);

    return pixel;
}

/* BLEND FUNCTIONS */

/* blend_kClear()
 * 0
 */
inline GPixel blend_kClear(const GColor& src, GPixel* dst) {
    return GPixel_PackARGB(0,0,0,0);
}

/* blend_kSrc()
 * S
 */
inline GPixel blend_kSrc(const GColor& src, GPixel* dst) {
    return convertColor2Pixel(src);
}

/* blend_kDst()
 * D
 */
inline GPixel blend_kDst(const GColor& src, GPixel* dst) {
    return *dst;
}

/* blend_kSrcOver()
 * Sx + (1-Sa)*Dx
 */
inline GPixel blend_kSrcOver(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(sa + div255((255 - sa) * da));
    unsigned r = unsigned(sr + div255((255 - sa) * dr));
    unsigned g = unsigned(sg + div255((255 - sa) * dg));
    unsigned b = unsigned(sb + div255((255 - sa) * db));

    return GPixel_PackARGB(a,r,g,b);    
}

inline GPixel blend_kSrcOver(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(sa + div255((255 - sa) * da));
    unsigned r = unsigned(sr + div255((255 - sa) * dr));
    unsigned g = unsigned(sg + div255((255 - sa) * dg));
    unsigned b = unsigned(sb + div255((255 - sa) * db));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kDstOver()
 * Dx + (1-Da)*Sx
 */
inline GPixel blend_kDstOver(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(da + div255((255 - da) * sa));
    unsigned r = unsigned(dr + div255((255 - da) * sr));
    unsigned g = unsigned(dg + div255((255 - da) * sg));
    unsigned b = unsigned(db + div255((255 - da) * sb));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kDstOver(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(da + div255((255 - da) * sa));
    unsigned r = unsigned(dr + div255((255 - da) * sr));
    unsigned g = unsigned(dg + div255((255 - da) * sg));
    unsigned b = unsigned(db + div255((255 - da) * sb));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kSrcIn()
 * Da * Sx
 */
inline GPixel blend_kSrcIn(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);

    // blended color
    unsigned a = unsigned(div255(da * sa));
    unsigned r = unsigned(div255(da * sr));
    unsigned g = unsigned(div255(da * sg));
    unsigned b = unsigned(div255(da * sb));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kSrcIn(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);

    // blended color
    unsigned a = unsigned(div255(da * sa));
    unsigned r = unsigned(div255(da * sr));
    unsigned g = unsigned(div255(da * sg));
    unsigned b = unsigned(div255(da * sb));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kDstIn()
 * Sa * Dx
 */
inline GPixel blend_kDstIn(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255(sa * da));
    unsigned r = unsigned(div255(sa * dr));
    unsigned g = unsigned(div255(sa * dg));
    unsigned b = unsigned(div255(sa * db));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kDstIn(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255(sa * da));
    unsigned r = unsigned(div255(sa * dr));
    unsigned g = unsigned(div255(sa * dg));
    unsigned b = unsigned(div255(sa * db));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kSrcOut()
 * (1-Da)*Sx
 */
inline GPixel blend_kSrcOut(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);

    // blended color
    unsigned a = unsigned(div255((255 - da) * sa));
    unsigned r = unsigned(div255((255 - da) * sr));
    unsigned g = unsigned(div255((255 - da) * sg));
    unsigned b = unsigned(div255((255 - da) * sb));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kSrcOut(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);

    // blended color
    unsigned a = unsigned(div255((255 - da) * sa));
    unsigned r = unsigned(div255((255 - da) * sr));
    unsigned g = unsigned(div255((255 - da) * sg));
    unsigned b = unsigned(div255((255 - da) * sb));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kDstOut()
 * (1-Sa)*Dx
 */
inline GPixel blend_kDstOut(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((255 - sa) * da));
    unsigned r = unsigned(div255((255 - sa) * dr));
    unsigned g = unsigned(div255((255 - sa) * dg));
    unsigned b = unsigned(div255((255 - sa) * db));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kDstOut(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((255 - sa) * da));
    unsigned r = unsigned(div255((255 - sa) * dr));
    unsigned g = unsigned(div255((255 - sa) * dg));
    unsigned b = unsigned(div255((255 - sa) * db));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kSrcATop()
 * (Da*Sx) + (1-Sa)*Dx
 */
inline GPixel blend_kSrcATop(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((da * sa) + ((255 - sa) * da)));
    unsigned r = unsigned(div255((da * sr) + ((255 - sa) * dr)));
    unsigned g = unsigned(div255((da * sg) + ((255 - sa) * dg)));
    unsigned b = unsigned(div255((da * sb) + ((255 - sa) * db)));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kSrcATop(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((da * sa) + ((255 - sa) * da)));
    unsigned r = unsigned(div255((da * sr) + ((255 - sa) * dr)));
    unsigned g = unsigned(div255((da * sg) + ((255 - sa) * dg)));
    unsigned b = unsigned(div255((da * sb) + ((255 - sa) * db)));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kDstATop()
 * (Sa*Dx) + (1-Da)*Sx
 */
inline GPixel blend_kDstATop(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((sa * da) + ((255 - da) * sa)));
    unsigned r = unsigned(div255((sa * dr) + ((255 - da) * sr)));
    unsigned g = unsigned(div255((sa * dg) + ((255 - da) * sg)));
    unsigned b = unsigned(div255((sa * db) + ((255 - da) * sb)));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kDstATop(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255((sa * da) + ((255 - da) * sa)));
    unsigned r = unsigned(div255((sa * dr) + ((255 - da) * sr)));
    unsigned g = unsigned(div255((sa * dg) + ((255 - da) * sg)));
    unsigned b = unsigned(div255((sa * db) + ((255 - da) * sb)));

    return GPixel_PackARGB(a,r,g,b);
}

/* blend_kXor()
 * (1-Sa)*Dx + (1-Da)*Sx
 */
inline GPixel blend_kXor(const GColor& src, GPixel* dst) {
    // new color (to add)
    int sa = int(round(src.a * 255));
    int sr = int(round(src.r * src.a * 255));
    int sg = int(round(src.g * src.a * 255));
    int sb = int(round(src.b * src.a * 255));

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255(((255 - sa) * da) + ((255 - da) * sa)));
    unsigned r = unsigned(div255(((255 - sa) * dr) + ((255 - da) * sr)));
    unsigned g = unsigned(div255(((255 - sa) * dg) + ((255 - da) * sg)));
    unsigned b = unsigned(div255(((255 - sa) * db) + ((255 - da) * sb)));

    return GPixel_PackARGB(a,r,g,b);
}

inline GPixel blend_kXor(GPixel* src, GPixel* dst) {
    // new color (to add)
    int sa = GPixel_GetA(*src);
    int sr = GPixel_GetR(*src);
    int sg = GPixel_GetG(*src);
    int sb = GPixel_GetB(*src);

    // current color
    int da = GPixel_GetA(*dst);
    int dr = GPixel_GetR(*dst);
    int dg = GPixel_GetG(*dst);
    int db = GPixel_GetB(*dst);

    // blended color
    unsigned a = unsigned(div255(((255 - sa) * da) + ((255 - da) * sa)));
    unsigned r = unsigned(div255(((255 - sa) * dr) + ((255 - da) * sr)));
    unsigned g = unsigned(div255(((255 - sa) * dg) + ((255 - da) * sg)));
    unsigned b = unsigned(div255(((255 - sa) * db) + ((255 - da) * sb)));

    return GPixel_PackARGB(a,r,g,b);
}

/* GET BLEND */

/* blend() 
 * mixes new color (GPaint srcPaint) and current color (GPixel dst) based on the blend mode
 * which is returned using the GPaint.getBlendMode() function
*/
inline GPixel blendPaint(const GPaint src, GPixel* dst) {

    switch (src.getBlendMode()) {
        
        case GBlendMode::kClear: // 0
            return blend_kClear(src.getColor(), dst);
        
        case GBlendMode::kSrc: // S
            return blend_kSrc(src.getColor(), dst);

        case GBlendMode::kDst: // D
            return blend_kDst(src.getColor(), dst);

        case GBlendMode::kSrcOver: // S + (1-Sa)*D
            return blend_kSrcOver(src.getColor(), dst);

        case GBlendMode::kDstOver: // D + (1-Da)*S
            return blend_kDstOver(src.getColor(), dst);

        case GBlendMode::kSrcIn: // Da * S
            return blend_kSrcIn(src.getColor(), dst);

        case GBlendMode::kDstIn: // Sa * D
            return blend_kDstIn(src.getColor(), dst);

        case GBlendMode::kSrcOut: // (1-Da)*S
            return blend_kSrcOut(src.getColor(), dst);
        
        case GBlendMode::kDstOut: // (1-Sa)*D
            return blend_kDstOut(src.getColor(), dst);
        
        case GBlendMode::kSrcATop: // Da*S + (1 - Sa)*D
            return blend_kSrcATop(src.getColor(), dst);
        
        case GBlendMode::kDstATop: // Sa*D + (1 - Da)*S
            return blend_kDstATop(src.getColor(), dst);
        
        case GBlendMode::kXor: // (1 - Sa)*D + (1 - Da)*S
            return blend_kXor(src.getColor(), dst);
        
        default:
            return GPixel_PackARGB(0,0,0,0);
    }
}

/* get_optimized_blend()
 * get new GBlendMode based on src.getAlpha()
 */
inline GBlendMode get_optimized_blend(const GPaint& src) {
    float alpha = src.getAlpha();
    GShader* shader = src.peekShader();

    if (shader) {
        if ((*shader).isOpaque()){
            alpha = 1;
        } else {
            alpha = 0;
        }
    }

    switch (src.getBlendMode()) {
        case GBlendMode::kClear: // 0
            return GBlendMode::kClear;
        
        case GBlendMode::kSrc: // S
            return GBlendMode::kSrc;

        case GBlendMode::kDst: // D
            return GBlendMode::kDst;

        case GBlendMode::kSrcOver: // S + (1-Sa)*D
            
            // 0 + (1-0)*D = D
            if (alpha == 0) {
                return GBlendMode::kDst;
            }

            // S + (1-1)*D = S
            if (alpha == 1) {
                return GBlendMode::kSrc;
            }

            return GBlendMode::kSrcOver;

        case GBlendMode::kDstOver: // D + (1-Da)*S

            // D + (1-Da)*0 = D
            if (alpha == 0) {
                return GBlendMode::kDst;
            }

            return GBlendMode::kDstOver;

        case GBlendMode::kSrcIn: // Da * S

            // Da * 0 = 0
            if (alpha == 0) {
                return GBlendMode::kClear;
            }

            return GBlendMode::kSrcIn;

        case GBlendMode::kDstIn: // Sa * D

            // 0 * D = 0
            if (alpha == 0) {
                return GBlendMode::kClear;
            }

            // 1 * D = D
            if (alpha == 1) {
                return GBlendMode::kDst;
            }

            return GBlendMode::kDstIn;

        case GBlendMode::kSrcOut: // (1-Da)*S
            
            // (1-Da)*0 = 0
            if (alpha == 0) {
                return GBlendMode::kClear;
            }

            return GBlendMode::kSrcOut;
        
        case GBlendMode::kDstOut: // (1-Sa)*D
            
            // (1-0)*D = D
            if (alpha == 0) {
                return GBlendMode::kDst;
            }
            
            // (1-1)*D = 0
            if (alpha == 1) {
                return GBlendMode::kClear;
            }

            return GBlendMode::kDstOut;
        
        case GBlendMode::kSrcATop: // Da*S + (1 - Sa)*D

            // Da*0 + (1-0)*D = D
            if (alpha == 0) {
                return GBlendMode::kDst;
            }

            return GBlendMode::kSrcATop;
        
        case GBlendMode::kDstATop: // Sa*D + (1 - Da)*S

            // 0*D + (1-Da)*0 = 0
            if (alpha == 0) {
                return GBlendMode::kClear;
            }

            return GBlendMode::kDstATop;
        
        case GBlendMode::kXor: // (1 - Sa)*D + (1 - Da)*S

            // (1-0)*D + (1-Da)*0 = D
            if (alpha == 0) {
                return GBlendMode::kDst;
            }

            // (1-1)*D + (1-Da)*S
            if (alpha == 1) {
                return GBlendMode::kSrcOut;
            }

            return GBlendMode::kXor;
        
        default:
            return GBlendMode::kClear;
    }
}

/* get_blend_function()
 * returns the blend function associated with the source (src) GPaint
 */
inline GPixel (*get_blend_function(const GBlendMode bm))(const GColor&, GPixel*) {
    switch (bm) {
        
        case GBlendMode::kClear: // 0
            return &blend_kClear;
        
        case GBlendMode::kSrc: // S
            return &blend_kSrc;

        case GBlendMode::kDst: // D
            return &blend_kDst;

        case GBlendMode::kSrcOver: // S + (1-Sa)*D
            return &blend_kSrcOver;

        case GBlendMode::kDstOver: // D + (1-Da)*S
            return &blend_kDstOver;

        case GBlendMode::kSrcIn: // Da * S
            return &blend_kSrcIn;

        case GBlendMode::kDstIn: // Sa * D
            return &blend_kDstIn;

        case GBlendMode::kSrcOut: // (1-Da)*S
            return &blend_kSrcOut;
        
        case GBlendMode::kDstOut: // (1-Sa)*D
            return &blend_kDstOut;
        
        case GBlendMode::kSrcATop: // Da*S + (1 - Sa)*D
            return &blend_kSrcATop;
        
        case GBlendMode::kDstATop: // Sa*D + (1 - Da)*S
            return &blend_kDstATop;
        
        case GBlendMode::kXor: // (1 - Sa)*D + (1 - Da)*S
            return &blend_kXor;
        
        default:
            return &blend_kClear;
    }
}

#endif