#pragma once
#include "../parser.h"

typedef struct Node Node;
typedef struct Parser Parser;
typedef struct StringViewArr StringViewArr;

Node* newLiteralNode(Number* num);
Node* newBooleanNode(Number* num);
Node* newAssignmentNode(Token name, Node* value);
Node* newVarNode(Token name);
Node* newUnaryNode(Token op, Node* operand);
Node* newPrefixNode(Token op, Node* operand);
Node* newBinaryNode(Token op, Node* left, Node* right);
Node* newFuncCallNode(Token name, int argCount, Node* arg1, ...);
Node* newFuncDefNode(Token name, StringViewArr* params, Node* val);
Number* evaluateString(Lexer* lexer, Parser* parser, char* str);
Node* newLiteralNodeWithVal(double val);
void nextToken(Parser* parser);
void freeNode(Node* node);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
char* lookupTokenKind(TokenKind kind);
char* lookupNodeKind(NodeKind kind);
bool canBeNodeLiteral(Node* node);
Node* copyNode(Node* node);