#include "core/Application.hpp"
#include "conn/tcp/AsyncTcpServer.hpp"
#include <system_error>
#include <iostream>

using namespace cs; //TODO cs::common
using namespace cs::server;

/// The entry point to the server application.
/**
 * \todo use argv for \a portNumber and \a service
 */
int main(int argc, char *argv[]) {
    auto& app = core::Application::instance();

    try {
        conn::tcp::AsyncTcpServer tcpServer(app, 54321, "daytime");
        app.run();
    } catch (const std::system_error &err) {
        std::cerr << "EXCEPTION CAUGHT:\n"
                << err.code() << ": " << err.what()
                << std::endl;
    }

    return 0;
}
