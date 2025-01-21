#include <string>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Logger.h"
#include "PostgreSQLParser.h"

class ProxyServer{
    private:
    int _serverSocket = 0;
    std::string _dbHost{};
    int _dbPort = 0;
    Logger _logger;
    bool _isRun = false;
    std::vector<pollfd> _pollFds{};

    public:
    ProxyServer(int listenPort, const std::string& dbHost, int dbPort, const std::string& logFile);

    void Start();
    void Stop();

    void AcceptNewConnection();
    void HandleCommunication(int clientSocket, int dbSocket);
    void ProcessPollEvents();

};