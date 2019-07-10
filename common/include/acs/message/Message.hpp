#ifndef ACS_MESSAGE_HPP__
#define ACS_MESSAGE_HPP__

namespace acs::message {

/// Abstract base class (interface) for concrete Messages.
/**
 * Concrete Message types implement their handle() methods,
 * which the receiver of given message executes on reception.
 */
class Message {
public:
    //TODO multi-method ?
    virtual void handle(/*Sender &sender, Context &context*/) = 0;
    virtual ~Message() {}
};

//TODO
struct Context {
    //? Sender &sender;
    //...
};

} // namespace acs::message

#endif // ACS_MESSAGE_HPP__
