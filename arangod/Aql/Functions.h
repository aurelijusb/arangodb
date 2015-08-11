////////////////////////////////////////////////////////////////////////////////
/// @brief Aql, C++ implementation of AQL functions
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

#ifndef ARANGODB_AQL_FUNCTIONS_H
#define ARANGODB_AQL_FUNCTIONS_H 1

#include "Basics/Common.h"
#include "Aql/AqlValue.h"
#include "Basics/tri-strings.h"
#include "Utils/AqlTransaction.h"

#include <functional>
      
namespace triagens {
  namespace aql {

    class Query;

// -----------------------------------------------------------------------------
// --SECTION--                                                   public typedefs
// -----------------------------------------------------------------------------
    
    typedef std::function<bool()> ExecutionCondition;
    
    typedef std::vector<std::pair<AqlValue, TRI_document_collection_t const*>> FunctionParameters;

    typedef std::function<AqlValue(triagens::aql::Query*,
                                   triagens::arango::AqlTransaction*,
                                   FunctionParameters const&)> FunctionImplementation;


    struct Functions {

// -----------------------------------------------------------------------------
// --SECTION--                                      AQL functions public helpers
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief called before a query starts
/// has the chance to set up any thread-local storage
////////////////////////////////////////////////////////////////////////////////

      static void InitializeThreadContext ();

////////////////////////////////////////////////////////////////////////////////
/// @brief called when a query ends
/// its responsibility is to clear any thread-local storage
////////////////////////////////////////////////////////////////////////////////

      static void DestroyThreadContext ();

// -----------------------------------------------------------------------------
// --SECTION--                                             AQL function bindings
// -----------------------------------------------------------------------------

      static AqlValue IsNull        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue IsBool        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue IsNumber      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue IsString      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue IsArray       (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue IsObject      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue ToNumber      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue ToString      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue ToBool        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue ToArray       (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Length        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Concat        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Like          (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Passthru      (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Unset         (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Keep          (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Merge         (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Has           (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Attributes    (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Values        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Min           (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Max           (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Sum           (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Average       (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Md5           (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Sha1          (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Unique        (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Union         (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue UnionDistinct (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Intersection  (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
      static AqlValue Neighbors     (triagens::aql::Query*, triagens::arango::AqlTransaction*, FunctionParameters const&);
    };

  }
}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
