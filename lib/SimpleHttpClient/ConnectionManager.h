////////////////////////////////////////////////////////////////////////////////
/// @brief manages open HTTP connections on the client side
///
/// @file ConnectionManager.h
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
/// @author Max Neunhoeffer
/// @author Copyright 2014, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_SIMPLE_HTTP_CLIENT_CONNECTION_MANAGER_H
#define ARANGODB_SIMPLE_HTTP_CLIENT_CONNECTION_MANAGER_H 1

#include "Basics/Common.h"
#include "Basics/ReadWriteLock.h"
#include "SimpleHttpClient/GeneralClientConnection.h"

// TODO: change to constexpr when feasible
#define CONNECTION_MANAGER_BUCKETS 8 

namespace triagens {
  namespace httpclient {

// -----------------------------------------------------------------------------
// --SECTION--                                                 ConnectionManager
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief options for connections
////////////////////////////////////////////////////////////////////////////////

    struct ConnectionOptions {
      double _connectTimeout;
      double _requestTimeout;
      size_t _connectRetries;
      double _singleRequestTimeout;
      uint32_t _sslProtocol;
    };

////////////////////////////////////////////////////////////////////////////////
/// @brief the class to manage open client connections
////////////////////////////////////////////////////////////////////////////////

    class ConnectionManager {

// -----------------------------------------------------------------------------
// --SECTION--                                     constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief initializes library
///
/// We are a singleton class, therefore nobody is allowed to create
/// new instances or copy them, except we ourselves.
////////////////////////////////////////////////////////////////////////////////

      private:

        ConnectionManager () = default; 

        ConnectionManager (ConnectionManager const&) = delete;
        ConnectionManager& operator= (ConnectionManager const&) = delete;

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief shuts down library
////////////////////////////////////////////////////////////////////////////////

        ~ConnectionManager ( );

////////////////////////////////////////////////////////////////////////////////
/// @brief initializes library
////////////////////////////////////////////////////////////////////////////////

        static void initialize ();

// -----------------------------------------------------------------------------
// --SECTION--                                                 public subclasses
// -----------------------------------------------------------------------------

        struct ServerConnections;

////////////////////////////////////////////////////////////////////////////////
/// @brief class to administrate one connection to a server
////////////////////////////////////////////////////////////////////////////////

        struct SingleServerConnection {
          ServerConnections*         _connections;
          GeneralClientConnection*   _connection;
          triagens::rest::Endpoint*  _endpoint;
          std::string const          _endpointSpecification;
          time_t                     _lastUsed;

          SingleServerConnection (ServerConnections* manager,
                                  GeneralClientConnection* connection,
                                  triagens::rest::Endpoint* endpoint,
                                  std::string const& endpointSpecification)
            : _connections(manager),
              _connection(connection), 
              _endpoint(endpoint), 
              _endpointSpecification(endpointSpecification),
              _lastUsed(::time(0)) {
          }

          ~SingleServerConnection ();

        };

////////////////////////////////////////////////////////////////////////////////
/// @brief class to administrate all connections to a server
////////////////////////////////////////////////////////////////////////////////

        struct ServerConnections {
          std::vector<SingleServerConnection*> _connections;
          std::list<SingleServerConnection*>   _unused;
          triagens::basics::ReadWriteLock      _lock;

          ServerConnections () = default;

          ~ServerConnections ();   // closes all connections

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a single connection
////////////////////////////////////////////////////////////////////////////////

          void addConnection (SingleServerConnection*);

////////////////////////////////////////////////////////////////////////////////
/// @brief pop a free connection - returns nullptr if no connection is
/// available
////////////////////////////////////////////////////////////////////////////////

          SingleServerConnection* popConnection ();

////////////////////////////////////////////////////////////////////////////////
/// @brief push a unused connection back on the stack, allowing its re-use
////////////////////////////////////////////////////////////////////////////////

          void pushConnection (SingleServerConnection*);

////////////////////////////////////////////////////////////////////////////////
/// @brief remove a (broken) connection from the list of connections
////////////////////////////////////////////////////////////////////////////////

          void removeConnection (SingleServerConnection*);

////////////////////////////////////////////////////////////////////////////////
/// @brief closes unused connections
////////////////////////////////////////////////////////////////////////////////
          
          void closeUnusedConnections (double);
        };

// -----------------------------------------------------------------------------
// --SECTION--                                                   public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief get the unique instance
////////////////////////////////////////////////////////////////////////////////

        static ConnectionManager* instance ();

////////////////////////////////////////////////////////////////////////////////
/// @brief open or get a previously cached connection to a server
////////////////////////////////////////////////////////////////////////////////

        SingleServerConnection* leaseConnection (std::string const& endpoint);

////////////////////////////////////////////////////////////////////////////////
/// @brief return leased connection to a server
////////////////////////////////////////////////////////////////////////////////

        void returnConnection (SingleServerConnection* singleConnection);

////////////////////////////////////////////////////////////////////////////////
/// @brief report a leased connection as being broken
////////////////////////////////////////////////////////////////////////////////

        void brokenConnection(SingleServerConnection* singleConnection);

////////////////////////////////////////////////////////////////////////////////
/// @brief closes all connections that have been unused for more than
/// limit seconds
////////////////////////////////////////////////////////////////////////////////

        void closeUnusedConnections (double limit);

// -----------------------------------------------------------------------------
// --SECTION--                                         private methods and data
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief hash the endpoint value into a bucket
////////////////////////////////////////////////////////////////////////////////

        size_t bucket (std::string const& endpoint) const {
          return std::hash<std::string>()(endpoint) % CONNECTION_MANAGER_BUCKETS;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief global options for connections
////////////////////////////////////////////////////////////////////////////////

        static ConnectionOptions _globalConnectionOptions;

////////////////////////////////////////////////////////////////////////////////
/// @brief map to store all connections to all servers with corresponding lock
////////////////////////////////////////////////////////////////////////////////

        // We keep connections to servers open:
        struct TRI_ALIGNAS(64) ConnectionsBucket {
          std::unordered_map<std::string, ServerConnections*> _connections;

          triagens::basics::ReadWriteLock _lock;
        };

        ConnectionsBucket _connectionsBuckets[CONNECTION_MANAGER_BUCKETS];

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
