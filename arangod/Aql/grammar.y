%define api.pure
%name-prefix "Aql"
%locations 
%defines
%parse-param { triagens::aql::Parser* parser }
%lex-param { void* scanner } 
%error-verbose

%{
#include "Aql/AstNode.h"
#include "Aql/Function.h"
#include "Aql/Parser.h"
#include "Basics/conversions.h"
#include "Basics/tri-strings.h"
%}

%union {
  triagens::aql::AstNode*  node;
  char*                    strval;
  bool                     boolval;
  int64_t                  intval;
}

%{

using namespace triagens::aql;

////////////////////////////////////////////////////////////////////////////////
/// @brief forward for lexer function defined in Aql/tokens.ll
////////////////////////////////////////////////////////////////////////////////

int Aqllex (YYSTYPE*, 
            YYLTYPE*, 
            void*);
 
////////////////////////////////////////////////////////////////////////////////
/// @brief register parse error
////////////////////////////////////////////////////////////////////////////////

void Aqlerror (YYLTYPE* locp, 
               triagens::aql::Parser* parser,
               char const* message) {
  parser->registerParseError(TRI_ERROR_QUERY_PARSE, message, locp->first_line, locp->first_column);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief shortcut macro for signalling out of memory
////////////////////////////////////////////////////////////////////////////////

#define ABORT_OOM                                   \
  parser->registerError(TRI_ERROR_OUT_OF_MEMORY);   \
  YYABORT;

#define scanner parser->scanner()

%}

/* define tokens and "nice" token names */
%token T_FOR "FOR declaration"
%token T_LET "LET declaration" 
%token T_FILTER "FILTER declaration" 
%token T_RETURN "RETURN declaration" 
%token T_COLLECT "COLLECT declaration" 
%token T_SORT "SORT declaration" 
%token T_LIMIT "LIMIT declaration"

%token T_ASC "ASC keyword"
%token T_DESC "DESC keyword"
%token T_IN "IN keyword"
%token T_WITH "WITH keyword"
%token T_INTO "INTO keyword"

%token T_GRAPH "GRAPH keyword"
%token T_DISTINCT "DISTINCT modifier"

%token T_REMOVE "REMOVE command"
%token T_INSERT "INSERT command"
%token T_UPDATE "UPDATE command"
%token T_REPLACE "REPLACE command"
%token T_UPSERT "UPSERT command"

%token T_NULL "null" 
%token T_TRUE "true" 
%token T_FALSE "false"
%token T_STRING "identifier" 
%token T_QUOTED_STRING "quoted string" 
%token T_INTEGER "integer number" 
%token T_DOUBLE "number" 
%token T_PARAMETER "bind parameter"

%token T_ASSIGN "assignment"

%token T_NOT "not operator"
%token T_AND "and operator"
%token T_OR "or operator"

%token T_EQ "== operator"
%token T_NE "!= operator"
%token T_LT "< operator"
%token T_GT "> operator"
%token T_LE "<= operator"
%token T_GE ">= operator"

%token T_PLUS "+ operator"
%token T_MINUS "- operator"
%token T_TIMES "* operator"
%token T_DIV "/ operator"
%token T_MOD "% operator"

%token T_QUESTION "?"
%token T_COLON ":"
%token T_SCOPE "::"
%token T_RANGE ".."

%token T_COMMA ","
%token T_OPEN "("
%token T_CLOSE ")"
%token T_OBJECT_OPEN "{"
%token T_OBJECT_CLOSE "}"
%token T_ARRAY_OPEN "["
%token T_ARRAY_CLOSE "]"

%token T_END 0 "end of query string"

%token T_OUTBOUND "outbound direction"
%token T_INBOUND "inbound direction"
%token T_ANY "any direction"

/* define operator precedence */
%left T_COMMA
%left T_DISTINCT
%right T_QUESTION T_COLON
%right T_ASSIGN
%left T_WITH
%nonassoc T_INTO
%left T_OR 
%left T_AND
%left T_EQ T_NE 
%left T_IN T_NIN 
%left T_LT T_GT T_LE T_GE
%left T_RANGE
%left T_PLUS T_MINUS
%left T_TIMES T_DIV T_MOD
%right UMINUS UPLUS T_NOT
%left FUNCCALL
%left REFERENCE
%left INDEXED
%left EXPANSION
%left T_SCOPE

/* define token return types */
%type <strval> T_STRING
%type <strval> T_QUOTED_STRING
%type <node> T_INTEGER
%type <node> T_DOUBLE
%type <strval> T_PARAMETER; 
%type <node> sort_list;
%type <node> sort_element;
%type <node> sort_direction;
%type <node> collect_list;
%type <node> collect_element;
%type <node> collect_variable_list;
%type <node> keep;
%type <strval> optional_into;
%type <strval> count_into;
%type <node> expression;
%type <node> expression_or_query;
%type <node> distinct_expression;
%type <node> operator_unary;
%type <node> operator_binary;
%type <node> operator_ternary;
%type <node> function_call;
%type <strval> function_name;
%type <node> optional_function_call_arguments;
%type <node> function_arguments_list;
%type <node> compound_value;
%type <node> array;
%type <node> optional_array_elements;
%type <node> array_elements_list;
%type <node> object;
%type <node> options;
%type <node> optional_object_elements;
%type <node> object_elements_list;
%type <node> object_element;
%type <strval> object_element_name;
%type <intval> array_filter_operator;
%type <node> optional_array_filter;
%type <node> optional_array_limit;
%type <node> optional_array_return;
%type <node> graph_subject;
%type <intval> graph_direction;
%type <node> graph_direction_steps;
%type <node> graph_collection;
%type <node> reference;
%type <node> simple_value;
%type <node> value_literal;
%type <node> collection_name;
%type <node> in_or_into_collection;
%type <node> bind_parameter;
%type <strval> variable_name;
%type <node> numeric_value;
%type <intval> update_or_replace;


/* define start token of language */
%start query

%%

query: 
    optional_statement_block_statements return_statement {
    }
  | optional_statement_block_statements remove_statement optional_post_modification_block {
    }
  | optional_statement_block_statements insert_statement optional_post_modification_block {
    }
  | optional_statement_block_statements update_statement optional_post_modification_block {
    }
  | optional_statement_block_statements replace_statement optional_post_modification_block {
    }
  | optional_statement_block_statements upsert_statement optional_post_modification_block {
    }
  ;

optional_post_modification_lets:
    /* empty */ {
    }
  | optional_post_modification_lets let_statement {
    }
  ;

optional_post_modification_block:
    /* empty */ {
      // still need to close the scope opened by the data-modification statement
      parser->ast()->scopes()->endNested();
    }
  | optional_post_modification_lets return_statement {
      // the RETURN statement will close the scope opened by the data-modification statement
    }
  ;

optional_statement_block_statements:
    /* empty */ {
    }
  | optional_statement_block_statements statement_block_statement {
    }
  ;

statement_block_statement: 
    for_statement {
    }
  | let_statement {
    }
  | filter_statement {
    }
  | collect_statement {
    }
  | sort_statement {
    }
  | limit_statement {
    }
  ;

for_statement:
    T_FOR variable_name T_IN expression {
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_FOR);
     
      auto node = parser->ast()->createNodeFor($2, $4);
      parser->ast()->addOperation(node);
    }
    | T_FOR variable_name T_IN graph_direction_steps expression graph_subject {
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_FOR);
      auto node = parser->ast()->createNodeTraversal($2, $4, $5, $6);
      parser->ast()->addOperation(node);
    }
    | T_FOR variable_name T_COMMA variable_name T_IN graph_direction_steps expression graph_subject {
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_FOR);
      auto node = parser->ast()->createNodeTraversal($2, $4, $6, $7, $8);
      parser->ast()->addOperation(node);
    }
    | T_FOR variable_name T_COMMA variable_name T_COMMA variable_name T_IN graph_direction_steps expression graph_subject {
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_FOR);
      auto node = parser->ast()->createNodeTraversal($2, $4, $6, $8, $9, $10);
      parser->ast()->addOperation(node);
    }
  ;

