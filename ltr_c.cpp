#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <filesystem>

#define VER_MAJ 1
#define VER_MIN 0
#define VER_REV 0

void printHelp() {
    printf(R"(NAME
       LTR-C - Left to right C

SYNOPSIS
       ltr_c [options] [ c_compiler_args... | ltr_c_sources... ]

DESCRIPTION
       Transpiler for ltr source files.

       Regular  C  is  written in a top-to-bottom manner, LTR-C is written
       left-to-write, this tool is a transpiller, it is however able to call on
       your favorite C compiler in order to work seamlessly with your favorite
       build system.

OPTIONS
       --version -v
              Display version number and quit.

       --help -h
              Display help dialog and quit.

       --set-compiler compiler
              Set the compiler command to be used, if not set will try to use
              gcc, clang or the CC environment variable.

       --transpile
              Do not compile, just transpile, should not be used with
              set-compiler, by default creates a new file with the same name as
              the original and appends the ".ltr" extension to it.

       --in-place
              Transpile without creating a new file, overwrite transpiled file
              instead, to be used with transpile.

       --silent
              Don't produce any output to stdout.

       --debug-only
              Don't do nothing, just print debug information.

       --b4 command
              Puts 'command' as an argument before every file.

ENVIRONMENT
       CC
              If no compiler was set and not on transpiling mode $CC will be
              used as the compiler.

DIAGNOSTICS
       The following diagnostics may be issued on stderr: Bad arguments
              When set-compiler and transpile are set or when transpile isn't
              set and in-place is set.
       Compiler errors
              Any compiler error may be written to stderr

       The following diagnostics may be issued on stdout: General file errors
              When a file fails to open.

AUTHOR
       Neondregs (André Botelho) <neondregs@gmail.com>

SEE ALSO
       gcc(1), clang(1)

1.0.0                           2020 Mar 22                             ltr_c(1)
)");
}

class options {
public:
    std::string set_compiler = "";
    bool transpile = false;
    bool in_place = false;
    bool silent = false;
    bool debug_only = false;
    std::string b4 = "";
    std::vector<std::string> native_compiler_args{};
    std::vector<std::pair<std::FILE*, std::string>> files_to_transpile{};

    void print() {
        printf("Options: [\n");
        printf("\tset-compiler: [%s]\n", set_compiler.c_str());
        printf("\ttranspile: %s\n", (transpile)?"YES":"NO");
        printf("\tin-place: %s\n", (in_place)?"YES":"NO");
        printf("\tsilent: %s\n", (silent)?"YES":"NO");
        printf("\tdebug-only: %s\n", (debug_only)?"YES":"NO");
        printf("\tnative_compiler_args: [\n");
        for(auto& nca : native_compiler_args) {
            printf("\t\t%s\n", nca.c_str());
        }
        printf("\t]\n");
        printf("\tfiles_to_transpile: [\n");
        for(auto& ftt : files_to_transpile) {
            printf("\t\t<%s;%s>\n", (ftt.first)?"FILE":"NO FILE", ftt.second.c_str());
        }
        printf("\t]\n");
        printf("]\n");
    }
};

bool transpileFile(std::FILE* source, std::FILE* destination) {
    std::vector<std::string> lines;
    std::string buffer;

    // Copy all lines of file to memory
    int c;
    unsigned max_size = 0;
    while ((c = fgetc(source)) != EOF) {
        if(c == '\n') {
            if(buffer.size() > max_size) max_size = buffer.size();
            lines.emplace_back(std::move(buffer));
            buffer = std::string{};
        } else buffer += c;
    }
    if(buffer.size() != 0) {
        if(buffer.size() > max_size) max_size = buffer.size();
        lines.emplace_back(std::move(buffer));
    }
    if(source != destination) fclose(source);


    //Re-write file
    if(source == destination) {
        destination = freopen(NULL, "w+", destination);
        if(!destination) return false;
    }
    for (unsigned column = 0; column < max_size; ++column) {
        for(auto& line : lines) {
            if(column >= line.size()) fputc(' ', destination);
            else fputc(line[column], destination);
        }
        fputc('\n', destination);
    }
    fclose(destination);
    return true;
}

// Set of helper macros
#define dprint if constexpr (!silent) printf
#define do_action if constexpr (!debug_only)

