clang                                   \
  -I ../tree-sitter/lib/include            \
  test-js-parser.c                    \
  ../tree-sitter-javascript/src/parser.c         \
  ../tree-sitter-javascript/src/scanner.c         \
  ../tree-sitter/libtree-sitter.a          \
  -o test-js-parser

