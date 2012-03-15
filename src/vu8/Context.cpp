#include <vu8/Context.hpp>
#include <vu8/Throw.hpp>
#include <vu8/Module.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <dlfcn.h>

namespace vu8 {

v8::Handle<v8::Value> LoadModule(const v8::Arguments& args) {
    v8::HandleScope scope;
    if (1 != args.Length())
        return scope.Close(Throw("loadmodule: incorrect arguments"));

    v8::String::Utf8Value str(args[0]);
    std::string modName = *str;

    v8::Handle<v8::Value> ctxtValue =
        args.Holder()->Get(v8::String::New("context"));

    if (ctxtValue.IsEmpty() || ! ctxtValue->IsExternal()) {
        return scope.Close(
            Throw("loadmodule: context is set up incorrectly"));
    }

    Context& context =
        *reinterpret_cast<Context *>(v8::External::Unwrap(ctxtValue));

    typedef Context::modules_t modules_t;
    modules_t::iterator it = context.modules_.find(modName);
    // check if module is already loaded
    if (it != context.modules_.end())
        return scope.Close(boost::get<1>(it->second));

    std::string modPath = context.libPath_;
    modPath.append("/libvu8_").append(modName).append(".so");
    void *dl = dlopen(modPath.c_str(), RTLD_LAZY);

    if (! dl) {
        return scope.Close(
            Throw("loadmodule: could not find shared library"));
    }

    void *sym = dlsym(dl, "vu8_module_init");
    if (! sym) {
        return scope.Close(
            Throw("loadmodule: initialisation function not found"));
    }

    // g++ 3 is broken and can only handle a C-style cast for this
#if 0
    v8::Handle<v8::Value> value =
        (*reinterpret_cast<Context::ModuleLoadCallback>(sym))();
#else
    v8::Handle<v8::Value> value = ((Context::ModuleLoadCallback) sym)();
#endif

    std::pair<modules_t::iterator, bool> ret =
        context.modules_.insert(modules_t::value_type(
            modName, boost::make_tuple(
                dl, v8::Persistent<v8::Value>::New(value))));
#if 0
    // not sure if closing over persistent handle is a problem or necessary
    return boost::get<1>(ret.first->second);
#else
    return scope.Close(boost::get<1>(ret.first->second));
#endif
}

namespace {
    void initContext(Context& context, v8::Persistent<v8::Context>& v8Context) {
        v8Context = v8::Context::New();
        v8Context->Enter();

        Module mod;
        mod("load", &LoadModule)
           ("context", &context)
           ;

        v8Context->Global()->Set(v8::String::New("vu8"), mod.NewInstance());
    }

    struct UnlockAtExit {
        UnlockAtExit(v8::Persistent<v8::Context>& context) : context_(context) {}
        ~UnlockAtExit() { context_->Exit(); }
      private:
        v8::Persistent<v8::Context>& context_;
    };
}

void Context::Init() {
    initContext(*this, context_);
    context_->Exit();
}

void Context::RunFile(char const *filename) {
    v8::HandleScope scope;
    UnlockAtExit unlockCtxt(context_);
    if (IsEmpty()) initContext(*this, context_);
    else context_->Enter();

    std::ifstream stream(filename);
    if (! stream) {
        std::string error = "could not locate file ";
        error.append(filename);
        throw std::runtime_error(error);
    }

    std::stringstream scriptStream;
    std::string line;
    while (! stream.eof()) {
        std::getline(stream, line);
        scriptStream << line << '\n';
    }

    std::auto_ptr<v8::ScriptOrigin> origin(
        new v8::ScriptOrigin(v8::String::New(filename)));

    v8::Handle<v8::Script> script = v8::Script::Compile(
        v8::String::New(scriptStream.str().c_str()), origin.get());
    script->Run();
}

Context::Context(std::string const& libPath) : libPath_(libPath) {}

Context::~Context() {
    if (! IsEmpty()) {
        for (modules_t::iterator it = modules_.begin();
             it != modules_.end(); ++it)
        {
            boost::get<1>(it->second).Dispose();
            dlclose(boost::get<0>(it->second));
        }
        modules_.clear();
        context_.Dispose();
    }
}

}