template<bool silent, bool debug_only> // Removing unnecessary calls may increase performance.
int transpile(options opt) {
    // What mode are we on?
    if(opt.transpile) {
        // In place or not
        if(opt.in_place) {
            for(auto& pair : opt.files_to_transpile) {
                // First -> pointer to file; Second -> Path to file
                dprint("Trying to transpile [%s]...\n", pair.second.c_str());
                if(!pair.first) {
                    dprint("ERROR: COULD NOT OPEN INPUT FILE\n");
                    continue;
                }
                do_action {
                    if(!transpileFile(pair.first, pair.first)) {
                        dprint("ERROR: TRANSPILING FAILED\n");
                    }
                }
                dprint("Transpillation ended for [%s]...\n", pair.second.c_str());
            }
            return 0;
        } else {
            for(auto& pair : opt.files_to_transpile) {
                // First -> pointer to input file; Second -> Path to input file
                dprint("Trying to transpile [%s]...\n", pair.second.c_str());
                std::string out = pair.second.substr(0, pair.second.size()-1) + "ltr";
                dprint("Trying to open output file [%s]...\n", out.c_str());
                do_action {
                    std::FILE* ofile = fopen(out.c_str(), "w");
                    if(!ofile) {
                        dprint("ERROR: COULD NOT OPEN OUTPUT FILE [%s]\n", out.c_str());
                        continue;
                    }
                    if(!pair.first) {
                        dprint("ERROR: COULD NOT OPEN INPUT FILE\n");
                        continue;
                    }
                    if(!transpileFile(pair.first, ofile)) {
                        dprint("ERROR: TRANSPILING FAILED\n");
                    }
                }
                dprint("Transpillation ended for [%s]...\n", pair.second.c_str());
            }
            return 0;
        }
    } else {
        // Now we start converting the source files into the temporary files
        for(auto& pair : opt.files_to_transpile) {
            // First -> pointer to tmp file; Second -> Path to original file
            dprint("Trying to transpile [%s]...\n", pair.second.c_str());
            dprint("Trying to open input file [%s]...\n", pair.second.c_str());
            do_action {
                std::FILE* ifile = fopen(pair.second.c_str(), "r");
                if(!ifile) {
                    dprint("ERROR: COULD NOT OPEN INPUT FILE [%s]\n", pair.second.c_str());
                    return -1;
                }
                if(!pair.first) {
                    dprint("ERROR: COULD NOT OPEN TEMPORARY FILE\n");
                    return -1;
                }
                if(!transpileFile(ifile, pair.first)) {
                    dprint("ERROR: TRANSPILING FAILED\n");
                    return -1;
                }
            }
            dprint("Transpillation ended for [%s]...\n", pair.second.c_str());
        }

        // Now we call the compiler
        dprint("Compiling...\n");
        std::string final_comand = opt.set_compiler;
        for(auto& arg : opt.native_compiler_args) {
            if(arg.size() != 0) final_comand += " " + arg;
            else final_comand += " \"\"";
        }
        dprint("COMPILING COMAND: [%s]\n", final_comand.c_str());
        do_action {
            dprint("Comand output:\n");
            int ret_val = system(final_comand.c_str());
            dprint("Return value: %d\n", ret_val);
            return (ret_val <= 255) ? ret_val : -1;
        } else {
            dprint("Command call skiped...\n");
            return 0;
        }
    }
}

template<bool silent, bool debug_only>
void checkForCompiler(options &opt) {
    if(opt.set_compiler.length() == 0) {
        // We need to check for a compiler
        dprint("Checking for compiler...\n");
        dprint("Checking for gcc...\n");
        FILE* p = popen("gcc --version", "r");
        int exitStat = WEXITSTATUS(pclose(p));
        if(!exitStat) {
            dprint("gcc found on the system!\n");
            opt.set_compiler = "gcc";
            opt.b4 += "-x c";
        } else {
            dprint("Checking for clang...\n");
            p = popen("clang --version", "r");
            exitStat = WEXITSTATUS(pclose(p));
            if(!exitStat) {
                dprint("clang found on the system!\n");
                opt.set_compiler = "clang";
                opt.b4 += "-x c";
            } else {
                dprint("clang not found...\n");
                dprint("WARNING: WILL USE $CC AS DEFAULT COMPILER, MAKE SURE THIS VARIABLE IS ON YOUR ENVIRONMENT!\n");
                opt.set_compiler = "$CC";
            }
        }
    }
}

