""" Generates a single header file from a file including multiple C/C++ headers.

    Contributed by Sorin Fetche

    Usage:

        python3 generate_single_header.py  --help

        e.g. python3 generate_single_header.py -i boost/leaf/all.hpp -o leaf_all.hpp boost/leaf

    Note:

        If unit tests are build by meson, you can enable the 'build_all_hpp' option (see meson_options.txt),
        in which case each time the tests are built, first all.hpp will be rebuilt (no dependency checks).

"""

import argparse
import os
import re

def _append(input_file_name, input_file, output_file, regex_includes, include_folder, includes_set):
    line_count = 1
    for line in input_file:
        result = regex_includes.search(line)
        line_count += 1
        if result:
            next_input_file_name = result.group("include")
            include_first_time = next_input_file_name not in includes_set
            if include_first_time:
                print("%s" % next_input_file_name, flush=True)
            with open(os.path.join(include_folder, next_input_file_name), "r") as next_input_file:
                if include_first_time:
                    includes_set.add(next_input_file_name)
                output_file.write('\n//>>>>>>>>\n//%s\n//>>>>>>>>\n#line 1 "%s"\n' % \
                    (next_input_file_name, next_input_file_name))
                _append(next_input_file_name, next_input_file, output_file,
                        regex_includes, include_folder, includes_set)
                output_file.write('\n//<<<<<<<<\n//%s\n//<<<<<<<<\n#line %d "%s"\n' % \
                    (next_input_file_name, line_count, input_file_name))
        else:
            output_file.write(line)

def _main():
    parser = argparse.ArgumentParser(
        description="Generates a single include file from a file including multiple C/C++ headers")
    parser.add_argument("-i", "--input", action="store", type=str, default="in.cpp",
                        help="Input file including the headers to be merged")
    parser.add_argument("-o", "--output", action="store", type=str, default="out.cpp",
                        help="Output file. NOTE: It will be overwritten!")
    parser.add_argument("-p", "--path", action="store", type=str, default=".",
                        help="Include path")
    parser.add_argument("prefix", action="store", type=str,
                        help="Non-empty include file prefix (e.g. a/b)")
    args = parser.parse_args()

    regex_includes = re.compile(r"""^\s*#include\s*("|\<)(?P<include>%s.*)("|\>)""" % args.prefix)
    print("Rebuilding %s:" % args.input)
    with open(args.output, 'w') as output_file, open(args.input, 'r') as input_file:
        _append(args.input, input_file, output_file, regex_includes, args.path, set())

if __name__ == "__main__":
    _main()
