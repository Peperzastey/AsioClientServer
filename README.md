# AsioClientServer

### __---Work in progress---__

## About
Client-server application using TCP protocol.  
Server accepts both IPv6 and IPv4(through v4-mapped-on-v6*) connections.   
Both client and server execute on a single thread using Asio's event loop  
(which in turn uses `epoll` system calls - on Linux kernels (v.2.6+); and reactor pattern).

\* v4-mapped-on-v6 is not available on Windows XP

### Functions:
- echo command
- (in progress) chat rooms
- (todo) a real-time game

### ToDo-s:
- use thread pool in server app to execute the async-op completion handlers concurrently

### Usage:
1. Run the server app
```
> ./server [listen-port]
```
2. Run clients apps
```
> ./client [server-listen-port] [server-host]
```
You can close the _server_ app by sending the `SIGINT` signal to it. `Ctrl`+`C` from the terminal running the server in foreground.

You can close the _client_ app by either sending `SIGINT` to it or by registering a `EOF` on its input which will terminate the app's command-input loop (`Ctrl`+`D` from the terminal running the client in foreground).

#### Command line arguments default values:
- `listen-port`, `server-listen-port`: &nbsp;`54321`
- `server-host`: &nbsp;`::1` (only a valid IPv6 or IPv4 address)

__NOTE:__ the server app listens on all(any) network interfaces (as per [`in6addr_any` on Linux](http://man7.org/linux/man-pages/man7/ipv6.7.html)).

## Requirements
- POSIX-compliant OS**
- C++17-compliant compiler
- [Asio](http://think-async.com/Asio/) ([standalone](https://github.com/chriskohlhoff/asio))
- [Protobuf](https://github.com/protocolbuffers/protobuf) compiler and libraries (for C++)

\*\* due to using POSIX-specific Asio classes for async operations on stdin and stdout  
There are, however, Windows-specific counterparts that may be used in this project someday, enabling it to also be run on Windows

## Other requirements
- CMake 3.9+ (build system) with CTest
- [GTest & GMock](https://github.com/google/googletest) (when building tests)
- Doxygen (for generating documentation)

## Build instruction
```
> mkdir build && cd build
> cmake [cmake flags] ..
> make
```
By default the provided cmake configuration builds and runs all the tests.  
You can disable these actions by providing the `-DBUILD_TESTING=OFF` CMake flag, like so:
```
> cmake -DBUILD_TESTING=OFF ..
```
This way only the _client_ and _server_ executables are built.

## Run tests
Precondition: build tests.
Run `make test` or `ctest` from the _build_ directory.

You can also run test executable directly (also from the _build_ dir):
```
> ./tests [gtest/gmock flags]
```  
This option gives you the possibility to provide additional flags (e.g. to only list test cases names or filter tests to run) and outputs detailed test results.

You can find available command line flags here:
- [gtest flags](https://github.com/google/googletest/blob/master/googletest/docs/advanced.md#running-test-programs-advanced-options)
- [gmock flags](https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md#flags)

## Generate docs
```
# run in the project root dir
> doxygen docs/Doxyfile
```
This command with the provided `Doxyfile` generates _html_ and _latex_ documentation  
in the `build/docs/html` and `build/docs/latex` directories respectively.

## Helpful links for new users
- [installing CMake](https://cliutils.gitlab.io/modern-cmake/chapters/intro/installing.html)