filter_statement:
    T_FILTER expression {
      // operand is a reference. can use it directly
      auto node = parser->ast()->createNodeFilter($2);
      parser->ast()->addOperation(node);
    }
  ;

let_statement:
    T_LET let_list {
    }
  ;

let_list: 
    let_element {
    }
  | let_list T_COMMA let_element {
    }
  ;

let_element:
    variable_name T_ASSIGN expression {
      auto node = parser->ast()->createNodeLet($1, $3, true);
      parser->ast()->addOperation(node);
    }
  ;

count_into: 
    T_WITH T_STRING T_INTO variable_name {
      if (! TRI_CaseEqualString($2, "COUNT")) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "unexpected qualifier '%s', expecting 'COUNT'", $2, yylloc.first_line, yylloc.first_column);
      }

      $$ = $4;
    }
  ;

collect_variable_list:
    T_COLLECT {
      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
    } collect_list { 
      auto list = static_cast<AstNode*>(parser->popStack());

      if (list == nullptr) {
        ABORT_OOM
      }
      $$ = list;
    }
  ;

collect_statement: 
    T_COLLECT count_into options {
      auto scopes = parser->ast()->scopes();

      // check if we are in the main scope
      bool reRegisterVariables = (scopes->type() != triagens::aql::AQL_SCOPE_MAIN); 

      if (reRegisterVariables) {
        // end the active scopes
        scopes->endNested();
        // start a new scope
        scopes->start(triagens::aql::AQL_SCOPE_COLLECT);
      }

      auto node = parser->ast()->createNodeCollectCount(parser->ast()->createNodeArray(), $2, $3);
      parser->ast()->addOperation(node);
    }
  | collect_variable_list count_into options {
      auto scopes = parser->ast()->scopes();

      // check if we are in the main scope
      bool reRegisterVariables = (scopes->type() != triagens::aql::AQL_SCOPE_MAIN); 

      if (reRegisterVariables) {
        // end the active scopes
        scopes->endNested();
        // start a new scope
        scopes->start(triagens::aql::AQL_SCOPE_COLLECT);

        size_t const n = $1->numMembers();
        for (size_t i = 0; i < n; ++i) {
          auto member = $1->getMember(i);

          if (member != nullptr) {
            TRI_ASSERT(member->type == NODE_TYPE_ASSIGN);
            auto v = static_cast<Variable*>(member->getMember(0)->getData());
            scopes->addVariable(v);
          }
        }
      }

      auto node = parser->ast()->createNodeCollectCount($1, $2, $3);
      parser->ast()->addOperation(node);
    }
  | collect_variable_list optional_into options {
      auto scopes = parser->ast()->scopes();

      // check if we are in the main scope
      bool reRegisterVariables = (scopes->type() != triagens::aql::AQL_SCOPE_MAIN); 

      if (reRegisterVariables) {
        // end the active scopes
        scopes->endNested();
        // start a new scope
        scopes->start(triagens::aql::AQL_SCOPE_COLLECT);

        size_t const n = $1->numMembers();
        for (size_t i = 0; i < n; ++i) {
          auto member = $1->getMember(i);

          if (member != nullptr) {
            TRI_ASSERT(member->type == NODE_TYPE_ASSIGN);
            auto v = static_cast<Variable*>(member->getMember(0)->getData());
            scopes->addVariable(v);
          }
        }
      }

      auto node = parser->ast()->createNodeCollect($1, $2, nullptr, $3);
      parser->ast()->addOperation(node);
    }
  | collect_variable_list optional_into keep options {
      auto scopes = parser->ast()->scopes();

      // check if we are in the main scope
      bool reRegisterVariables = (scopes->type() != triagens::aql::AQL_SCOPE_MAIN); 

      if (reRegisterVariables) {
        // end the active scopes
        scopes->endNested();
        // start a new scope
        scopes->start(triagens::aql::AQL_SCOPE_COLLECT);

        size_t const n = $1->numMembers();
        for (size_t i = 0; i < n; ++i) {
          auto member = $1->getMember(i);

          if (member != nullptr) {
            TRI_ASSERT(member->type == NODE_TYPE_ASSIGN);
            auto v = static_cast<Variable*>(member->getMember(0)->getData());
            scopes->addVariable(v);
          }
        }
      }

      if ($2 == nullptr && 
          $3 != nullptr) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "use of 'KEEP' without 'INTO'", yylloc.first_line, yylloc.first_column);
      } 

      auto node = parser->ast()->createNodeCollect($1, $2, $3, $4);
      parser->ast()->addOperation(node);
    }
  | collect_variable_list T_INTO variable_name T_ASSIGN expression options {
      auto scopes = parser->ast()->scopes();

      // check if we are in the main scope
      bool reRegisterVariables = (scopes->type() != triagens::aql::AQL_SCOPE_MAIN); 

      if (reRegisterVariables) {
        // end the active scopes
        scopes->endNested();
        // start a new scope
        scopes->start(triagens::aql::AQL_SCOPE_COLLECT);

        size_t const n = $1->numMembers();
        for (size_t i = 0; i < n; ++i) {
          auto member = $1->getMember(i);

          if (member != nullptr) {
            TRI_ASSERT(member->type == NODE_TYPE_ASSIGN);
            auto v = static_cast<Variable*>(member->getMember(0)->getData());
            scopes->addVariable(v);
          }
        }
      }

      auto node = parser->ast()->createNodeCollectExpression($1, $3, $5, $6);
      parser->ast()->addOperation(node);
    }
  ;

