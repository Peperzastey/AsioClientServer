#ifndef ACS_SERVER_CONTEXT_RESOLVER_HPP__
#define ACS_SERVER_CONTEXT_RESOLVER_HPP__

#include "acs/message/Message.hpp"
#include <map>
#include <utility>

namespace acs::context {

using ContextId = message::Message::ContextId;

/// Generate globally-unique context id.
/**
 * Context-id identify context instances across all ContextResolver types.
 */
// A static local variable in an extern inline function always refers to the same object.
inline ContextId generateNewContextId() {
    static ContextId nextGenCtxId = 0;
    return nextGenCtxId++;
}

template <typename ContextT>
class ContextResolver {
public:
    static ContextResolver& instance() {
        static ContextResolver instance{};
        return instance;
    }

    template <typename... CtorArgs>
    bool addNewContextInstance(const ContextId &id, CtorArgs&&... ctorArgs) { // inline
        auto [iter, success] = _mapping.try_emplace(id, std::forward<CtorArgs>(ctorArgs)...);
        (void)iter;
        return success;
    }

    /*const ContextT& and const method*/
    //TODO decltype(auto) return type ?
    ContextT& getContextInstance(const ContextId &id) { // inline
        return _mapping.at(id);
    }

private:
    ContextResolver() = default;
    ContextResolver(const ContextResolver&) = delete;
    ContextResolver(ContextResolver&&) = delete;

private:
    std::map<ContextId, ContextT> _mapping;
};

} // namespace acs::context

#endif // ACS_SERVER_CONTEXT_RESOLVER_HPP__
