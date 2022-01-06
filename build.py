# Copyright 2021 Nekos Team
# Published under MIT License

import os
import sys
import re
import pickle
import time

multithreaded = True

if multithreaded:
    from multiprocessing.pool import ThreadPool
    from threading import Lock
    pool = ThreadPool()
    compilation_result_output_lock = Lock()

class DependencyInfo:
    def __init__(self, check_time, dependencies, not_found_dependencies):
        assert(isinstance(check_time, float)) # Returned from os.path.getmtime
        assert(isinstance(dependencies, list)) # List of files
        self.check_time = check_time
        # It's like an identifier of run that created this DependencyInfo
        # Each run build.py creates unique analise_start_time
        # (which is just execution time of analise_dependencies function)
        self.analise_start_time = 0
        self.dependencies = dependencies
        self.not_found_dependencies = not_found_dependencies
        # To prevent recursive search of newer include (updated dependency)
        # If we checked the include and got that it's newer - we just return
        # If we checked the include and got that it's older - we put the (time
        # aganist which we checked the include) into was_tested_on. Then check
        # its dependencies, if they are newer than the time we look for.
        # At some point we can return to this file cause it's recursively included
        # For this case we have time aganist what we checked the include in 
        # was_tested_on field. So we can look at this and know that the file was
        # already tested aganist this specific time. So we don't need to check its
        # dependencies.
        # This resolves recursive include analisys.
        self.was_tested_on = None

def collect_files(ext, folder = "kernel", recursive = True):
    files = []
    for item in os.listdir(folder):
        item = f"{folder}/{item}"
        if item.endswith(ext) and os.path.isfile(item):
            files.append(item)
        if recursive and os.path.isdir(item) and not item.endswith(".") and not item.endswith(".."):
            files += collect_files(ext, folder = item, recursive = recursive)
    return files

def collect_cpp_files(folder = "kernel", recursive = True):
    return collect_files(".cpp", folder = folder, recursive = recursive)

def collect_S_files(folder = "kernel", recursive = True):
    return collect_files(".S", folder = folder, recursive = recursive)

def collect_sources(folder = "kernel", recursive = True):
    return (collect_files(".cpp", folder = folder, recursive = recursive) +
            collect_files(".S", folder = folder, recursive = recursive))

def remove_arch_sources(files):
    return [f for f in files if not f.startswith("kernel/arch")]

def filter_cpp_sources(files):
    return [f for f in files if f.endswith(".cpp")]

def filter_S_sources(files):
    return [f for f in files if f.endswith(".S")]

def source_path_to_object_path(source):
    return f"build/objects/{source}.o"

def get_file_directory(path):
    path = path.replace("\\", "/")
    if "/" in path:
        folder = "/".join(path.split("/")[:-1])
        if folder == "":
            return "/" # It was a file in the root folder
        return folder
    else:
        return "." # Just a filename, let's return current folder

def get_filename(path):
    path = path.replace("\\", "/")
    return path.split('/')[-1]

def create_dirs_for_file(file):
    dir_path = get_file_directory(file)
    os.makedirs(dir_path, exist_ok = True)

def make_boot_first(objects):
    boot_file_index = 0
    for i in range(len(objects)):
        filename = objects[i]
        if f"kernel/arch/{ARCH_FAMILY}/{ARCH}/boot." in filename:
            boot_file_index = i
            break
    objects[0], objects[i] = objects[i], objects[0]
    return objects

def existing_dependencies(paths):
    existing_files = []
    not_found = []
    for path in paths:
        if not os.path.exists(path):
            not_found.append(path)
        else:
            existing_files.append(path)
    return (existing_files, not_found)

# Takes "includes" and source file name and returns
# paths of the includes relative to current working directory
def translate_local_includes_of_file(includes, filename):
    # Get path relative to filename
    includes = [ f"{get_file_directory(filename)}/{i}" for i in includes ]
    # Remove stuff like path/../path
    includes = [ os.path.normpath(i) for i in includes ]
    # Return UNIX-like slashes back (windows-only)
    includes = [ i.replace("\\", "/") for i in includes ]
    return includes

# Takes <includes> and returns paths of the includes
# relative to current working directory
def translate_global_includes(includes):
    return includes

def parse_c_dependencies(filename):
    with open(filename) as c_file:
        contents = c_file.read()
    include_local = re.findall(r'#include\s*"([\w\./\\]+)"', contents)
    include_global = re.findall(r'#include\s*<([\w\./\\]+)>', contents)
    dependencies = []
    dependencies += translate_local_includes_of_file(include_local, filename)
    dependencies += translate_global_includes(include_global)
    (dependencies, not_found) = existing_dependencies(dependencies)
    check_time = time.time()
    return DependencyInfo(check_time, dependencies, not_found)

