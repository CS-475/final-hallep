#ifndef FINAL_DEFINED
#define FINAL_DEFINED

#include "include/GFinal.h"

class Final : public GFinal {
public:

    Final() {}

    std::shared_ptr<GShader> createVoronoiShader(const GPoint points[], const GColor colors[], int count) override;

};

#endif
