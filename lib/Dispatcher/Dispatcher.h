////////////////////////////////////////////////////////////////////////////////
/// @brief interface of a job dispatcher
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triAGENS GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Martin Schoenert
/// @author Copyright 2009-2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_DISPATCHER_DISPATCHER_H
#define TRIAGENS_DISPATCHER_DISPATCHER_H 1

#include "Basics/Common.h"

#include "Basics/Mutex.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

namespace triagens {
  namespace rest {
    class Job;
    class DispatcherQueue;
    class DispatcherThread;

// -----------------------------------------------------------------------------
// --SECTION--                                                  class Dispatcher
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief interface of a job dispatcher
////////////////////////////////////////////////////////////////////////////////

    class Dispatcher {
      private:
        Dispatcher (Dispatcher const&);
        Dispatcher& operator= (Dispatcher const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief queue thread creator
////////////////////////////////////////////////////////////////////////////////

        typedef DispatcherThread* (*newDispatcherThread_fptr)(DispatcherQueue*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                             public static methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief default queue thread creator
////////////////////////////////////////////////////////////////////////////////

        static DispatcherThread* defaultDispatcherThread (DispatcherQueue*);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        Dispatcher ();

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        virtual ~Dispatcher ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief is the dispatcher still running
////////////////////////////////////////////////////////////////////////////////

        bool isRunning ();

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a new queue
////////////////////////////////////////////////////////////////////////////////

        void addQueue (std::string const& name, size_t nrThreads);

/////////////////////////////////////////////////////////////////////////
/// @brief adds a queue which given dispatcher thread type
/////////////////////////////////////////////////////////////////////////

        void addQueue (std::string const& name, newDispatcherThread_fptr, size_t nrThreads);

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a new job
///
/// The method is called from the scheduler to add a new job request.  It
/// returns immediately (i.e. without waiting for the job to finish).  When the
/// job is finished the scheduler will be awoken and the scheduler will write
/// the response over the network to the caller.
////////////////////////////////////////////////////////////////////////////////

        bool addJob (Job*);

////////////////////////////////////////////////////////////////////////////////
/// @brief start the dispatcher
////////////////////////////////////////////////////////////////////////////////

        bool start ();

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if the dispatcher queues are up and running
////////////////////////////////////////////////////////////////////////////////

        bool isStarted ();

////////////////////////////////////////////////////////////////////////////////
/// @brief opens the dispatcher for business
////////////////////////////////////////////////////////////////////////////////

        bool open ();

////////////////////////////////////////////////////////////////////////////////
/// @brief begins shutdown process
////////////////////////////////////////////////////////////////////////////////

        void beginShutdown ();

////////////////////////////////////////////////////////////////////////////////
/// @brief shut downs the queue
////////////////////////////////////////////////////////////////////////////////

        void shutdown ();

////////////////////////////////////////////////////////////////////////////////
/// @brief reports status of dispatcher queues
////////////////////////////////////////////////////////////////////////////////

        void reportStatus ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 protected methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up a queue
////////////////////////////////////////////////////////////////////////////////

        DispatcherQueue* lookupQueue (string const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Dispatcher
/// @{
////////////////////////////////////////////////////////////////////////////////

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief dispatcher lock
////////////////////////////////////////////////////////////////////////////////

        basics::Mutex _accessDispatcher;

////////////////////////////////////////////////////////////////////////////////
/// @brief shutdown indicator
////////////////////////////////////////////////////////////////////////////////

        volatile sig_atomic_t _stopping;

////////////////////////////////////////////////////////////////////////////////
/// @brief dispatcher queues
////////////////////////////////////////////////////////////////////////////////

        map<string, DispatcherQueue*> _queues;
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}\\)"
// End:
