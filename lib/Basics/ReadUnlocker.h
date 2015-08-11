////////////////////////////////////////////////////////////////////////////////
/// @brief read unlocker
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
/// @author Frank Celler
/// @author Achim Brandt
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2010-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_READ_UNLOCKER_H
#define ARANGODB_BASICS_READ_UNLOCKER_H 1

#include "Basics/Common.h"

#include "Basics/ReadWriteLock.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                     public macros
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief construct unlocker with file and line information
///
/// Ones needs to use macros twice to get a unique variable based on the line
/// number.
////////////////////////////////////////////////////////////////////////////////

#define READ_UNLOCKER_VAR_A(a) _read_unlock_variable ## a
#define READ_UNLOCKER_VAR_B(a) READ_UNLOCKER_VAR_A(a)

#define READ_UNLOCKER(b) \
  triagens::basics::ReadUnlocker READ_UNLOCKER_VAR_B(__LINE__)(&b, __FILE__, __LINE__)

// -----------------------------------------------------------------------------
// --SECTION--                                                class ReadUnlocker
// -----------------------------------------------------------------------------

namespace triagens {
  namespace basics {

////////////////////////////////////////////////////////////////////////////////
/// @brief read unlocker
///
/// A ReadUnlocker unlocks a read-write lock during its lifetime and reaquires
/// the read-lock again when it is destroyed.
////////////////////////////////////////////////////////////////////////////////

    class ReadUnlocker {
        ReadUnlocker (ReadUnlocker const&);
        ReadUnlocker& operator= (ReadUnlocker const&);

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief unlocks the lock
///
/// The constructor unlocks the lock, the destructors aquires a read-lock.
////////////////////////////////////////////////////////////////////////////////

        explicit
        ReadUnlocker (ReadWriteLock* readWriteLock);

////////////////////////////////////////////////////////////////////////////////
/// @brief unlocks the lock
///
/// The constructor unlocks the lock, the destructors aquires a read-lock.
////////////////////////////////////////////////////////////////////////////////

        ReadUnlocker (ReadWriteLock* readWriteLock, char const* file, int line);

////////////////////////////////////////////////////////////////////////////////
/// @brief aquires the read-lock
////////////////////////////////////////////////////////////////////////////////

        ~ReadUnlocker ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief the read-write lock
////////////////////////////////////////////////////////////////////////////////

        ReadWriteLock* _readWriteLock;

////////////////////////////////////////////////////////////////////////////////
/// @brief file
////////////////////////////////////////////////////////////////////////////////

        char const* _file;

////////////////////////////////////////////////////////////////////////////////
/// @brief line number
////////////////////////////////////////////////////////////////////////////////

        int _line;
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