def parse_gas_dependencies(filename):
    # GNU Assembler uses C preprocessor
    return parse_c_dependencies(filename)

def parse_dependencies(filename):
    if filename.endswith(".cpp") or filename.endswith(".h"):
        return parse_c_dependencies(filename)
    elif filename.endswith(".S") or filename.endswith(".s"):
        return parse_gas_dependencies(filename)
    else:
        print(f"How to parse dependencies of {filename}?")
        exit(-1)

freestanding_includes = [
    "limits.h",
    "float.h",
    "stdint.h",
    "stddef.h",
    "stdbool.h",
    "stdarg.h",
    "stdalign.h",
    "iso646.h",
    "stdlib.h",
]

def analise_file_dependencies(dependency_infos, source, analise_start_time):
    # Prevent recursion: this checks whether dependencies of the file was analized in current run
    if source in dependency_infos and dependency_infos[source].analise_start_time == analise_start_time:
        return

    # If a source (probably include file) does not exist - warn about it and exit
    if not os.path.exists(source):
        print(f"WARNING: {source} not found, but was included and found last time")
        return

    # If we have no dependency info for this file or of the file was changed, scan its dependencies
    filemtime = os.path.getmtime(source)
    if source not in dependency_infos or filemtime > dependency_infos[source].check_time:
        dependency_infos[source] = parse_dependencies(source)

    # Mark the file as analized in current run
    dependency_infos[source].analise_start_time = analise_start_time

    # Then analise dependencies of its dependencies
    for dependency in dependency_infos[source].dependencies:
        analise_file_dependencies(dependency_infos, dependency, analise_start_time)

    # Warn if some dependencies aren't found
    # And if the dependencies aren't freesnanding include files
    not_found_dependencies_that_are_found_now = []
    for path in dependency_infos[source].not_found_dependencies:
        if get_filename(path) in freestanding_includes:
            continue
        # Now it exists, all OK
        if os.path.exists(path):
            not_found_dependencies_that_are_found_now.append(path)
            continue
        print(f"{source}: ({dependency_infos[source].analise_start_time} == {analise_start_time}) WARNING: Can't find included file: {path}, so can't trace it's change\n"
              + "  NOTE: Wrong name of file?\n"
              + "  NOTE: File not in freestanding_includes list?\n"
              + "  NOTE: File is in include (-I) directory?\n"
              + "  TODO: Include file directory (-I) analise isn't implemented\n")

    for found_now in not_found_dependencies_that_are_found_now:
        dependency_infos[source].not_found_dependencies.remove(found_now)
        dependency_infos[source].dependencies.append(found_now)

def analise_dependencies(dependency_infos, sources):
    analise_start_time = time.time()

    for source in sources:
        analise_file_dependencies(dependency_infos, source, analise_start_time)

def has_dependency_newer_than_time(dependency_infos, source, time):
    dependencies = dependency_infos[source].dependencies
    for dependency in dependencies:
        # The file is removed, so compilation result should change
        if not os.path.exists(dependency):
            return True
        if dependency_infos[dependency].was_tested_on == time:
            # We got to this inculude second time
            # And it's older than time
            # Let's skip it
            continue
        if os.path.getmtime(dependency) > time:
            return True
        else:
            dependency_infos[dependency].was_tested_on = time
            result = has_dependency_newer_than_time(dependency_infos, dependency, time)
            if result:
                # Some dependency of the dependency is newer than time
                return True
            else:
                # Found no dependency of the dependency which is newer than time
                # Let's analise next dependency in given dependency list
                continue
    return False

def find_sources_to_be_compiled(dependency_infos, sources):
    to_be_compiled = []
    for source in sources:
        source_object = source_path_to_object_path(source)
        # If no object file created - it obviously should be compiled
        if not os.path.exists(source_object):
            to_be_compiled.append(source)
            continue
        compile_time = os.path.getmtime(source_object)
        source_mtime = os.path.getmtime(source)
        if source_mtime < compile_time and not has_dependency_newer_than_time(dependency_infos, source, compile_time):
            continue
        to_be_compiled.append(source)
    return to_be_compiled

