#include <random>
#include <sstream>

namespace uuid {

    std::string generate_uuid() {
    	esp_random();
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << random(0, 15);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << random(0, 15);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << random(0, 15);
        }
        ss << "-";
        ss << random(8, 11);
        for (i = 0; i < 3; i++) {
            ss << random(0, 15);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << random(0, 15);
        };
        return ss.str();
    }
}
