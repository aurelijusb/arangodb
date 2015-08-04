////////////////////////////////////////////////////////////////////////////////
/// @brief Condition Locker
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
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2008-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_CONDITION_LOCKER_H
#define ARANGODB_BASICS_CONDITION_LOCKER_H 1

#include "Basics/Common.h"
#include "Basics/ConditionVariable.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                     public macros
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief construct locker with file and line information
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_SHOW_LOCK_TIME

#define CONDITION_LOCKER(a, b) \
  triagens::basics::ConditionLocker a(&b, __FILE__, __LINE__)

#else

#define CONDITION_LOCKER(a, b) \
  triagens::basics::ConditionLocker a(&b)

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                             class ConditionLocker
// -----------------------------------------------------------------------------

namespace triagens {
  namespace basics {

////////////////////////////////////////////////////////////////////////////////
/// @brief condition locker
///
/// A ConditionLocker locks a condition when constructed and releases the lock
/// when destroyed. It is possible the wait for an event in which case the lock
/// is released or to broadcast an event.
////////////////////////////////////////////////////////////////////////////////

    class  ConditionLocker {
        ConditionLocker (ConditionLocker const&);
        ConditionLocker& operator= (ConditionLocker const&);

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief locks the condition variable
///
/// The constructor locks the condition variable, the destructor unlocks
/// the condition variable
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_SHOW_LOCK_TIME

        ConditionLocker (ConditionVariable* conditionVariable, char const* file, int line);

#else

        explicit
        ConditionLocker (ConditionVariable* conditionVariable);

#endif        

////////////////////////////////////////////////////////////////////////////////
/// @brief unlocks the condition variable
////////////////////////////////////////////////////////////////////////////////

        ~ConditionLocker ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief waits for an event to occur
////////////////////////////////////////////////////////////////////////////////

        void wait ();

////////////////////////////////////////////////////////////////////////////////
/// @brief waits for an event to occur, using a timeout in micro seconds
////////////////////////////////////////////////////////////////////////////////

        bool wait (uint64_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief broadcasts an event
////////////////////////////////////////////////////////////////////////////////

        void broadcast ();

////////////////////////////////////////////////////////////////////////////////
/// @brief signals an event
////////////////////////////////////////////////////////////////////////////////

        void signal ();

////////////////////////////////////////////////////////////////////////////////
/// @brief unlocks the variable (handle with care, no exception allowed)
////////////////////////////////////////////////////////////////////////////////

        void unlock ();

////////////////////////////////////////////////////////////////////////////////
/// @brief relock the variable after unlock
////////////////////////////////////////////////////////////////////////////////

        void lock ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief the condition
////////////////////////////////////////////////////////////////////////////////

        ConditionVariable* _conditionVariable;

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
