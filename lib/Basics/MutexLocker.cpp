////////////////////////////////////////////////////////////////////////////////
/// @brief Mutex Locker
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

#include "MutexLocker.h"

using namespace triagens::basics;

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief aquires a lock
///
/// The constructor aquires a lock, the destructors releases the lock.
////////////////////////////////////////////////////////////////////////////////

MutexLocker::MutexLocker (Mutex* mutex)
  : MutexLocker(mutex, nullptr, 0) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief aquires a lock
///
/// The constructor aquires a lock, the destructors releases the lock.
////////////////////////////////////////////////////////////////////////////////

MutexLocker::MutexLocker (Mutex* mutex, char const* file, int line)
  : _mutex(mutex), _file(file), _line(line) {
  _mutex->lock();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief releases the lock
////////////////////////////////////////////////////////////////////////////////

MutexLocker::~MutexLocker () {
  _mutex->unlock();
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
