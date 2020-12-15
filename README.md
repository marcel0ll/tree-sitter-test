# Testing tree sitter

To run this code you will need:
- tree-sitter cloned at ../tree-sitter
- tree-sitter-javascript cloned at ../tree-sitter-javascript
- tree-sitter-json cloned at ../tree-sitter-json

# Idea

The idea is to use tree-sitter incremental parsing to build an incremental code
(re)generator such as an incremental transpiler or incremental linter.

Will this enable incremental building of resources in bundlers and SSG? 
I believe it might, but I may be wrong.

In any way it is good to practice C.


# State of this code

Currently this code is kind broken, I am struggling with strings dynamic 
reallocation in C and probably making some mistake with pointers and values.


## node-version

There is also a first experiment using tree-sitter node bindings
