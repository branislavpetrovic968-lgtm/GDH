#include <string>

namespace GDH { namespace Utils {
    
    namespace String {
        std::string toLowerCase(const std::string& input);
        std::string toUpperCase(const std::string& input);
        std::string replaceChar(const std::string& input, char from, char to);
    }

    std::string formatTime(double sec, bool ms);
    void updateCursorState(bool show);
    void UncompleteLevel();

    float easeInOut(float t);
    void hsvToRgb(float h, float s, float v, float &r, float &g, float &b);

    float getFps(bool only_read = false);
    void setPitchShifter(int semitones);
    void setReverb(float decayTime);
} }
