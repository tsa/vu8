#ifndef TSA_VU8_CONTEXT_HPP
#define TSA_VU8_CONTEXT_HPP

#include <vu8/config.hpp>
#include <vu8/Module.hpp>

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

    // Returns false on failure, use v8::TryCatch around it to find out why.
    bool RunFile(char const *filename);

    // Same as RunFile but for use inside a pre-existing handle scope.
    bool RunFileInScope(char const *filename);

    void Set(char const *name, Module& module) {
        context_->Global()->Set(v8::String::New(name), module.NewInstance());
    }

    v8::Handle<v8::Value> operator[](char const *key) {
        return context_->Global()->Get(v8::String::New(key));
    }

    void Init();

    // Release all tracked data without freeing it, use if running dlclose on
    // library that contains v8 object to avoid core dump.
    void Release();

  private:
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
