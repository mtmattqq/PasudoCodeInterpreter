/// --------------------
/// Node
/// --------------------

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <memory>
#include "token.h"

const std::string NODE_VALUE{"VALUE"};
const std::string NODE_BINOP{"BINOP"};
const std::string NODE_ERROR{"ERROR"};
const std::string NODE_UNARYOP("UNARYOP");
const std::string NODE_VARASSIGN("VARASSIGN");
const std::string NODE_VARACCESS("VARACCESS");
const std::string NODE_IF("IF");
const std::string NODE_FOR("FOR");
const std::string NODE_WHILE("WHILE");
const std::string NODE_REPEAT("REPEAT");
const std::string NODE_ALGODEF("ALGO");
const std::string NODE_ALGOCALL("CALL");
const std::string NODE_ARRAY("ARRAY");
const std::string NODE_ARRACCESS("ARRACCESS");
const std::string NODE_ARRASSIGN("ARRASSIGN");
const std::string TAB{"    "};

class Node {
public:
    virtual std::string get_node() = 0;
    virtual ~Node() {};
    virtual std::vector<std::shared_ptr<Node>> get_child() { return std::vector<std::shared_ptr<Node>>(0);}
    virtual std::string get_type() {return "NONE";}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual TokenList get_toks() { return TokenList(0);}
    virtual std::string get_name() {return "";}
};

using NodeList = std::vector<std::shared_ptr<Node>>;

class ErrorNode: public Node {
public:
    ErrorNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~ErrorNode() {}
    virtual std::string get_node();
    virtual std::shared_ptr<Token> get_tok() { return tok;}
    virtual std::string get_type() {return NODE_ERROR;}
protected:
    std::shared_ptr<Token> tok;
};

class ValueNode: public Node {
public:
    ValueNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~ValueNode() {}
    virtual std::string get_node();
    virtual std::string get_type() {return NODE_VALUE;}
    virtual std::shared_ptr<Token> get_tok() { return tok;}
protected:
    std::shared_ptr<Token> tok;
};

class BinOpNode: public Node {
public:
    BinOpNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right, std::shared_ptr<Token> tok)
        : left_node(left), right_node(right), op_tok(tok) {}
    // virtual ~BinOpNode() {}
    virtual std::string get_node();
    virtual NodeList get_child();
    virtual std::string get_type() {return NODE_BINOP;}
    virtual std::shared_ptr<Token> get_tok() { return op_tok;}
protected:
    std::shared_ptr<Node> left_node, right_node;
    std::shared_ptr<Token> op_tok;
};

class UnaryOpNode: public Node {
public:
    UnaryOpNode(std::shared_ptr<Node> _node, std::shared_ptr<Token> tok)
        : node(_node), op_tok(tok) {}
    // virtual ~UnaryOpNode() {}
    virtual std::string get_node();
    virtual NodeList get_child();
    virtual std::string get_type() { return NODE_UNARYOP;}
    virtual std::shared_ptr<Token> get_tok() { return op_tok;}
protected:
    std::shared_ptr<Node> node;
    std::shared_ptr<Token> op_tok;
};

