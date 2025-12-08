//
// Created by USER on 08.12.2025.
//

#ifndef HARDWARE_H
#define HARDWARE_H
#include <string>

class Hardware {
    public:
    static std::string getCpu();
    static std::string getGpu();
    static std::string getRam();
};

#endif //HARDWARE_H
