#include "ProxyServer.h"

int main(){

    ProxyServer server(5432, "127.0.0.1", 5432, "querles.log");
    server.Start();
    return 0;
}