#include "final.h"
#include "veronoi_shader.h"

std::shared_ptr<GShader> Final::createVoronoiShader(const GPoint points[], const GColor colors[], int count) {
    return std::shared_ptr<GShader>(new VeronoiShader(points, colors, count));
}

/* GCreateFinal() */
std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new Final());
}