collect_list: 
    collect_element {
    }
  | collect_list T_COMMA collect_element {
    }
  ;

collect_element:
    variable_name T_ASSIGN expression {
      auto node = parser->ast()->createNodeAssign($1, $3);
      parser->pushArrayElement(node);
    }
  ;

optional_into: 
    /* empty */ {
      $$ = nullptr;
    }
  | T_INTO variable_name {
      $$ = $2;
    }
  ;

variable_list: 
    variable_name {
      if (! parser->ast()->scopes()->existsVariable($1)) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "use of unknown variable '%s' for KEEP", $1, yylloc.first_line, yylloc.first_column);
      }
        
      auto node = parser->ast()->createNodeReference($1);
      if (node == nullptr) {
        ABORT_OOM
      }

      // indicate the this node is a reference to the variable name, not the variable value
      node->setFlag(FLAG_KEEP_VARIABLENAME);
      parser->pushArrayElement(node);
    }
  | variable_list T_COMMA variable_name {
      if (! parser->ast()->scopes()->existsVariable($3)) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "use of unknown variable '%s' for KEEP", $3, yylloc.first_line, yylloc.first_column);
      }
        
      auto node = parser->ast()->createNodeReference($3);
      if (node == nullptr) {
        ABORT_OOM
      }

      // indicate the this node is a reference to the variable name, not the variable value
      node->setFlag(FLAG_KEEP_VARIABLENAME);
      parser->pushArrayElement(node);
    }
  ;

