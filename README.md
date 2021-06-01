# OPaL - OSU Programming Language Compiler

### Table of contents

* [Introduction](#introduction)
* [Features](#features)
* [Copying](#copying)
* [System requirements](#system-requirements)
* [Installation](#installation)
* [Running tests](#running-tests)
* [Usage](#usage)
* [Feedback](#feedback)
* [About](#about)

## Introduction
Class: CS467-400 Spring 2021  Capstone Project

This is the repository for the team to design and develop a compiler using 
C for a dynamically typed language, that is inspired by Python and C.
It produces assembly code modelled after Java bytecode using a 0-address stack machine. 
An orchestrator tool that is used by the end user, runs the individual 
components of the project sequentially and then invokes `nasm` & `ld` to output an 
executable.

* The language syntax specification for the language can be found in the 
`doc/lang-spec.md` document.

* A few examples of programs written and compiled can be found in the `examples` directory.

## Features
The goal of the compiler is not to be the most efficient or create the most 
optimized program, but rather be easy to understand, implement and study. The 
process will help us learn compiler development and understand how a high level 
language can be translated into lower level code to run on a computer.
The compiler is split into following stages:
1. MARC - The preprocessor replaces macros in the source file. The language 
currently supports the \#include directive, which reads in the given file.
2. ALEX - The lexical analyzer reads the source file resulting from MARC and 
creates lexemes for the supported syntax to append to the symbol table.
3. ASTRO - The syntax analyzer walks symbol table created by ALEX and creates a 
abstract syntax tree based on the lanuage grammar.
4. GENIE - The code generator walks the syntax tree created by ASTRO and 
produces [single-address]() assembly code for a 32-bit stack machine.
5. opal - The orchestrator tool used by the end user calls the individual 
components in order to generate an assembly file for the given source file.
It then calls NASM to create an object file, followed by linking the object 
file with `ld` to generate an executable.

The program also generates a number of helpful references, that show the 
individual steps taken by the compiler. A HTML report is generated with the 
output shown from each stage & debug level log messages.

* The project follows the GNU coding standards found 
[here](https://www.gnu.org/prep/standards/standards.html#Formatting)
* The code is commented for [Doxygen](https://www.doxygen.nl) and developer 
documentation can be found [here](https://mckerracher.github.io/OPaL/)

### Copying:
* Clone the [Project git repository](https://github.com/mckerracher/OPaL.git)

### System Requirements:
1. The compiler has been tested to work on Linux x86_86 platform and may possibly
work on other POSIX compliant systems with the required tooling.
2. [glibc](https://www.gnu.org/software/libc/) 2.17 or newer
3. [gcc](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/) 4.8.5 or newer
4. [NASM](https://nasm.us/) 2.10.07 or newer

### Installation:
1. Untar the release file to a directory.
2. Change to the directory and run `make`

### Running tests:
After building, run `make all_tests` to run all the canned tests.

### Usage:
1. Write your program in the OPaL language.
2. Run the compiler `opal` with the argument as your source file.
3. Run with `--help` for usage instructions.

A binary as per the `--output` argument is created or output file is `a.out`.

A compilation report is created as an HTML file as per the `--report` argument or to 
`report/oc_report.html`.


## Feedback
Submit any feedback on [github](https://github.com/mckerracher/OPaL/issues)

## About
- Instructor: [Bill Pfeil](mailto: william.pfeil@oregonstate.edu)
- Team:
    - [Kedar Damle](mailto: damlek@oregonstate.edu)
    - [Josh Mckerracher](mailto: mckerraj@oregonstate.edu)
    - [Sarah Leon](mailto: leons@oregonstate.edu)
