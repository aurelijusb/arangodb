////////////////////////////////////////////////////////////////////////////////
/// @brief DB server job
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
/// @author Copyright 2009-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "ServerJob.h"

#include "Basics/MutexLocker.h"
#include "Basics/logging.h"
#include "Cluster/HeartbeatThread.h"
#include "Dispatcher/DispatcherQueue.h"
#include "V8/v8-utils.h"
#include "V8Server/ApplicationV8.h"
#include "VocBase/server.h"
#include "VocBase/vocbase.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                 class ServerJob
// -----------------------------------------------------------------------------

static triagens::basics::Mutex ExecutorLock;

using namespace triagens::arango;
using namespace triagens::rest;

////////////////////////////////////////////////////////////////////////////////
/// @brief general server job
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new db server job
////////////////////////////////////////////////////////////////////////////////

ServerJob::ServerJob (HeartbeatThread* heartbeat,
                      TRI_server_t* server,
                      ApplicationV8* applicationV8)
  : Job("HttpServerJob"),
    _heartbeat(heartbeat),
    _server(server),
    _applicationV8(applicationV8),
    _shutdown(0),
    _abandon(false) {
}

////////////////////////////////////////////////////////////////////////////////
/// @brief destructs a db server job
////////////////////////////////////////////////////////////////////////////////

ServerJob::~ServerJob () {
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       Job methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

Job::status_t ServerJob::work () {
  LOG_TRACE("starting plan update handler");

  if (_shutdown != 0) {
    return status_t(Job::JOB_DONE);
  }
 
  _heartbeat->setReady();

  bool result;

  { 
    // only one plan change at a time
    MUTEX_LOCKER(ExecutorLock);

    result = execute();
  }
    
  _heartbeat->removeDispatchedJob();

  if (result) {
    // tell the heartbeat thread that the server job was
    // executed successfully
    return status_t(Job::JOB_DONE);
  }

  return status_t(Job::JOB_FAILED);
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

bool ServerJob::cancel () {
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/// {@inheritDoc}
////////////////////////////////////////////////////////////////////////////////

void ServerJob::cleanup (DispatcherQueue* queue) {
  queue->removeJob(this);
  delete this;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief execute job
////////////////////////////////////////////////////////////////////////////////

bool ServerJob::execute () {
  // default to system database
  TRI_vocbase_t* vocbase = TRI_UseDatabaseServer(_server, TRI_VOC_SYSTEM_DATABASE);

  if (vocbase == nullptr) {
    // database is gone
    return false;
  }

  ApplicationV8::V8Context* context = _applicationV8->enterContext(vocbase, true);

  if (context == nullptr) {
    TRI_ReleaseDatabaseServer(_server, vocbase);
    return false;
  }

  auto isolate = context->isolate;
  try {
    v8::HandleScope scope(isolate);
    
    // execute script inside the context
    auto file = TRI_V8_ASCII_STRING("handle-plan-change");
    auto content = TRI_V8_ASCII_STRING("require('org/arangodb/cluster').handlePlanChange();");
    TRI_ExecuteJavaScriptString(isolate, isolate->GetCurrentContext(), content, file, false);
  }
  catch (...) {
  }


  // get the pointer to the last used vocbase
  TRI_GET_GLOBALS();
  void* orig = v8g->_vocbase;

  _applicationV8->exitContext(context);
  TRI_ReleaseDatabaseServer(_server, static_cast<TRI_vocbase_t*>(orig));

  return true;
}

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