keep: 
    T_STRING {
      if (! TRI_CaseEqualString($1, "KEEP")) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "unexpected qualifier '%s', expecting 'KEEP'", $1, yylloc.first_line, yylloc.first_column);
      }

      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
    } variable_list {
      auto list = static_cast<AstNode*>(parser->popStack());
      $$ = list;
    }
  ;

sort_statement:
    T_SORT {
      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
    } sort_list {
      auto list = static_cast<AstNode const*>(parser->popStack());
      auto node = parser->ast()->createNodeSort(list);
      parser->ast()->addOperation(node);
    }
  ;

sort_list: 
    sort_element {
      parser->pushArrayElement($1);
    }
  | sort_list T_COMMA sort_element {
      parser->pushArrayElement($3);
    }
  ;

sort_element:
    expression sort_direction {
      $$ = parser->ast()->createNodeSortElement($1, $2);
    }
  ;

sort_direction:
    /* empty */ {
      $$ = parser->ast()->createNodeValueBool(true);
    }
  | T_ASC {
      $$ = parser->ast()->createNodeValueBool(true);
    } 
  | T_DESC {
      $$ = parser->ast()->createNodeValueBool(false);
    }
  | simple_value {
      $$ = $1;
    }
  ;

limit_statement: 
    T_LIMIT simple_value {
      auto offset = parser->ast()->createNodeValueInt(0);
      auto node = parser->ast()->createNodeLimit(offset, $2);
      parser->ast()->addOperation(node);
    }
  | T_LIMIT simple_value T_COMMA simple_value {
      auto node = parser->ast()->createNodeLimit($2, $4);
      parser->ast()->addOperation(node);
    }
  ;

return_statement:
    T_RETURN distinct_expression {
      auto node = parser->ast()->createNodeReturn($2);
      parser->ast()->addOperation(node);
      parser->ast()->scopes()->endNested();
    }
  ;

in_or_into_collection:
    T_IN collection_name {
      $$ = $2;
    }
  | T_INTO collection_name {
       $$ = $2;
     }
   ;

remove_statement:
    T_REMOVE expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_REMOVE, $3, $4)) {
        YYABORT;
      }
      auto node = parser->ast()->createNodeRemove($2, $3, $4);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  ;

insert_statement:
    T_INSERT expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_INSERT, $3, $4)) {
        YYABORT;
      }
      auto node = parser->ast()->createNodeInsert($2, $3, $4);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  ;

update_parameters:
    expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_UPDATE, $2, $3)) {
        YYABORT;
      }

      AstNode* node = parser->ast()->createNodeUpdate(nullptr, $1, $2, $3);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  | expression T_WITH expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_UPDATE, $4, $5)) {
        YYABORT;
      }

      AstNode* node = parser->ast()->createNodeUpdate($1, $3, $4, $5);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  ;

update_statement:
    T_UPDATE update_parameters {
    }
  ;

replace_parameters:
    expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_REPLACE, $2, $3)) {
        YYABORT;
      }

      AstNode* node = parser->ast()->createNodeReplace(nullptr, $1, $2, $3);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  | expression T_WITH expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_REPLACE, $4, $5)) {
        YYABORT;
      }

      AstNode* node = parser->ast()->createNodeReplace($1, $3, $4, $5);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  ;

replace_statement:
    T_REPLACE replace_parameters {
    }
  ;

update_or_replace:
    T_UPDATE {
      $$ = static_cast<int64_t>(NODE_TYPE_UPDATE);
    }
  | T_REPLACE {
      $$ = static_cast<int64_t>(NODE_TYPE_REPLACE);
    }
  ;

upsert_statement:
    T_UPSERT { 
      // reserve a variable named "$OLD", we might need it in the update expression
      // and in a later return thing
      parser->pushStack(parser->ast()->createNodeVariable(Variable::NAME_OLD, true));
    } expression T_INSERT expression update_or_replace expression in_or_into_collection options {
      if (! parser->configureWriteQuery(AQL_QUERY_UPSERT, $8, $9)) {
        YYABORT;
      }

      AstNode* variableNode = static_cast<AstNode*>(parser->popStack());
      
      auto scopes = parser->ast()->scopes();
      
      scopes->start(triagens::aql::AQL_SCOPE_SUBQUERY);
      parser->ast()->startSubQuery();
      
      scopes->start(triagens::aql::AQL_SCOPE_FOR);
      std::string const variableName = std::move(parser->ast()->variables()->nextName());
      auto forNode = parser->ast()->createNodeFor(variableName.c_str(), $8, false);
      parser->ast()->addOperation(forNode);

      auto filterNode = parser->ast()->createNodeUpsertFilter(parser->ast()->createNodeReference(variableName.c_str()), $3);
      parser->ast()->addOperation(filterNode);
      
      auto offsetValue = parser->ast()->createNodeValueInt(0);
      auto limitValue = parser->ast()->createNodeValueInt(1);
      auto limitNode = parser->ast()->createNodeLimit(offsetValue, limitValue);
      parser->ast()->addOperation(limitNode);
      
      auto refNode = parser->ast()->createNodeReference(variableName.c_str());
      auto returnNode = parser->ast()->createNodeReturn(refNode);
      parser->ast()->addOperation(returnNode);
      scopes->endNested();

      AstNode* subqueryNode = parser->ast()->endSubQuery();
      scopes->endCurrent();
      
      std::string const subqueryName = std::move(parser->ast()->variables()->nextName());
      auto subQuery = parser->ast()->createNodeLet(subqueryName.c_str(), subqueryNode, false);
      parser->ast()->addOperation(subQuery);
      
      auto index = parser->ast()->createNodeValueInt(0);
      auto firstDoc = parser->ast()->createNodeLet(variableNode, parser->ast()->createNodeIndexedAccess(parser->ast()->createNodeReference(subqueryName.c_str()), index));
      parser->ast()->addOperation(firstDoc);

      auto node = parser->ast()->createNodeUpsert(static_cast<AstNodeType>($6), parser->ast()->createNodeReference(Variable::NAME_OLD), $5, $7, $8, $9);
      parser->ast()->addOperation(node);
      parser->setWriteNode(node);
    }
  ;

