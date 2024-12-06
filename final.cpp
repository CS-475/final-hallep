#include "final.h"
#include "veronoi_shader.h"
#include "linear_pos_gradient.h"

/* Veronoi Shader */
std::shared_ptr<GShader> Final::createVoronoiShader(const GPoint points[], const GColor colors[], int count) {
    return std::shared_ptr<GShader>(new VeronoiShader(points, colors, count));
}

/* Linear Position Gradient */
std::shared_ptr<GShader> Final::createLinearPosGradient(GPoint p0, GPoint p1, const GColor colors[], const float pos[], int count) {
    return std::shared_ptr<GShader>(new LinearPosGradient(p0, p1, colors, pos, count));
}

/* GCreateFinal() */
std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new Final());
}
