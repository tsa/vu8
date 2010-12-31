#ifndef TSA_VU8_CONTEXT_HPP
#define TSA_VU8_CONTEXT_HPP

#include <v8.h>

#include <string>

#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>

namespace tsa { namespace vu8 {

v8::Handle<v8::Value> LoadModule(const v8::Arguments& args);

struct Context {
    typedef v8::Handle<v8::Value> (*ModuleLoadCallback)();

    bool IsEmpty() const { return context_.IsEmpty(); }
    void Init();
    void RunFile(char const *filename);

    Context(std::string const& libPath);
    ~Context();

  private:
    typedef boost::unordered_map<
        std::string,
        boost::tuple<void *, v8::Persistent<v8::Value> > > modules_t;

    v8::HandleScope                scope_;
    v8::Handle<v8::ObjectTemplate> template_;
    v8::Persistent<v8::Context>    context_;

    modules_t    modules_;
    std::string  libPath_;

    friend v8::Handle<v8::Value> LoadModule(const v8::Arguments& args);
};

} }
#endif