distinct_expression:
    T_DISTINCT {
      auto const scopeType = parser->ast()->scopes()->type();

      if (scopeType == AQL_SCOPE_MAIN ||
          scopeType == AQL_SCOPE_SUBQUERY) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "cannot use DISTINCT modifier on top-level query element", yylloc.first_line, yylloc.first_column);
      }
    } expression {
      $$ = parser->ast()->createNodeDistinct($3);
    }
  | expression {
      $$ = $1;
    }
  ;

expression:
    operator_unary {
      $$ = $1;
    }
  | operator_binary {
      $$ = $1;
    }
  | operator_ternary {
      $$ = $1;
    }
  | value_literal {
      $$ = $1;
    }
  | reference {
      $$ = $1;
    }
  | expression T_RANGE expression {
      $$ = parser->ast()->createNodeRange($1, $3);
    }
  ;

function_name:
    T_STRING {
      $$ = $1;

      if ($$ == nullptr) {
        ABORT_OOM
      }
    }
  | function_name T_SCOPE T_STRING {
      if ($1 == nullptr || $3 == nullptr) {
        ABORT_OOM
      }

      std::string temp($1);
      temp.append("::");
      temp.append($3);
      $$ = parser->query()->registerString(temp.c_str(), temp.size(), false);

      if ($$ == nullptr) {
        ABORT_OOM
      }
    }
  ;

function_call:
    function_name {
      parser->pushStack($1);

      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
    } T_OPEN optional_function_call_arguments T_CLOSE %prec FUNCCALL {
      auto list = static_cast<AstNode const*>(parser->popStack());
      $$ = parser->ast()->createNodeFunctionCall(static_cast<char const*>(parser->popStack()), list);
    }
  ;

operator_unary:
    T_PLUS expression %prec UPLUS {
      $$ = parser->ast()->createNodeUnaryOperator(NODE_TYPE_OPERATOR_UNARY_PLUS, $2);
    }
  | T_MINUS expression %prec UMINUS {
      $$ = parser->ast()->createNodeUnaryOperator(NODE_TYPE_OPERATOR_UNARY_MINUS, $2);
    }
  | T_NOT expression %prec T_NOT { 
      $$ = parser->ast()->createNodeUnaryOperator(NODE_TYPE_OPERATOR_UNARY_NOT, $2);
    }
  ;

operator_binary:
    expression T_OR expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_OR, $1, $3);
    }
  | expression T_AND expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_AND, $1, $3);
    }
  | expression T_PLUS expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_PLUS, $1, $3);
    }
  | expression T_MINUS expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_MINUS, $1, $3);
    }
  | expression T_TIMES expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_TIMES, $1, $3);
    }
  | expression T_DIV expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_DIV, $1, $3);
    }
  | expression T_MOD expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_MOD, $1, $3);
    }
  | expression T_EQ expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_EQ, $1, $3);
    }
  | expression T_NE expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_NE, $1, $3);
    }
  | expression T_LT expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_LT, $1, $3);
    }
  | expression T_GT expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_GT, $1, $3);
    } 
  | expression T_LE expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_LE, $1, $3);
    }
  | expression T_GE expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_GE, $1, $3);
    }
  | expression T_IN expression {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_IN, $1, $3);
    }
  | expression T_NOT T_IN expression %prec T_NIN {
      $$ = parser->ast()->createNodeBinaryOperator(NODE_TYPE_OPERATOR_BINARY_NIN, $1, $4);
    }
  ;

operator_ternary:
    expression T_QUESTION expression T_COLON expression {
      $$ = parser->ast()->createNodeTernaryOperator($1, $3, $5);
    }
  ;

optional_function_call_arguments: 
    /* empty */ {
    }
  | function_arguments_list {
    }
  ;

