# vu8

vu8 is a project that allows one to give JavaScript access to C++ classes and methods. The binding library is a header only library that uses template-metaprogramming extensively to make binding as easy as possible for the library user.

## vu8 also provides

* libvu8.a - a static library to add "loadmodule" to the v8 javascript context. loadmodule is a system for loading plugins from shared libraries.
* v8 cmake - cmake modules to make it easy to build modules for use with loadmodule. It is recommended that modules use the vu8 meta-programming library to bind C++ to JavaScript but this is not mandatory.

## vu8 class binding example
    struct FileBase {
        bool IsOpen() { return stream_.is_open(); }
        bool Good()   { return stream_.good(); }
        bool Eof()    { return stream_.eof(); }
        void Close()  { stream_.close(); }

      protected:
        std::fstream stream_;
    };

    struct FileWriter : FileBase {
        bool Open(char const *str) { ... }
        void Print(const v8::Arguments& args) { ... }
        FileWriter(const v8::Arguments& args) { ... }
    };

    struct FileReader : FileBase { ...  };

    static inline v8::Handle<v8::Value> Open() {
        v8::HandleScope scope;
        vu8::BasicClass<FileBase> fileBase;
        fileBase.Method<void (), &FileBase::Close>("close")
                .Method<bool (), &FileBase::Good>("good")
                .Method<bool (), &FileBase::IsOpen>("is_open")
                .Method<bool (), &FileBase::Eof>("eof")
                ;

        // FileWriter inherits from FileBase
        vu8::Class<FileWriter> fileWriter(fileBase);
        fileWriter.Method<bool (char const *), &FileWriter::Open>("open")
                  .Method<void, &FileWriter::Print>("print")
                  .Method<void, &FileWriter::Println>("println")
                  ;
        // Print takes v8::Arguments directly so only the return type
        // needs to be specified rather than the signature. Arguments and
        // return types are converted from C++ to JS types and back for the
        // user automatically based on the template method signatures.

        vu8::Class<FileReader> fileReader(fileBase);
        // ... more code


        // Create a module to add the classes to and return a new
        // instance of the module to be embedded into the v8 context
        vu8::Module mod;
        return mod("Writer", fileWriter)
                  ("Reader", fileReader).NewInstance();
    }

## vu8 module example
    #include <vu8/Module.hpp>
    #include <iostream>

    namespace vu8 { namespace console {

    v8::Handle<v8::Value> Println(const v8::Arguments& args) { ... }

    static inline v8::Handle<v8::Value> Open() {
        v8::HandleScope scope;
        Module mod;
        mod("println", &Println);
        return mod.NewInstance();
    }

    } }

    extern "C" {
        // vu8_module_<modname> is the entry point
        v8::Handle<v8::Value> vu8_module_console() {
            return vu8::console::Open();
        }
    }

## Building a vu8 module using vu8 cmake
    find(Vu8)
    set(CMAKE_MODULE_PATH ${VU8_MODULE_PATH})
    include(Vu8)
    vu8_plugin(console "file1.cpp;file2.cpp")
    # the above will add the make target and cause the module to be
    # installed into the vu8 module path

## Creating a v8 Context capable of using loadmodule
    #include <vu8/Context.hpp>
    vu8::Context ctxt;
    // script at location jsFile can now use loadmodule. An application
    // that uses vu8::Context must link against libvu8.a
    ctxt.RunFile(jsFile);

## Using loadmodule from JavaScript
    // Load the file module from the class binding example and the
    // console module.
    var file    = loadmodule('file'),
        console = loadmodule('console')

    var writer = new file.Writer("file")
    if (writer.is_open()) {
        writer.println("some text")
        writer.close()
    }
    else console.println("could not open `file'")

    console.println("exit")