class VarAssignNode: public Node {
public:
    VarAssignNode(std::string _name, std::shared_ptr<Node> _node)
        : name(_name), node(_node) {}
    // virtual ~VarAssignNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{node};}
    virtual std::string get_type() { return NODE_VARASSIGN;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() {return name;}
protected:
    std::string name;
    std::shared_ptr<Node> node;
};

class VarAccessNode: public Node {
public:
    VarAccessNode(std::shared_ptr<Token> _tok)
        : tok(_tok) {}
    // virtual ~VarAccessNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList(0);}
    virtual std::string get_type() { return NODE_VARACCESS;}
    virtual std::shared_ptr<Token> get_tok() { return tok;}
    virtual std::string get_name() {return tok->get_value();}
protected:
    std::shared_ptr<Token> tok;
};

class IfNode: public Node {
public:
    IfNode(std::shared_ptr<Node> condition, NodeList expr, std::shared_ptr<Node> _else_node)
        : condition_node(condition), expr_node(expr), else_node(_else_node) {}
    // virtual ~IfNode() {}
    virtual std::string get_node();
    virtual NodeList get_child() {
        NodeList child{condition_node, else_node};
        for(auto node : expr_node) child.push_back(node);
        return child;
    }
    virtual std::string get_type() { return NODE_IF;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition_node, else_node; 
    NodeList expr_node;
};

class ForNode: public Node {
public:
    ForNode(
        std::shared_ptr<Node> _var_assign, std::shared_ptr<Node> _end_value,
        std::shared_ptr<Node> _step_value, NodeList _body_node
    )   : var_assign(_var_assign), end_value(_end_value), step_value(_step_value), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() {
        NodeList child{var_assign, end_value, step_value};
        for(auto node : body_node) child.push_back(node);
        return child;
    }
    virtual std::string get_type() { return NODE_FOR;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> var_assign, end_value, step_value;
    NodeList body_node;
};

class WhileNode: public Node {
public:
    WhileNode(std::shared_ptr<Node> _condition, NodeList _body_node)
        : condition(_condition), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() {
        NodeList child{condition};
        for(auto node : body_node) child.push_back(node);
        return child;
    }
    virtual std::string get_type() { return NODE_WHILE;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition;
    NodeList body_node;
};

class RepeatNode: public Node {
public:
    RepeatNode(NodeList _body_node, std::shared_ptr<Node> _condition)
        : condition(_condition), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() {
        NodeList child{condition};
        for(auto node : body_node) child.push_back(node);
        return child;
    }
    virtual std::string get_type() { return NODE_REPEAT;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    std::shared_ptr<Node> condition;
    NodeList body_node;
};

class AlgorithmDefNode: public Node {
public:
    AlgorithmDefNode(std::shared_ptr<Token> _algo_name, const TokenList &_args_name, NodeList _body_node = {})
        : algo_name(_algo_name), args_name(_args_name), body_node(_body_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return body_node;}
    virtual std::string get_type() { return NODE_ALGODEF;}
    virtual std::shared_ptr<Token> get_tok() { return algo_name;}
    virtual TokenList get_toks() { return args_name;}
    virtual std::string get_name() { return algo_name->get_value();}
protected:
    std::shared_ptr<Token> algo_name;
    TokenList args_name;
    NodeList body_node;
};

class AlgorithmCallNode: public Node {
public:
    AlgorithmCallNode(std::shared_ptr<Node> _call_node, const NodeList &_args)
        : call_node(_call_node), args(_args) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return args;}
    virtual std::string get_type() { return NODE_ALGOCALL;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return call_node->get_name();}
    std::shared_ptr<Node> get_call() { return call_node;}
protected:
    std::shared_ptr<Node> call_node;
    NodeList args;
};

class ArrayNode: public Node {
public:
    ArrayNode(const NodeList &_elements_node)
        : elements_node(_elements_node) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return elements_node;}
    virtual std::string get_type() { return NODE_ARRAY;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
    virtual std::string get_name() { return "";}
protected:
    NodeList elements_node;
};

class ArrayAccessNode: public Node {
public:
    ArrayAccessNode(std::shared_ptr<Node> _arr, std::shared_ptr<Node> _index)
        : arr(_arr), index(_index) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{arr, index};}
    virtual std::string get_type() { return NODE_ARRACCESS;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
protected:
    std::shared_ptr<Node> arr, index;
};

class ArrayAssignNode: public Node {
public:
    ArrayAssignNode(std::shared_ptr<Node> _arr, std::shared_ptr<Node> _value)
        : arr(_arr), value(_value) {}
    virtual std::string get_node();
    virtual NodeList get_child() { return NodeList{arr, value};}
    virtual std::string get_type() { return NODE_ARRASSIGN;}
    virtual std::shared_ptr<Token> get_tok() { return nullptr;}
protected:
    std::shared_ptr<Node> arr, value;
};

#endif