expression_or_query:
    expression {
      $$ = $1;
    }
  | {
      if (parser->isModificationQuery()) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "unexpected subquery after data-modification operation", yylloc.first_line, yylloc.first_column);
      }
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_SUBQUERY);
      parser->ast()->startSubQuery();
    } query {
      AstNode* node = parser->ast()->endSubQuery();
      parser->ast()->scopes()->endCurrent();

      std::string const variableName = std::move(parser->ast()->variables()->nextName());
      auto subQuery = parser->ast()->createNodeLet(variableName.c_str(), node, false);
      parser->ast()->addOperation(subQuery);

      $$ = parser->ast()->createNodeReference(variableName.c_str());
    }
  ;

function_arguments_list:
    expression_or_query {
      parser->pushArrayElement($1);
    }
  | function_arguments_list T_COMMA expression_or_query {
      parser->pushArrayElement($3);
    }
  ;

compound_value:
    array {
      $$ = $1;
    }
  | object {
      $$ = $1;
    }
  ;

array: 
    T_ARRAY_OPEN {
      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
    } optional_array_elements T_ARRAY_CLOSE {
      $$ = static_cast<AstNode*>(parser->popStack());
    }
  ;

optional_array_elements:
    /* empty */ {
    }
  | array_elements_list {
    }
  ;

array_elements_list:
    expression {
      parser->pushArrayElement($1);
    }
  | array_elements_list T_COMMA expression {
      parser->pushArrayElement($3);
    }
  ;

options:
    /* empty */ {
      $$ = nullptr;
    }
  | T_STRING object {
      if ($1 == nullptr || $2 == nullptr) {
        ABORT_OOM
      }

      if (! TRI_CaseEqualString($1, "OPTIONS")) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "unexpected qualifier '%s', expecting 'OPTIONS'", $1, yylloc.first_line, yylloc.first_column);
      }

      $$ = $2;
    }
  ;

object:
    T_OBJECT_OPEN {
      auto node = parser->ast()->createNodeObject();
      parser->pushStack(node);
    } optional_object_elements T_OBJECT_CLOSE {
      $$ = static_cast<AstNode*>(parser->popStack());
    }
  ;

optional_object_elements:
    /* empty */ {
    }
  | object_elements_list {
    }
  ;

object_elements_list:
    object_element {
    }
  | object_elements_list T_COMMA object_element {
    }
  ;

object_element: 
    T_STRING {
      // attribute-name-only (comparable to JS enhanced object literals, e.g. { foo, bar })
      auto ast = parser->ast();
      auto variable = ast->scopes()->getVariable($1);
      
      if (variable == nullptr) {
        // variable does not exist
        parser->registerParseError(TRI_ERROR_QUERY_VARIABLE_NAME_UNKNOWN, "use of unknown variable '%s' in object literal", $1, yylloc.first_line, yylloc.first_column);
      }

      // create a reference to the variable
      auto node = ast->createNodeReference(variable);
      parser->pushObjectElement($1, node);
    }
  | object_element_name T_COLON expression {
      // attribute-name : attribute-value
      parser->pushObjectElement($1, $3);
    }
  | T_PARAMETER T_COLON expression {
      // bind-parameter : attribute-value
      if ($1 == nullptr) {
        ABORT_OOM
      }
      
      if (strlen($1) < 1 || $1[0] == '@') {
        parser->registerParseError(TRI_ERROR_QUERY_BIND_PARAMETER_TYPE, TRI_errno_string(TRI_ERROR_QUERY_BIND_PARAMETER_TYPE), $1, yylloc.first_line, yylloc.first_column);
      }

      auto param = parser->ast()->createNodeParameter($1);
      parser->pushObjectElement(param, $3);
    }
  | T_ARRAY_OPEN expression T_ARRAY_CLOSE T_COLON expression {
      // [ attribute-name-expression ] : attribute-value
      parser->pushObjectElement($2, $5);
    }
  ;

array_filter_operator:
    T_TIMES {
      $$ = 1;
    }
  | array_filter_operator T_TIMES {
      $$ = $1 + 1;
    } 
  ;

optional_array_filter:
    /* empty */ {
      $$ = nullptr;
    }
  | T_FILTER expression {
      $$ = $2;
    }
  ;

optional_array_limit:
    /* empty */ {
      $$ = nullptr;
    }
  | T_LIMIT expression {
      $$ = parser->ast()->createNodeArrayLimit(nullptr, $2);
    }
  | T_LIMIT expression T_COMMA expression {
      $$ = parser->ast()->createNodeArrayLimit($2, $4);
    }
  ;

optional_array_return:
    /* empty */ {
      $$ = nullptr;
    }
  | T_RETURN expression {
      $$ = $2;
    }
  ;

graph_collection:
    T_STRING {
      $$ = parser->ast()->createNodeValueString($1);
    }
  | bind_parameter {
      // TODO FIXME check @s
      $$ = $1;
    }
  ;

graph_collection_list:
     graph_collection {
       auto node = static_cast<AstNode*>(parser->peekStack());
       node->addMember($1);
     }
   | graph_collection_list T_COMMA graph_collection {
       auto node = static_cast<AstNode*>(parser->peekStack());
       node->addMember($3);
     }
   ;

