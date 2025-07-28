#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./fp_simulator <input_trace> <output_csv>\n";
        return 1;
    }

    std::string input_trace = argv[1];
    std::string output_csv = argv[2];

    // TODO: Parse input_trace
    // TODO: Run simulation
    // TODO: Write results to output_csv

    return 0;
}

