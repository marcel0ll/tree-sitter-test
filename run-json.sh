clang                                   \
  -I ../tree-sitter/lib/include            \
  test-json-parser.c                    \
  ../tree-sitter-json/src/parser.c         \
  ../tree-sitter/libtree-sitter.a          \
  -o test-json-parser

