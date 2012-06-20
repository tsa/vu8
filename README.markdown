# vu8

vu8 is a project that allows one to give JavaScript access to C++ classes and methods. The binding library is a header only library that uses template-metaprogramming extensively to make binding as easy as possible for the library user.

## vu8 also provides

* libvu8.a - a static library to add "vu8.load" to the v8 javascript context. "vu8.load" is a system for loading plugins from shared libraries.
* v8 cmake - cmake modules to make it easy to build modules for use with "vu8.load". It is recommended that modules use the vu8 meta-programming library to bind C++ to JavaScript but this is not mandatory.
* vu8bin - A binary for running JavaScript files in a context which has vu8 module loading functions provided.

## vu8 module example
```c++
#include <vu8/Module.hpp>

namespace vu8 { namespace console {

v8::Handle<v8::Value> Println(const v8::Arguments& args) { ... }
void Flush() { std::cout.flush(); }

static inline v8::Handle<v8::Value> Open() {
    v8::HandleScope scope;
    Module mod;
    return scope.Close(
        mod("println", &Println)
              .Set<void(), &Flush>("flush")
              .NewInstance());
}

} }
```

## Turning a vu8 module into a vu8 plugin
```c++
extern "C" {
    // vu8_module_init is the entry point
    v8::Handle<v8::Value> vu8_module_init() {
        return vu8::console::Open();
    }
}
```

## Building a vu8 module using vu8 cmake
```
# The following line is neccesary before find_package(Vu8) if the vu8
# cmake files were not installed globally.
# set(CMAKE_MODULE_PATH <path containing FindVu8.cmake>)

find_package(Vu8) # find vu8, define vu8_plugin

if(NOT VU8_FOUND)
    message(FATAL_ERROR "could not find vu8")
endif()

include_directories(${VU8_INCLUDE_DIRS})
vu8_plugin(module_name "source_file1.cpp;source_file2.cpp")
# the above will add the make target and cause the module to be
# installed into the vu8 module path
```


## vu8 class binding example
```c++
#include <vu8/Module.hpp>
#include <vu8/Class.hpp>

struct FileBase {
    bool IsOpen() { return stream_.is_open(); }
    bool Good()   { return stream_.good(); }
    bool Eof()    { return stream_.eof(); }
    void Close()  { stream_.close(); }

  protected:
    std::fstream stream_;
};

struct FileWriter : FileBase {
    bool Open(const std::string& file) { ... }
    void Print(const v8::Arguments& args) { ... }
    void Println(const v8::Arguments& args) { ... }
    FileWriter(const v8::Arguments& args) { ... }
};

struct FileReader : FileBase {
    FileReader(const std::string& file) { ... }
    bool Open(char const *path) { ... }
};

bool Rename(char const *src, char const *dest) {
    return ! std::rename(src, dest);
}

static inline v8::Handle<v8::Value> Open() {
    v8::HandleScope scope;
    vu8::Class<FileBase> fileBase;
    fileBase.Set<void (), &FileBase::Close>("close")
            .Set<bool (), &FileBase::Good>("good")
            .Set<bool (), &FileBase::IsOpen>("is_open")
            .Set<bool (), &FileBase::Eof>("eof")
            ;

    // FileWriter inherits from FileBase
    // Second template argument is a factory.. this one passes the
    // v8::Arguments directly to FileWriter's constructor
    vu8::Class<FileWriter, V8ArgFactory> fileWriter(fileBase);
    fileWriter.Set<bool (const std::string&), &FileWriter::Open>("open")
              .Set<void, &FileWriter::Print>("print")
              .Set<void, &FileWriter::Println>("println")
              ;
    // Print/Println take v8::Arguments directly so only the return type
    // needs to be specified rather than the signature. Arguments and
    // return types are converted from C++ to JS types and back for the
    // user automatically based on the template method signatures.

    // This factory calls FileReader's single argument constructor. It
    // converts v8::Arguments to the appropriate c++ arguments.
    vu8::Class<
        FileReader, Factory<std::string const&>
    > fileReader(fileBase);

    fileReader.Set<bool (char const *), &FileReader::Open>("open");
    // Can convert JS arguments to std::string/char const * amonst other types
    // Code to setup other FileReader methods omitted for brevity

    // Create a module to add classes and functions to and return a
    // new instance of the module to be embedded into the v8 context
    vu8::Module mod;
    return scope.Close(
        mod("Writer", fileWriter)
           ("Reader", fileReader)
           .Set<bool(char const *, char const *), &Rename>("rename")
           .NewInstance());
}
```

## Creating a v8 Context capable of using "vu8.load"
```c++
#include <vu8/Context.hpp>

vu8::Context ctxt;
// script at location jsFile can now use "vu8.load". An application
// that uses vu8::Context must link against libvu8.
ctxt.RunFile(jsFile);
```

## Using "vu8.load" from JavaScript
```javascript
// Load the file module from the class binding example and the
// console module.
var file    = vu8.load('file'),
    console = vu8.load('console')

var writer = new file.Writer("file")
if (writer.is_open()) {
    writer.println("some text")
    writer.close()
    if (! file.rename("file", "newfile"))
        console.println("could not rename file")
}
else console.println("could not open `file'")

console.println("exit")
```

## Create a handle to an externally referenced C++ class.
```c++
// Memory for this won't be touched by Javascript
typedef vu8::Class<MyClass> classWrapper;
v8::Handle<v8::Value> val = classWrapper::ReferenceExternal(&MyClass::instance());
// Assuming MyClass::instance() returns reference to class
```

## Import externally created C++ class into vu8 engine.
```c++
// Memory for object will be reclaimed by Javascript using "delete"
typedef vu8::Class<MyClass> classWrapper;
v8::Handle<v8::Value> val = classWrapper::ImportExternal(new MyClass());
```

## vu8 alternatives
* [v8-juice](http://code.google.com/p/v8-juice/)
    * Has a slightly higher syntactic overhead than vu8.
    * Has been tested on more compilers.

## Using vu8bin to load JavaScript files
    % vu8bin file1.js file2.js  # both files run with the same context

## Dependencies
* boost 1.37+
* cmake 2.4+
* libv8.so (the vu8 build system can help build certain versions of this for you).

## TODO
* Support translating optional arguments.
* v8::Object -> boost::unordered_set translation.
* Node/CommonJS packages
