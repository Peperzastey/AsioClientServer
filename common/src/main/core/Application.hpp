#ifndef APPLICATION_HPP__
#define APPLICATION_HPP__

//#include <memory>
#include <asio/io_context.hpp>

/// Contains core functionality.
namespace cs::core {

/// Main class representing application context.
/**
 * Singleton.
 */
class Application {
public:
    /// IO context typedef.
    using io_context_t = asio::io_context;
    /// Typedef of type used to count the number of handlers executed by the IO context.
    /**
     * \todo try just `using asio::io_context::count_type;`
     */
    using count_type = asio::io_context::count_type;
    //using pointer = std::unique_ptr<Application>;

    /// Get the single instance.
    /**
     * \warning Not Thread-Safe. ?
     * \return Reference to the single instance of this class.
     */
    static Application& instance() {
        //return std::make_unique(); // C++17 // <- factory method, not singleton
        static Application singleInstance;
        return singleInstance;
    }

    /// Run the event loop in the calling thread.
    count_type run() { return _ioContext.run(); }

    /// Get application's shared IO context.
    io_context_t& getContext() { return _ioContext; }

    //TODO method to create customized app context (create(customization-params)) <- like Qt::Application - e.g. pass the argv

private:
    /// Private constructor
    Application() {}
    Application(const Application&) = delete;
    Application(Application&&) = delete;    // necessary ?

protected:
    /// Asio IO context.
    asio::io_context _ioContext;
};

} // namespace cs::core

#endif // APPLICATION_HPP__