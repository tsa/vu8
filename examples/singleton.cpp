// Example of binding a singleton class to a JavaScript object using the vu8 singleton class template.
// Compile: g++ singleton.cpp -o singleton -lv8 -lpthread
// Author: Steven Webb - https://github.com/swebb
// Adapted from http://code.google.com/apis/v8/get_started.html

#include <iostream>
#include <string>
#include <v8.h>
#include <vu8/Class.hpp>
#include <boost/utility.hpp>

using namespace std;
using namespace v8;
using namespace vu8;

class Console : private boost::noncopyable
{
  public:

    static Console & instance()
    {
      static Console c;
      return c;
    };

    void log(const string &s)
    {
      cout << s << endl;
    };

  private:

    Console() {};

};

int main(int argc, char* argv[])
{
  HandleScope handle_scope;
  Persistent<Context> context = Context::New();
  Context::Scope context_scope(context);

  HandleScope singleton_scope;
  Singleton<Console> console(&(Console::instance()));
  console.Set<void (const string &s), &Console::log>("log");
  context->Global()->Set(v8::String::New("console"), singleton_scope.Close(console.NewInstance()));

  Handle<String> source = String::New("console.log('test string')");
  Handle<Script> script = Script::Compile(source);
  script->Run();

  context.Dispose();
  return 0;
}
