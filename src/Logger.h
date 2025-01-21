#include <string>
#include <fstream>

class Logger{

    private:
    std::ofstream _logStream{};

    public:
    Logger() = default;
    Logger(const std::string& logFile);

    void log(const std::string& message);

};