graph_subject:
    graph_collection {
      auto node = parser->ast()->createNodeArray();
      node->addMember($1);
      $$ = parser->ast()->createNodeCollectionList(node);
    }
  | graph_collection T_COMMA 
    {
      auto node = parser->ast()->createNodeArray();
      parser->pushStack(node);
      node->addMember($1);
    } graph_collection_list {
      auto node = static_cast<AstNode*>(parser->popStack());
      $$ = parser->ast()->createNodeCollectionList(node);
    }
  | T_GRAPH bind_parameter {
      // graph name
      $$ = $2;
    }
  | T_GRAPH T_QUOTED_STRING {
      // graph name
      $$ = parser->ast()->createNodeValueString($2);
    }
  ;

graph_direction:
    // Returns the edge direction number.
    // Identical order as TRI_edge_direction_e
    T_OUTBOUND {
      $$ = 2;
    }
    | T_INBOUND {
      $$ = 1;
    }
    | T_ANY {
      $$ = 0; 
    }
  ;

graph_direction_steps:
    graph_direction {
      $$ = parser->ast()->createNodeDirection($1, 1);
    }
    | expression graph_direction {
      $$ = parser->ast()->createNodeDirection($2, $1);
    }
  ;

reference:
    T_STRING {
      // variable or collection
      auto ast = parser->ast();
      AstNode* node = nullptr;

      auto variable = ast->scopes()->getVariable($1);
      
      if (variable != nullptr) {
        // variable exists, now use it
        node = ast->createNodeReference(variable);
      }
      else {
        // variable does not exist
        // now try variable aliases OLD (= $OLD) and NEW (= $NEW)
        if (strcmp($1, "OLD") == 0) {
          variable = ast->scopes()->getVariable(Variable::NAME_OLD);
        }
        else if (strcmp($1, "NEW") == 0) {
          variable = ast->scopes()->getVariable(Variable::NAME_NEW);
        }
        else if (ast->scopes()->canUseCurrentVariable() && strcmp($1, "CURRENT") == 0) {
          variable = ast->scopes()->getCurrentVariable();
        }
        else if (strcmp($1, Variable::NAME_CURRENT) == 0) {
          variable = ast->scopes()->getCurrentVariable();
        }
        
        if (variable != nullptr) {
          // variable alias exists, now use it
          node = ast->createNodeReference(variable);
        }
      }

      if (node == nullptr) {
        // variable not found. so it must have been a collection
        node = ast->createNodeCollection($1, TRI_TRANSACTION_READ);
      }

      TRI_ASSERT(node != nullptr);

      $$ = node;
    }
  | compound_value {
      $$ = $1;
    }
  | bind_parameter {
      $$ = $1;
    }
  | function_call {
      $$ = $1;
      
      if ($$ == nullptr) {
        ABORT_OOM
      }
    }
  | T_OPEN expression T_CLOSE {
      if ($2->type == NODE_TYPE_EXPANSION) {
        // create a dummy passthru node that reduces and evaluates the expansion first
        // and the expansion on top of the stack won't be chained with any other expansions
        $$ = parser->ast()->createNodePassthru($2);
      }
      else {
        $$ = $2;
      }
    }
  | T_OPEN {
      if (parser->isModificationQuery()) {
        parser->registerParseError(TRI_ERROR_QUERY_PARSE, "unexpected subquery after data-modification operation", yylloc.first_line, yylloc.first_column);
      }
      parser->ast()->scopes()->start(triagens::aql::AQL_SCOPE_SUBQUERY);
      parser->ast()->startSubQuery();
    } query T_CLOSE {
      AstNode* node = parser->ast()->endSubQuery();
      parser->ast()->scopes()->endCurrent();

      std::string const variableName = std::move(parser->ast()->variables()->nextName());
      auto subQuery = parser->ast()->createNodeLet(variableName.c_str(), node, false);
      parser->ast()->addOperation(subQuery);

      $$ = parser->ast()->createNodeReference(variableName.c_str());
    } 
  | reference '.' T_STRING %prec REFERENCE {
      // named variable access, e.g. variable.reference
      if ($1->type == NODE_TYPE_EXPANSION) {
        // if left operand is an expansion already...
        // dive into the expansion's right-hand child nodes for further expansion and
        // patch the bottom-most one
        auto current = const_cast<AstNode*>(parser->ast()->findExpansionSubNode($1));
        TRI_ASSERT(current->type == NODE_TYPE_EXPANSION);
        current->changeMember(1, parser->ast()->createNodeAttributeAccess(current->getMember(1), $3));
        $$ = $1;
      }
      else {
        $$ = parser->ast()->createNodeAttributeAccess($1, $3);
      }
    }
  | reference '.' bind_parameter %prec REFERENCE {
      // named variable access, e.g. variable.@reference
      if ($1->type == NODE_TYPE_EXPANSION) {
        // if left operand is an expansion already...
        // patch the existing expansion
        auto current = const_cast<AstNode*>(parser->ast()->findExpansionSubNode($1));
        TRI_ASSERT(current->type == NODE_TYPE_EXPANSION);
        current->changeMember(1, parser->ast()->createNodeBoundAttributeAccess(current->getMember(1), $3));
        $$ = $1;
      }
      else {
        $$ = parser->ast()->createNodeBoundAttributeAccess($1, $3);
      }
    }
  | reference T_ARRAY_OPEN expression T_ARRAY_CLOSE %prec INDEXED {
      // indexed variable access, e.g. variable[index]
      if ($1->type == NODE_TYPE_EXPANSION) {
        // if left operand is an expansion already...
        // patch the existing expansion
        auto current = const_cast<AstNode*>(parser->ast()->findExpansionSubNode($1));
        TRI_ASSERT(current->type == NODE_TYPE_EXPANSION);
        current->changeMember(1, parser->ast()->createNodeIndexedAccess(current->getMember(1), $3));
        $$ = $1;
      }
      else {
        $$ = parser->ast()->createNodeIndexedAccess($1, $3);
      }
    }
  | reference T_ARRAY_OPEN array_filter_operator {
      // variable expansion, e.g. variable[*], with optional FILTER, LIMIT and RETURN clauses
      if ($3 > 1 && $1->type == NODE_TYPE_EXPANSION) {
        // create a dummy passthru node that reduces and evaluates the expansion first
        // and the expansion on top of the stack won't be chained with any other expansions
        $1 = parser->ast()->createNodePassthru($1);
      }

      // create a temporary iterator variable
      std::string const nextName = parser->ast()->variables()->nextName() + "_";
      char const* iteratorName = nextName.c_str();

      if ($1->type == NODE_TYPE_EXPANSION) {
        auto iterator = parser->ast()->createNodeIterator(iteratorName, $1->getMember(1));
        parser->pushStack(iterator);
      }
      else {
        auto iterator = parser->ast()->createNodeIterator(iteratorName, $1);
        parser->pushStack(iterator);
      }

      auto scopes = parser->ast()->scopes();
      scopes->stackCurrentVariable(scopes->getVariable(iteratorName));
    } optional_array_filter optional_array_limit optional_array_return T_ARRAY_CLOSE %prec EXPANSION {
      auto scopes = parser->ast()->scopes();
      scopes->unstackCurrentVariable();

      auto iterator = static_cast<AstNode const*>(parser->popStack());
      auto variableNode = iterator->getMember(0);
      TRI_ASSERT(variableNode->type == NODE_TYPE_VARIABLE);
      auto variable = static_cast<Variable const*>(variableNode->getData());

      if ($1->type == NODE_TYPE_EXPANSION) {
        auto expand = parser->ast()->createNodeExpansion($3, iterator, parser->ast()->createNodeReference(variable->name.c_str()), $5, $6, $7);
        $1->changeMember(1, expand);
        $$ = $1;
      }
      else {
        $$ = parser->ast()->createNodeExpansion($3, iterator, parser->ast()->createNodeReference(variable->name.c_str()), $5, $6, $7);
      }
    }
  ;

