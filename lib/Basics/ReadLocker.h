////////////////////////////////////////////////////////////////////////////////
/// @brief read locker
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

#ifndef ARANGODB_BASICS_READ_LOCKER_H
#define ARANGODB_BASICS_READ_LOCKER_H 1

#include "Basics/Common.h"
#include "Basics/ReadWriteLock.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                     public macros
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief construct locker with file and line information
///
/// Ones needs to use macros twice to get a unique variable based on the line
/// number.
////////////////////////////////////////////////////////////////////////////////

#define READ_LOCKER_VAR_A(a) _read_lock_variable ## a
#define READ_LOCKER_VAR_B(a) READ_LOCKER_VAR_A(a)

#ifdef TRI_SHOW_LOCK_TIME

#define READ_LOCKER(b) \
  triagens::basics::ReadLocker READ_LOCKER_VAR_B(__LINE__)(&b, __FILE__, __LINE__)

#define READ_LOCKER_EVENTUAL(b, t) \
  triagens::basics::ReadLocker READ_LOCKER_VAR_B(__LINE__)(&b, t, __FILE__, __LINE__)

#else

#define READ_LOCKER(b) \
  triagens::basics::ReadLocker READ_LOCKER_VAR_B(__LINE__)(&b)

#define READ_LOCKER_EVENTUAL(b, t) \
  triagens::basics::ReadLocker READ_LOCKER_VAR_B(__LINE__)(&b, t)

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                  class ReadLocker
// -----------------------------------------------------------------------------

namespace triagens {
  namespace basics {

////////////////////////////////////////////////////////////////////////////////
/// @brief read locker
///
/// A ReadLocker read-locks a read-write lock during its lifetime and unlocks
/// the lock when it is destroyed.
////////////////////////////////////////////////////////////////////////////////

    class ReadLocker {
        ReadLocker (ReadLocker const&);
        ReadLocker& operator= (ReadLocker const&);

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief aquires a read-lock
///
/// The constructors read-locks the lock, the destructors unlocks the lock.
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_SHOW_LOCK_TIME

        ReadLocker (ReadWriteLock* readWriteLock, char const* file, int line);

#else

        explicit
        ReadLocker (ReadWriteLock*);

#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief aquires a read-lock, with periodic sleeps while not acquired
/// sleep time is specified in nanoseconds
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_SHOW_LOCK_TIME

        ReadLocker (ReadWriteLock* readWriteLock, uint64_t sleepDelay, char const* file, int line);

#else

        ReadLocker (ReadWriteLock*, uint64_t);

#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief releases the read-lock
////////////////////////////////////////////////////////////////////////////////

        ~ReadLocker ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief the read-write lock
////////////////////////////////////////////////////////////////////////////////

        ReadWriteLock* _readWriteLock;

#ifdef TRI_SHOW_LOCK_TIME

////////////////////////////////////////////////////////////////////////////////
/// @brief file
////////////////////////////////////////////////////////////////////////////////

        char const* _file;

////////////////////////////////////////////////////////////////////////////////
/// @brief line number
////////////////////////////////////////////////////////////////////////////////

        int _line;

////////////////////////////////////////////////////////////////////////////////
/// @brief lock time
////////////////////////////////////////////////////////////////////////////////

        double _time;

#endif        

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
