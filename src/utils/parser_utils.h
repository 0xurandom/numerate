#pragma once
#include "../parser.h"

Node* newLiteralNode(Number* num);
Node* newBooleanNode(Number* num);
Node* newAssignmentNode(Token name, Node* value);
Node* newVarNode(Token name);
Node* newUnaryNode(Token op, Node* operand);
Node* newPrefixNode(Token op, Node* operand);
Node* newBinaryNode(Token op, Node* left, Node* right);
Number* evaluateString(Lexer* lexer, Parser* parser, char* str);
void nextToken(Parser* parser);
void freeNode(Node* node);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
char* lookupTokenKind(TokenKind kind);
char* lookupNodeKind(NodeKind kind);
bool canBeNodeLiteral(Node* node);
