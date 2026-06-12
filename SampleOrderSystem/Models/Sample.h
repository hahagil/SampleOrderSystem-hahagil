#pragma once
#include <string>

namespace Model {

struct Sample {
    std::string sampleId;
    std::string name;
    int         unitProductionTime; // min/unit
    double      yield;              // 0.0 ~ 1.0
    int         stock;
};

} // namespace Model
