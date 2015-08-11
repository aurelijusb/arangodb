////////////////////////////////////////////////////////////////////////////////
/// @brief console thread
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
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_REST_SERVER_CONSOLE_THREAD_H
#define ARANGODB_REST_SERVER_CONSOLE_THREAD_H 1

#include "Basics/Common.h"
#include "Basics/Thread.h"
#include "V8/V8LineEditor.h"
#include "V8Server/ApplicationV8.h"

struct TRI_vocbase_t;

namespace triagens {
  namespace rest {
    class ApplicationServer;
  }

  namespace arango {

// -----------------------------------------------------------------------------
// --SECTION--                                               class ConsoleThread
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief the line editor object for use in debugging
////////////////////////////////////////////////////////////////////////////////

    extern std::atomic<V8LineEditor*> serverConsole;
    extern triagens::basics::Mutex serverConsoleMutex;

////////////////////////////////////////////////////////////////////////////////
/// @brief ArangoDB server
////////////////////////////////////////////////////////////////////////////////

    class ConsoleThread : public basics::Thread {
      private:
        ConsoleThread (const ConsoleThread&);
        ConsoleThread& operator= (const ConsoleThread&);

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        ConsoleThread (triagens::rest::ApplicationServer*,
                       ApplicationV8*,
                       TRI_vocbase_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~ConsoleThread ();

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the console thread is done
////////////////////////////////////////////////////////////////////////////////

        bool done () const {
          return (_done == 1);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the user abort flag
////////////////////////////////////////////////////////////////////////////////

        void userAbort () {
          _userAborted = true;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the thread is chatty on shutdown
////////////////////////////////////////////////////////////////////////////////

        bool isSilent () {
          return true;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief runs the thread
////////////////////////////////////////////////////////////////////////////////

        void run ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief inner thread loop
////////////////////////////////////////////////////////////////////////////////

        void inner ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

        rest::ApplicationServer* _applicationServer;

        ApplicationV8* _applicationV8;

        ApplicationV8::V8Context* _context;

        TRI_vocbase_t* _vocbase;

        sig_atomic_t _done;

        bool _userAborted;

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
