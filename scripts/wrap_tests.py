#!/usr/bin/env python
# coding: utf-8

"""
Builds an OpenCL C wrapper for all test functions in a given file.
"""

import argparse
import sys
import os.path

KERNEL_TEMPLATE = """
kernel void cl_%s(global int *status) {
    *status = %s();
}""" # to be formated with (name, name)

TEST_SIG = "int test_"

def get_names(filename):
    """Return all test names defined in filename."""
    names = []

    with open(filename) as f:
        # Assuming, we put the output into the same directory...


        for line in f.readlines():
            line = line.strip()
            if line.startswith(TEST_SIG):
                try:
                    idx = line.index("()")
                    name = line[:idx].split(" ")[1] # gets rid of "int "
                    names.append(name)
                except ValueError:
                    pass

    return names


def print_wrappers(funcnames):
    for name in funcnames:
        print(KERNEL_TEMPLATE % (name, name))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Extract information from unit tests.")
    parser.add_argument("filenames", metavar="filename", nargs="+",
        help="The filename(s) with unit tests to parse.")
    parser.add_argument('--info', dest="action", action="store_const",
        const="info", default="wrap",
        help="Output C-parseable information (default: output OpenCL wrappers.)")

    args = parser.parse_args()

    if args.action == "wrap":
        for filename in args.filenames:
            funcnames = get_names(filename)
            print_wrappers(funcnames)

    elif args.action == "info":
        testcount = 0
        kernel_names = []
        test_names = []

        for filename in args.filenames:
            print('#include "%s"' % os.path.split(filename)[1])

            funcnames = get_names(filename)
            test_names.extend(funcnames)

            for name in funcnames:
                kernel_names.append("cl_%s" % name)
                testcount += 1

        print("const int test_count = %d;\n" % testcount)
        print("const char *test_names[] = {")
        print(",\n".join(['    "%s"' % n for n in test_names]))
        print("};\n")

        print("const int (*test_functions[])() = {")
        print(",\n".join(['    %s' % n for n in test_names]))
        print("};\n")

        print("const char *kernel_names[] = {")
        print(",\n".join(['    "%s"' % n for n in kernel_names]))
        print("};\n")
