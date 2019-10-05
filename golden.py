#!/usr/bin/python
import os
from subprocess import PIPE, run

C_EXTENSION = ".c"
TEST_DIR = "tests"


def get_c_files(dir):
    for file in os.listdir(dir):
        if file.endswith(C_EXTENSION):
            yield os.path.join(dir, file)


def join_split(str):
    return ' '.join(str.split())


def test_lex(file):
    lex_file = file[:-2] + ".lex"
    expected = ""
    with open(lex_file, "r") as fp:
        expected = join_split(fp.read())
    command = ["./cici", file, "stdout", "lex"]
    result = run(command, stdout=PIPE, universal_newlines=True).stdout
    result = join_split(result)
    return (result == expected, expected, result)


def test_ast(file):
    lex_file = file[:-2] + ".ast"
    expected = ""
    with open(lex_file, "r") as fp:
        expected = join_split(fp.read())
    command = ["./cici", file, "stdout", "parse"]
    result = run(command, stdout=PIPE, universal_newlines=True).stdout
    result = join_split(result)
    return (result == expected, expected, result)


def print_result(file, res):
    (passed, expected, result) = res
    if passed:
        print(f"\033[32m\033[1m{file} Ok\033[0m")
    else:
        print(f"\033[31m\033[1m{file} Fail\033[0m")
        print("\033[32mExpected:\033[0m")
        print(" ", expected)
        print("\033[31mBut found:\033[0m")
        print(" ", result)


if __name__ == "__main__":
    print("Building compiler...\n")
    run(["make"], stdout=PIPE, universal_newlines=True)
    print("Testing lex output...\n")
    c_files = list(get_c_files(TEST_DIR))
    for file in c_files:
        res = test_lex(file)
        print_result(file, res)
    print("\nTesting parse output...\n")
    for file in c_files:
        res = test_ast(file)
        print_result(file, res)
