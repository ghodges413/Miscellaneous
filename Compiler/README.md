# Compiler

This is my attempt at writing a basic C compiler.

The basic anatomy of a compiler:

- The scanner converts the raw text into tokens
- The parser converts the tokens to the abstract syntax tree (AST) - bonus output code formed from AST to check that it is working
- Semantic Analysis - The AST is checked for errors
    --|-> Symbol Table
    --|-> Name Resolution
    --|-> Type checking
- The AST is converted into an intermediate representation (IR)
- The IR is passed through multiple optimization phases	(we may want to consider using the same IR as llvm)
- The IR is converted to the target assembly language

The final test for a compiler is to re-compile it from its own source code.  This toy compiler currently uses C++ features, so that's not possible at the moment.