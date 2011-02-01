#ifndef TSA_VU8_CONTEXT_HPP
#define TSA_VU8_CONTEXT_HPP

#include <vu8/config.hpp>

#include <v8.h>

#include <string>

#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>

namespace vu8 {

v8::Handle<v8::Value> LoadModule(const v8::Arguments& args);

struct ContextScope;

struct Context {
    friend struct ContextScope;

    typedef v8::Handle<v8::Value> (*ModuleLoadCallback)();

    bool IsEmpty() const { return context_.IsEmpty(); }
    void RunFile(char const *filename);

    v8::Handle<v8::Value> operator[](char const *key) {
        return context_->Global()->Get(v8::String::New(key));
    }

  private:
    void Init();

    typedef boost::unordered_map<
        std::string,
        boost::tuple<void *, v8::Persistent<v8::Value> > > modules_t;

    v8::Persistent<v8::Context>    context_;

    modules_t    modules_;
    std::string  libPath_;

    friend v8::Handle<v8::Value> LoadModule(const v8::Arguments& args);

  public:
    Context(std::string const& libPath = VU8_PLUGIN_LIB_PATH);
    ~Context();
};

struct ContextScope {
    ContextScope(Context& context) : context_(context) {
        context_.context_->Enter();
    }
    ~ContextScope() { context_.context_->Exit(); }
  private:
    Context& context_;
};

struct ContextScopeThreadLock : v8::Locker, ContextScope {
    ContextScopeThreadLock(Context& context) : ContextScope(context) {}
  private:
    v8::HandleScope scope;
};

}
#endif
