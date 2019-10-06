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


def get_expected(name, file):
    expected = ""
    header = "/*" + name + "\n"
    with open(file, "r") as fp:
        capture = False
        for line in fp:
            if line == header:
                capture = True
                continue
            if capture:
                if line == "*/\n":
                    break
                expected += line
    return expected


def test_lex(file):
    expected = join_split(get_expected("LEX", file))
    command = ["./cici", file, "stdout", "lex"]
    result = run(command, stdout=PIPE, universal_newlines=True)
    if result.returncode != 0:
        return ("error", expected, result.stdout)
    result = join_split(result.stdout)
    code = "passed" if result == expected else "failed"
    return (code, expected, result)


def test_ast(file):
    expected = join_split(get_expected("AST", file))
    command = ["./cici", file, "stdout", "parse"]
    result = run(command, stdout=PIPE, universal_newlines=True)
    if result.returncode != 0:
        return ("error", expected, result.stdout)
    result = join_split(result.stdout)
    code = "passed" if result == expected else "failed"
    return (code, expected, result)


def print_result(file, res):
    (code, expected, result) = res
    if code == "passed":
        print(f"\033[32m\033[1m{file} Ok\033[0m")
    elif code == "failed":
        print(f"\033[31m\033[1m{file} Fail\033[0m")
        print("\033[32mExpected:\033[0m")
        print(" ", expected)
        print("\033[31mBut found:\033[0m")
        print(" ", result)
    elif code == "error":
        print(f"\033[31m\033[1m{file} Error\033[0m")
        for line in result.split("\n"):
            print(" ", line)
    return code == "passed"


def main():
    print("Building compiler...\n")
    make_res = run(["make"], stdout=PIPE, universal_newlines=True)
    if make_res.returncode != 0:
        return
    print("Testing lex output...\n")
    c_files = sorted(list(get_c_files(TEST_DIR)))
    for file in c_files:
        res = test_lex(file)
        if not print_result(file, res):
            return
    print("\nTesting parse output...\n")
    for file in c_files:
        res = test_ast(file)
        if not print_result(file, res):
            return


if __name__ == "__main__":
    main()