def compile_cpp(source):
    global cppflags
    global source_counter
    global compiling_now_sources
    global sources_to_compile_count

    compiling_now_sources.add(source)
    source_object = source_path_to_object_path(source)
    create_dirs_for_file(source_object)
    os.system(f"clang {cppflags} -I . -c {source} -o {source_object}")
    return source

def compile_S(source):
    global aflags
    global source_counter
    global compiling_now_sources
    global sources_to_compile_count

    compiling_now_sources.add(source)
    source_object = source_path_to_object_path(source)
    create_dirs_for_file(source_object)
    os.system(f"clang++ {aflags} -I . -c {source} -o {source_object}")
    return source

def compile_success(source):
    global source_counter
    global sources_to_compile_count
    global compiling_now_sources
    with compilation_result_output_lock:
        compiling_now_sources.remove(source)
        print(f"[{source_counter}/{sources_to_compile_count}] Building {source}... Done")
        source_counter += 1

def compile_failure(result):
    print(f"FAILURE: {result}")

if __name__ == '__main__':
    is_win32 = True if sys.platform == "win32" else False
    is_linux = True if sys.platform == "linux" or sys.platform == "linux2" else False
    is_osx = True if sys.platform == "darwin" else False

    arch_to_family = {
        "i686": "x86",
    }

    ARCH="i686"
    ARCH_FAMILY = arch_to_family[ARCH]
    cppflags = f"-std=c++17 -ffreestanding -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -fno-rtti -fno-exceptions -fno-threadsafe-statics -fno-strict-aliasing -fno-stack-protector -Wall -Wextra -g -DARCH_{ARCH} --target={ARCH}-unknown-linux-gnu"
    aflags = f"-ffreestanding -g --target={ARCH}-unknown-linux-gnu"
    lflags = f"-ffreestanding -nostdlib"

    sources = collect_sources()
    sources = remove_arch_sources(sources)
    sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}", recursive = False)
    sources += collect_sources(f"kernel/arch/{ARCH_FAMILY}/{ARCH}")

    # Cached dict of "filename" -> DependencyInfo
    # Contains information about a source file (its dependencies and other stuff)
    dependency_infos = {}
    actual_dependency_infos_version = 9
    if os.path.exists("build/dependency_infos.pickle"):
        dependency_infos = pickle.load(open("build/dependency_infos.pickle", "rb"))
        if dependency_infos["\\version//"] < actual_dependency_infos_version:
            os.remove("build/dependency_infos.pickle") # The file is old, get rid of it
            dependency_infos = {}
    analise_dependencies(dependency_infos, sources)
    if not os.path.exists("build"):
        os.mkdir("build")
    dependency_infos["\\version//"] = actual_dependency_infos_version
    pickle.dump(dependency_infos, open("build/dependency_infos.pickle", "wb"))

    sources_to_compile = find_sources_to_be_compiled(dependency_infos, sources)
    sources_to_compile_count = len(sources_to_compile)

    if len(sources_to_compile) == 0 and os.path.exists("kernel/kernel.elf"):
        print("Nothing to be done")
        exit()

    cpp_sources = filter_cpp_sources(sources_to_compile)
    S_sources = filter_S_sources(sources_to_compile)

    compiling_now_sources = set()
    source_counter = 1

    for source in cpp_sources:
        if multithreaded:
            pool.apply_async(compile_cpp, args = (source,), callback = compile_success, error_callback = compile_failure)
        else:
            source_object = source_path_to_object_path(source)
            create_dirs_for_file(source_object)
            print(f"[{source_counter}/{len(sources_to_compile)}] Building {source}...")
            os.system(f"clang {cppflags} -I . -c {source} -o {source_object}")
            source_counter += 1

    for source in S_sources:
        if multithreaded:
            pool.apply_async(compile_S, args = (source,), callback = compile_success, error_callback = compile_failure)
        else:
            source_object = source_path_to_object_path(source)
            create_dirs_for_file(source_object)
            print(f"[{source_counter}/{len(sources_to_compile)}] Building {source}...")
            os.system(f"clang++ {aflags} -I . -c {source} -o {source_object}")
            source_counter += 1

    if multithreaded:
        pool.close()
        pool.join()

    objects = [ source_path_to_object_path(s) for s in sources ]
    make_boot_first(objects)
    objects = " ".join(objects)
    linker_script = f"kernel/arch/{ARCH_FAMILY}/{ARCH}/link.ld"
    print("Linkng...")
    linker = "ld.lld" if is_win32 else "lld"
    os.system(f"{linker} -T {linker_script} -o kernel/kernel.elf {objects}")