std::pair<std::FILE*, std::string> getTmpFile() {
    char* f_path = strdup(P_tmpdir "/LTR_C_TMP_XXXXXX");
    std::FILE* f = fdopen(mkstemp(f_path), "r+");
    if(!f_path) f_path = (char*)""; // Must not be null for std::string
    std::string path {f_path};
    delete f_path; // I don't think std::string takes ownership
    return { f , path };
}

int main(int argc, char** argv) {

    // Put strings in string vector
    std::vector<std::string> args;
    args.reserve(argc);
    for(int a = 1; a < argc; ++a) {
        args.push_back(argv[a]);
    }

    // Parse options
    options opt;

    // Transpiller options
    int arg;
    for(arg = 0; arg < args.size(); arg++) {
        if(args[arg] == "--set-compiler") {
            opt.set_compiler = args[++arg];
        } else if (args[arg] == "--transpile") {
            opt.transpile = true;
        } else if (args[arg] == "--in-place") {
            opt.in_place = true;
        } else if (args[arg] == "--silent") {
            opt.silent = true;
        } else if (args[arg] == "--debug-only") {
            opt.debug_only = true;
        } else if (args[arg] == "--b4") {
            opt.b4 = args[++arg];
        } else if (args[arg] == "--help" || args[arg] == "-h") {
            printHelp();
            return 0;
        } else if (args[arg] == "--version" || args[arg] == "-v") {
            printf("LTR-C %d.%d.%d\n", VER_MAJ, VER_MIN, VER_REV);
            return 0;
        } else {
            // We're done with transpiller options
            break;
        }
    }

    // Check for incompatibilities
    if(opt.set_compiler.size() != 0 && opt.transpile) {
        fprintf(stderr, "Bad arguments: --set compiler and --transpile should not be set at the same time.\nAre you trying to transpile or to compile?\n");
        return -1;
    }
    if(!opt.transpile && opt.in_place) {
        fprintf(stderr, "Bad arguments: --in-place only makes sense when used with --transpile.\n");
        return -1;
    }

    // Check for a compiler
    if(opt.silent) {
        if(opt.debug_only) checkForCompiler<true, true>(opt);
        else checkForCompiler<true, false>(opt);
    } else {
        if(opt.debug_only) checkForCompiler<false, true>(opt);
        else checkForCompiler<false, false>(opt);
    }

    // Compiler options
    for(; arg < args.size(); ++arg) {
        if(std::filesystem::is_regular_file(args[arg])) {
            // We have a file
            auto ext = std::filesystem::path(args[arg]).extension();
            // Now depending on wether we are in transpile mode or compile mode we are going to either compile or transpile the file.
            if(opt.transpile) {
                // We need to turn c files into .ltr file
                if(ext == ".c" || ext == ".C") {
                    // C file found
                    opt.files_to_transpile.emplace_back(std::pair{fopen(args[arg].c_str(), "r+"), std::move(args[arg])});
                }
            } else {
                // We need to de-transpile ltr files into regular C files
                if(ext == ".c" || ext == ".C" || ext == ".ltr" || ext == ".LTR" || ext == ".LTR_C"|| ext == ".ltr_c") {
                    // ltr file found
                    // We need to add the original path to files_to_transpile and the temporary file path to native_compiler_args
                    opt.files_to_transpile.push_back(getTmpFile());
                    opt.native_compiler_args.push_back(opt.b4);
                    opt.native_compiler_args.push_back(std::move(args[arg]));
                    std::swap(opt.files_to_transpile.back().second, opt.native_compiler_args.back());
                }
            }
        } else {
            // Compiler arg
            opt.native_compiler_args.push_back(std::move(args[arg]));
        }
    }

    if(opt.debug_only && !opt.silent) opt.print();

    if(opt.silent) {
        if(opt.debug_only) return transpile<true, true>(std::move(opt));
        return transpile<true, false>(std::move(opt));
    } else {
        if(opt.debug_only) return transpile<false, true>(std::move(opt));
        return transpile<false, false>(std::move(opt));
    }
}