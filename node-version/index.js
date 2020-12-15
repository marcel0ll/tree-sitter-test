const Parser = require("tree-sitter");
const JavaScript = require("tree-sitter-javascript");
const fs = require("fs");
const odiff = require("odiff");

function visitPreOrder(node, fn) {
  if (!node) {
    return
  }

  fn(node);
  (node.children || []).forEach((n) => visitPreOrder(n, fn));
}

// visitors
const visitors = {};
const node_keyword = (node) => node.type;
const node_spacedKeyword = (node) => node.type + " ";
const node_text = (node) => node.text;
const node_other = (node) => "";

let class_counter = 0;
visitors["export"] = node_spacedKeyword;
visitors["default"] = node_spacedKeyword;
visitors["this"] = node_keyword;
visitors["class"] = () => class_counter++ %2 === 0 ? "class ": "";
visitors["const"] = node_spacedKeyword;
visitors["new"] = node_spacedKeyword;
visitors["var"] = node_spacedKeyword;
visitors["let"] = node_spacedKeyword;
// visitors["let"] = () => "def ";
visitors["for"] = node_spacedKeyword;
visitors["while"] = node_spacedKeyword;
visitors["do"] = node_spacedKeyword;
visitors["."] = node_keyword;
visitors["="] = node_keyword;
visitors[";"] = node_keyword;
visitors[","] = node_keyword;
visitors["number"] = node_text;
visitors["function"] = node_spacedKeyword;
visitors["identifier"] = node_text;
visitors["property_identifier"] = node_text;
visitors["<"] = node_keyword;
visitors[">"] = node_keyword;
visitors["["] = node_keyword;
visitors["]"] = node_keyword;
visitors["("] = node_keyword;
visitors[")"] = node_keyword;
visitors["{"] = node_keyword;
visitors["}"] = node_keyword;
visitors["||"] = node_keyword;
visitors["&&"] = node_keyword;
visitors["__other__"] = node_other;

// parser

const parser = new Parser();
parser.setLanguage(JavaScript);

const source = fs.readFileSync("source.js", "utf8");

const sourceTree = parser.parse(source);
console.log("source:", sourceTree.rootNode.toString());

// generator

let text = ``;
function visitChildNode(node, count=0) {
  if (!node) {
    console.warn("weird shit, no child");
    return;
  }
  const genNode = {...node};

  count += 1;
  if (node.children && node.children.length) {
    genNode.children = node.children.map((n) => {
      let genChild = visitChildNode(n, count)

      count = genChild.count;

      return genChild;
    });
  }

  let visitor = visitors[node.type];
  if (visitor) {
    console.log("Visiting: ", node.type);
    genNode.gen = visitor(node);
  } else {
    console.log("Missing: ", node.type);
    genNode.gen = "";
  }
  genNode.count = count;
  genNode.node = node;

  return genNode;
}

function printTree(tree) {
  let text = "";
  visitPreOrder(tree, (n) => {
    console.log(n);
    text+= n.gen
  })
  console.log(text);
}

let generatedTree = visitChildNode(sourceTree.rootNode);
console.log(printTree(generatedTree), generatedTree.count);

const newSource = fs.readFileSync("new_source.js", "utf8");

sourceTree.edit({
  startIndex: 0,
  oldEndIndex: 3,
  newEndIndex: 5,
  startPosition: {row: 0, column: 0},
  oldEndPosition: {row: 0, column: 3},
  newEndPosition: {row: 0, column: 5},
});

const newTree = parser.parse(newSource, sourceTree);

const sourceWalker = sourceTree.walk();
const newTreeWalker = newTree.walk();
