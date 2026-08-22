#pragma once
#include "../parser.h"

typedef struct Node Node;
typedef struct StringViewArr StringViewArr;

Node *newLiteralNode(Number *num);
Node *newUnitLiteralNode(Number *num, const Unit *unit);
Node *newBooleanNode(Number *num);
Node *newAssignmentNode(Token name, Node *value);
Node *newVarNode(Token name);
Node *newUnaryNode(Token op, Node *operand);
Node *newPrefixNode(Token op, Node *operand);
Node *newBinaryNode(Token op, Node *left, Node *right);
Node *newFuncCallNode(Token name, Node **args, int argCount);
Node *newFuncDefNode(Token name, StringViewArr *params, Node *val);
Node *newSetNode(Node **elements, int count);
Number *evaluateString(Lexer *lexer, Parser *parser, char *str,
                       const Unit **outUnit);
Node *newLiteralNodeWithVal(double val);
void nextToken(Parser *parser);
void freeNode(Node *node);
bool isImplicitMult(TokenKind left, TokenKind right);
Node *newAddNode(Node *left, Node *right);
Node *newSubNode(Node *left, Node *right);
Node *newMulNode(Node *left, Node *right);
Node *newDivNode(Node *left, Node *right);
Node *newPowNode(Node *base, Node *exp);
bool isArithOp(TokenKind kind);
bool isComparisonOp(TokenKind kind);
char *lookupTokenKind(TokenKind kind);
char *lookupNodeKind(NodeKind kind);
bool canBeNodeLiteral(Node *node);
Node *copyNode(Node *node);
