// Filename - test-json-parser.c
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>

typedef struct {
  char * code;
  const char * source;
  int * counter;
} GenContext;

char * get_text (uint32_t start, uint32_t end, const char * source) {
  uint32_t len = (end - start);
  char * text = (char *)malloc((len + 1) * sizeof(char));

  uint32_t i = 0;
  for (i = 0; i < len; i++) {
    char c = source[start + i];
    text[i] = c;
  }
  text[i] = 0;

  return text;
}

char * ts_node_text (TSNode node, const char * source) {
  char * code = get_text(ts_node_start_byte(node), ts_node_end_byte(node), source);
  return code;
}

TSLanguage *tree_sitter_javascript();

void noop () {}

void * node_keyword (TSNode node, char * text) {
  char * new_text = ts_node_type(node);
  size_t size = strlen(text);
  size_t more = strlen(new_text);
  size_t new_size = size + more + 1;

  text = realloc(text, new_size * sizeof(char));
  if (text == NULL) {
    printf("memory allocation failure\n");
  }
  for(int i = 0; i<more; i++) {
    text[i + size] = new_text[i];
  }
  text[new_size]='\0';
}

void * node_spaced_keyword (TSNode node, char * text) {
  char * new_text = ts_node_type(node);
  size_t size = strlen(text);
  size_t more = strlen(new_text);
  size_t new_size = size + more + 1;

  text = realloc(text, new_size * sizeof(char));
  if (text == NULL) {
    printf("memory allocation failure\n");
  }
  for(int i = 0; i < more; i++) {
    text[i + size] = new_text[i];
  }
  text[new_size]='\0';
}

void * node_class (TSNode node, char * text, int * counter) {
  /* *counter = ((*counter) + 1) % 2; */
  printf("Counter: %d\n", *counter);
  printf("String: %s\n", text);

  if (!*counter) {
    char new_text[] = "class ";
    size_t size = strlen(text);
    size_t more = strlen(new_text);
    size_t new_size = size + more + 1;
    printf("nString: %s\n", new_text);
    printf("d: %d %d %d\n", size, more, new_size);

    text = realloc(text, new_size * sizeof(char));
    if (text == NULL) {
      printf("memory allocation failure\n");
    }
    printf("String: %s\n", text);
    for(int i = 0; i < more; i++) {
      text[i + size] = new_text[i];
      printf("String: %s\n", text);
    }
    text[new_size]='\0';
    printf("last: %s\n", text);
  }
}

void * node_text (TSNode node, char * text, const char * source) {
  char * new_text = ts_node_text(node, source);
  size_t size = strlen(text);
  size_t more = strlen(new_text);
  size_t new_size = size + more + 1;

  text = realloc(text, new_size * sizeof(char));
  if (text == NULL) {
    printf("memory allocation failure\n");
  }
  for(int i = 0; i<more; i++) {
    text[i + size]= new_text[i];
  }
  text[new_size]='\0';
}

void * node_noop (TSNode node, char * text) {
  printf("Missing: %s\n", ts_node_type(node));
}

void print_id (TSNode node) {
  printf("ID: %u\n", (unsigned int) (uint16_t *) node.id);
}

void visitPreOrder (TSNode node, void (*fn)(), void * context) {
  if (ts_node_is_null(node)) {
    return;
  }

  fn(node, context);

  unsigned child_count = ts_node_child_count(node);
  for (unsigned i = 0; i < child_count; i++) {
    TSNode child_node = ts_node_child(node, i);
    visitPreOrder(child_node, fn, context);
  }
}

void * visit_node (TSNode node, GenContext * context) {
  printf("VISIT: %s\n", context->code);
  const char * type = ts_node_type(node);

  if (type == "const") node_spaced_keyword(node, context->code);
  else if (type == "import") node_spaced_keyword(node, context->code);
  else if (type == "export") node_spaced_keyword(node, context->code);
  else if (type == "default") node_spaced_keyword(node, context->code);
  else if (type == "new") node_spaced_keyword(node, context->code);
  else if (type == "var") node_spaced_keyword(node, context->code);
  else if (type == "let") node_spaced_keyword(node, context->code);
  else if (type == "for") node_spaced_keyword(node, context->code);
  else if (type == "do") node_spaced_keyword(node, context->code);
  else if (type == "while") node_spaced_keyword(node, context->code);
  else if (type == "function") node_spaced_keyword(node, context->code);
  else if (type == "class") node_class(node, context->code, context->counter);
  else if (type == "this") node_keyword(node, context->code);
  else if (type == ".") node_keyword(node, context->code);
  else if (type == ";") node_keyword(node, context->code);
  else if (type == "=") node_keyword(node, context->code);
  else if (type == ",") node_keyword(node, context->code);
  else if (type == "(") node_keyword(node, context->code);
  else if (type == ")") node_keyword(node, context->code);
  else if (type == "<") node_keyword(node, context->code);
  else if (type == ">") node_keyword(node, context->code);
  else if (type == "[") node_keyword(node, context->code);
  else if (type == "]") node_keyword(node, context->code);
  else if (type == "{") node_keyword(node, context->code);
  else if (type == "}") node_keyword(node, context->code);
  else if (type == "||") node_keyword(node, context->code);
  else if (type == "&&") node_keyword(node, context->code);
  else if (type == "number") node_text(node, context->code, context->source);
  else if (type == "identifier") node_text(node, context->code, context->source);
  else if (type == "property_identifier") node_text(node, context->code, context->source);
  else node_noop(node, context->code);
}

int main() {
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_javascript());

  FILE *source_file;
  source_file = fopen("source.js","rb");
  fseek(source_file, 0, SEEK_END);
  long fsize = ftell(source_file);
  fseek(source_file, 0, SEEK_SET);

  char *source_code = malloc(fsize + 1);
  fread(source_code, 1, fsize, source_file);

  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  TSNode root_node = ts_tree_root_node(tree);

  int counter = 0;
  size_t size = 1;
  char *code;
  code = malloc(size * sizeof(char));
  *code = '\0';

  GenContext context = {code, source_code, &counter};
  visitPreOrder(root_node, visit_node, &context);
  printf("\nSrc code: \n\n%s\n", source_code);
  printf("\nGen code: \n\n%s\n", code);

  free(code);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  return 0;
}

