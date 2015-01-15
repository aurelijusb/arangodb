////////////////////////////////////////////////////////////////////////////////
/// @brief MVCC transaction collection object
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
/// @author Copyright 2015, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_MVCC_TRANSACTION_COLLECTION_H
#define ARANGODB_MVCC_TRANSACTION_COLLECTION_H 1

#include "Basics/Common.h"
#include "VocBase/voc-types.h"
#include "VocBase/vocbase.h"

struct TRI_shaper_s;
struct TRI_vocbase_s;
struct TRI_vocbase_col_s;

namespace triagens {
  namespace mvcc {
    
// -----------------------------------------------------------------------------
// --SECTION--                                       class TransactionCollection
// -----------------------------------------------------------------------------

    class TransactionCollection {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief create the collection, using a collection id
////////////////////////////////////////////////////////////////////////////////

        TransactionCollection (struct TRI_vocbase_s*,
                               TRI_voc_cid_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief create the collection, using a collection name
////////////////////////////////////////////////////////////////////////////////

        TransactionCollection (struct TRI_vocbase_s*,
                               std::string const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroy the collection
////////////////////////////////////////////////////////////////////////////////

        ~TransactionCollection ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief return the collection id
////////////////////////////////////////////////////////////////////////////////

        inline TRI_voc_cid_t id () const {
          return _collection->_cid;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the collection name
////////////////////////////////////////////////////////////////////////////////

        inline std::string name () const {
          return std::string(_collection->_name);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the collection shaper
////////////////////////////////////////////////////////////////////////////////

        struct TRI_shaper_s* shaper () const;

////////////////////////////////////////////////////////////////////////////////
/// @brief generate a key
////////////////////////////////////////////////////////////////////////////////

        std::string generateKey (TRI_voc_tick_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief validate a key
/// this will throw if the key is invalid
////////////////////////////////////////////////////////////////////////////////

        void validateKey (char const*) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief get a string representation of the collection
////////////////////////////////////////////////////////////////////////////////

        std::string toString () const;

////////////////////////////////////////////////////////////////////////////////
/// @brief append the transaction to an output stream
////////////////////////////////////////////////////////////////////////////////
    
        friend std::ostream& operator<< (std::ostream&, TransactionCollection const*);

        friend std::ostream& operator<< (std::ostream&, TransactionCollection const&);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief pointer to the vocbase
////////////////////////////////////////////////////////////////////////////////

        struct TRI_vocbase_s* _vocbase; 

////////////////////////////////////////////////////////////////////////////////
/// @brief pointer to the vocbase collection
////////////////////////////////////////////////////////////////////////////////

        struct TRI_vocbase_col_s* _collection; 
      
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