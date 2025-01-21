#include <string>

class PostgreSQLParser{
    private:

    public:
    static std::string parse(const char* buffer, size_t len);

};