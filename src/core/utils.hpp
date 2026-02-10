#include <string>

namespace GDH { namespace Utils {

    namespace String {
        std::string toLowerCase(const std::string& input);
        std::string toUpperCase(const std::string& input);
        std::string replaceChar(const std::string& input, char from, char to);
    }

    std::string formatTime(double sec, bool ms);
    void updateCursorState(bool show);
} }