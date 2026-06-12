#pragma once
#include "../Controllers/SampleController.h"
#include <vector>

namespace View {

class SampleView {
public:
    explicit SampleView(Controller::SampleController& ctrl);
    void run();

private:
    void registerSample();
    void listAll();
    void search();
    void printTable(const std::vector<Model::Sample>& samples);

    Controller::SampleController& ctrl_;
};

} // namespace View
