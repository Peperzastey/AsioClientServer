#include "core/Application.hpp"
#include "conn/tcp/SyncTcpClient.hpp"
#include <system_error>
#include <iostream>

using namespace cs;

/// The entry point to the client application.
/**
 * \todo use argv for remoteHost and service
 */
int main(int argc, char *argv[]) {
    auto& app = core::Application::instance();

    try {
        conn::tcp::SyncTcpClient tcpClient(app, "localhost", "daytime");
        tcpClient.receiveInfinitely(std::cout);
    } catch (const std::system_error &err) {
        std::cerr << "EXCEPTION CAUGHT:\n"
                << err.code() << ": " << err.what()
                << std::endl;
    }

    return 0;
}