simple_value:
    value_literal {
      $$ = $1;
    }
  | bind_parameter {
      $$ = $1;
    }
  ;

numeric_value:
    T_INTEGER {
      if ($1 == nullptr) {
        ABORT_OOM
      }
      
      $$ = $1;
    }
  | T_DOUBLE {
      if ($1 == nullptr) {
        ABORT_OOM
      }

      $$ = $1;
    }
  ;
  
value_literal: 
    T_QUOTED_STRING {
      $$ = parser->ast()->createNodeValueString($1); 
    }
  | numeric_value {
      $$ = $1;
    }
  | T_NULL {
      $$ = parser->ast()->createNodeValueNull();
    }
  | T_TRUE {
      $$ = parser->ast()->createNodeValueBool(true);
    }
  | T_FALSE {
      $$ = parser->ast()->createNodeValueBool(false);
    }
  ;

collection_name:
    T_STRING {
      if ($1 == nullptr) {
        ABORT_OOM
      }

      $$ = parser->ast()->createNodeCollection($1, TRI_TRANSACTION_WRITE);
    }
  | T_QUOTED_STRING {
      if ($1 == nullptr) {
        ABORT_OOM
      }

      $$ = parser->ast()->createNodeCollection($1, TRI_TRANSACTION_WRITE);
    }
  | T_PARAMETER {
      if ($1 == nullptr) {
        ABORT_OOM
      }
      
      if (strlen($1) < 2 || $1[0] != '@') {
        parser->registerParseError(TRI_ERROR_QUERY_BIND_PARAMETER_TYPE, TRI_errno_string(TRI_ERROR_QUERY_BIND_PARAMETER_TYPE), $1, yylloc.first_line, yylloc.first_column);
      }

      $$ = parser->ast()->createNodeParameter($1);
    }
  ;

bind_parameter:
    T_PARAMETER {
      $$ = parser->ast()->createNodeParameter($1);
    }
  ;

object_element_name:
    T_STRING {
      if ($1 == nullptr) {
        ABORT_OOM
      }

      $$ = $1;
    }
  | T_QUOTED_STRING {
      if ($1 == nullptr) {
        ABORT_OOM
      }

      $$ = $1;
    }

variable_name:
    T_STRING {
      $$ = $1;
    }
  ;

