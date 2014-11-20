////////////////////////////////////////////////////////////////////////////////
/// @brief Ahuacatl, scopes
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2014 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2012-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_AHUACATL_AHUACATL__SCOPE_H
#define ARANGODB_AHUACATL_AHUACATL__SCOPE_H 1

#include "Basics/Common.h"

#include "BasicsC/associative.h"
#include "BasicsC/vector.h"

#include "Ahuacatl/ahuacatl-ast-node.h"
#include "Ahuacatl/ahuacatl-context.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief scope limit status
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_AQL_LIMIT_UNDEFINED = 0,    // no limit optimisation to be used
  TRI_AQL_LIMIT_USE       = 1,    // use limit optimisation
  TRI_AQL_LIMIT_IGNORE    = 2     // limit optimisation prohibited
}
TRI_aql_limit_status_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief scope types used by various components
////////////////////////////////////////////////////////////////////////////////

typedef enum {
  TRI_AQL_SCOPE_MAIN,
  TRI_AQL_SCOPE_SUBQUERY,
  TRI_AQL_SCOPE_FOR,
  TRI_AQL_SCOPE_FOR_NESTED,
  TRI_AQL_SCOPE_FUNCTION,
  TRI_AQL_SCOPE_EXPAND
}
TRI_aql_scope_e;

////////////////////////////////////////////////////////////////////////////////
/// @brief limit type, used in scope and hints
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_aql_limit_s {
  int64_t                _offset;
  int64_t                _limit;
  TRI_aql_limit_status_e _status;
  uint32_t               _found;
  bool                   _hasFilter;
}
TRI_aql_limit_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief scope type
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_aql_scope_s {
  TRI_associative_pointer_t  _variables;     // variables contained in the scope
  TRI_vector_pointer_t*      _ranges;        // filter conditions, used for optimisation
  TRI_aql_limit_t            _limit;         // scope limit, used for optimisation
  TRI_vector_pointer_t       _sorts;         // scope sort criteria, used for optimisation
  TRI_aql_scope_e            _type;          // type of scope
  size_t                     _level;         // level (start at 0)
  bool                       _selfContained; // means: scope doesn't refer to exterior variables
  bool                       _empty;
}
TRI_aql_scope_t;

////////////////////////////////////////////////////////////////////////////////
/// @brief hint for an AQL for loop
////////////////////////////////////////////////////////////////////////////////

typedef struct TRI_aql_for_hint_s {
  TRI_aql_limit_t _limit;
  bool            _isIncremental;
}
TRI_aql_for_hint_t;

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief create a hint for an AQL for
////////////////////////////////////////////////////////////////////////////////

TRI_aql_for_hint_t* TRI_CreateForHintScopeAql (TRI_aql_context_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief free a hint for an AQL for
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeForHintScopeAql (TRI_aql_for_hint_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief get the name of a scope type
////////////////////////////////////////////////////////////////////////////////

const char* TRI_TypeNameScopeAql (const TRI_aql_scope_e);

////////////////////////////////////////////////////////////////////////////////
/// @brief init scopes
////////////////////////////////////////////////////////////////////////////////

void TRI_InitScopesAql (TRI_aql_context_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief free all scopes
////////////////////////////////////////////////////////////////////////////////

void TRI_FreeScopesAql (TRI_aql_context_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief add a new scope
////////////////////////////////////////////////////////////////////////////////

bool TRI_StartScopeAql (TRI_aql_context_t* const, const TRI_aql_scope_e);

////////////////////////////////////////////////////////////////////////////////
/// @brief end the current scope, only 1
////////////////////////////////////////////////////////////////////////////////

bool TRI_EndScopeAql (TRI_aql_context_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief end the current scopes, 0 .. n
////////////////////////////////////////////////////////////////////////////////

bool TRI_EndScopeByReturnAql (TRI_aql_context_t* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if a variable is defined in the current scope or above
////////////////////////////////////////////////////////////////////////////////

bool TRI_VariableExistsScopeAql (TRI_aql_context_t* const, const char* const);

////////////////////////////////////////////////////////////////////////////////
/// @brief push a variable into the current scope's symbol table
////////////////////////////////////////////////////////////////////////////////

bool TRI_AddVariableScopeAql (TRI_aql_context_t* const,
                              const char*,
                              TRI_aql_node_t* const,
                              bool);

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End: