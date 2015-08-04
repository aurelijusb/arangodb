////////////////////////////////////////////////////////////////////////////////
/// @brief Aql, NodeFinder
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

#ifndef ARANGODB_AQL_NODE_FINDER_H
#define ARANGODB_AQL_NODE_FINDER_H 1

#include "Basics/Common.h"
#include "Aql/ExecutionNode.h"
#include "Aql/WalkerWorker.h"

namespace triagens {
  namespace aql {

// -----------------------------------------------------------------------------
// --SECTION--                                                  class NodeFinder
// -----------------------------------------------------------------------------

    template<typename T>
    class NodeFinder final : public WalkerWorker<ExecutionNode> {

        T _lookingFor;

        std::vector<ExecutionNode*>& _out;

        bool _enterSubqueries;

      public:

        NodeFinder (T,
                    std::vector<ExecutionNode*>&,
                    bool);

        bool before (ExecutionNode*) override final;

        bool enterSubquery (ExecutionNode*, ExecutionNode*) override final {
          return _enterSubqueries;
        }

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
