////////////////////////////////////////////////////////////////////////////////
/// @brief statistics basics
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
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2012-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_STATISTICS_STATISTICS_H
#define ARANGODB_STATISTICS_STATISTICS_H 1

#include "Basics/Common.h"
#include "Rest/HttpRequest.h"
#include "Statistics/figures.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief request statistics
////////////////////////////////////////////////////////////////////////////////

struct TRI_request_statistics_t {
  TRI_request_statistics_t ()
    : _readStart(0.0),
      _readEnd(0.0),
      _queueStart(0.0),
      _queueEnd(0.0),
      _requestStart(0.0),
      _requestEnd(0.0),
      _writeStart(0.0),
      _writeEnd(0.0),
      _receivedBytes(0.0),
      _sentBytes(0.0),
      _requestType(triagens::rest::HttpRequest::HTTP_REQUEST_ILLEGAL),
      _async(false),
      _tooLarge(false),
      _executeError(false),
      _ignore(false) {
  }

  void reset () {
    _readStart     = 0.0;
    _readEnd       = 0.0;
    _queueStart    = 0.0;
    _queueEnd      = 0.0;
    _requestStart  = 0.0;
    _requestEnd    = 0.0;
    _writeStart    = 0.0;
    _writeEnd      = 0.0;
    _receivedBytes = 0.0;
    _sentBytes     = 0.0;
    _requestType   = triagens::rest::HttpRequest::HTTP_REQUEST_ILLEGAL;
    _async         = false;
    _tooLarge      = false;
    _executeError  = false;
    _ignore        = false;
  }

  double _readStart;
  double _readEnd;
  double _queueStart;
  double _queueEnd;
  double _requestStart;
  double _requestEnd;
  double _writeStart;
  double _writeEnd;

  double _receivedBytes;
  double _sentBytes;

  triagens::rest::HttpRequest::HttpRequestType _requestType;

  bool _async;
  bool _tooLarge;
  bool _executeError;
  bool _ignore;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief connection statistics
////////////////////////////////////////////////////////////////////////////////

struct TRI_connection_statistics_t {
  TRI_connection_statistics_t ()
    : _connStart(0.0),
      _connEnd(0.0),
      _http(false),
      _error(false) {
  }

  void reset () {
    _connStart = 0.0;
    _connEnd   = 0.0;
    _http      = false;
    _error     = false;
  }
  
  double _connStart;
  double _connEnd;

  bool   _http;
  bool   _error;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief global server statistics
////////////////////////////////////////////////////////////////////////////////

struct TRI_server_statistics_t {
  TRI_server_statistics_t ()
    : _startTime(0.0),
      _uptime(0.0) {
  }

  double _startTime;
  double _uptime;
};

// -----------------------------------------------------------------------------
// --SECTION--                               public request statistics functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief gets a new statistics block
////////////////////////////////////////////////////////////////////////////////

TRI_request_statistics_t* TRI_AcquireRequestStatistics (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief releases a statistics block
////////////////////////////////////////////////////////////////////////////////

void TRI_ReleaseRequestStatistics (TRI_request_statistics_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief fills the current statistics
////////////////////////////////////////////////////////////////////////////////

void TRI_FillRequestStatistics (triagens::basics::StatisticsDistribution& totalTime,
                                triagens::basics::StatisticsDistribution& requestTime,
                                triagens::basics::StatisticsDistribution& queueTime,
                                triagens::basics::StatisticsDistribution& ioTime,
                                triagens::basics::StatisticsDistribution& bytesSent,
                                triagens::basics::StatisticsDistribution& bytesReceived);

// -----------------------------------------------------------------------------
// --SECTION--                            public connection statistics functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief gets a new statistics block
////////////////////////////////////////////////////////////////////////////////

TRI_connection_statistics_t* TRI_AcquireConnectionStatistics (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief releases a statistics block
////////////////////////////////////////////////////////////////////////////////

void TRI_ReleaseConnectionStatistics (TRI_connection_statistics_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief fills the current statistics
////////////////////////////////////////////////////////////////////////////////

void TRI_FillConnectionStatistics (triagens::basics::StatisticsCounter& httpConnections,
                                   triagens::basics::StatisticsCounter& totalRequests,
                                   std::vector<triagens::basics::StatisticsCounter>& methodRequests,
                                   triagens::basics::StatisticsCounter& asyncRequests,
                                   triagens::basics::StatisticsDistribution& connectionTime);

// -----------------------------------------------------------------------------
// --SECTION--                                public server statistics functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief gets the server statistics
////////////////////////////////////////////////////////////////////////////////

TRI_server_statistics_t TRI_GetServerStatistics ();

// -----------------------------------------------------------------------------
// --SECTION--                                                  public variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief statistics enabled flags
////////////////////////////////////////////////////////////////////////////////

extern bool TRI_ENABLE_STATISTICS;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of http connections
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsCounter TRI_HttpConnectionsStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief total number of requests
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsCounter TRI_TotalRequestsStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of requests by HTTP method
////////////////////////////////////////////////////////////////////////////////

extern std::vector<triagens::basics::StatisticsCounter> TRI_MethodRequestsStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief number of async requests
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsCounter TRI_AsyncRequestsStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief connection time distribution vector
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsVector TRI_ConnectionTimeDistributionVectorStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief total time distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_ConnectionTimeDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief request time distribution vector
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsVector TRI_RequestTimeDistributionVectorStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief total time distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_TotalTimeDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief request time distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_RequestTimeDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief queue time distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_QueueTimeDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief i/o distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_IoTimeDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief bytes sent distribution vector
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsVector TRI_BytesSentDistributionVectorStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief bytes sent distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_BytesSentDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief bytes received distribution vector
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsVector TRI_BytesReceivedDistributionVectorStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief bytes received distribution
////////////////////////////////////////////////////////////////////////////////

extern triagens::basics::StatisticsDistribution* TRI_BytesReceivedDistributionStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief global server statistics
////////////////////////////////////////////////////////////////////////////////

extern TRI_server_statistics_t TRI_ServerStatistics;

////////////////////////////////////////////////////////////////////////////////
/// @brief physical memory
////////////////////////////////////////////////////////////////////////////////

extern uint64_t TRI_PhysicalMemory;

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief gets the current wallclock time
////////////////////////////////////////////////////////////////////////////////

double TRI_StatisticsTime (void);

// -----------------------------------------------------------------------------
// --SECTION--                                             module initialisation
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief module init function
////////////////////////////////////////////////////////////////////////////////

void TRI_InitialiseStatistics (void);

////////////////////////////////////////////////////////////////////////////////
/// @brief shut down statistics
////////////////////////////////////////////////////////////////////////////////

void TRI_ShutdownStatistics (void);

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
