////////////////////////////////////////////////////////////////////////////////
/// @brief handler factory
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
/// @author Copyright 2009-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_HTTP_SERVER_HTTP_HANDLER_FACTORY_H
#define ARANGODB_HTTP_SERVER_HTTP_HANDLER_FACTORY_H 1

#include "Basics/Common.h"

#include "Basics/Mutex.h"
#include "Basics/ReadWriteLock.h"
#include "Rest/HttpResponse.h"

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

namespace triagens {
  namespace rest {
    struct ConnectionInfo;
    class HttpHandler;
    class HttpRequest;
    class HttpResponse;

// -----------------------------------------------------------------------------
// --SECTION--                                          class HttpHandlerFactory
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief handler factory
////////////////////////////////////////////////////////////////////////////////

    class HttpHandlerFactory {

// -----------------------------------------------------------------------------
// --SECTION--                                                      public types
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief handler
////////////////////////////////////////////////////////////////////////////////

        typedef HttpHandler GeneralHandler;

////////////////////////////////////////////////////////////////////////////////
/// @brief request
////////////////////////////////////////////////////////////////////////////////

        typedef HttpRequest GeneralRequest;

////////////////////////////////////////////////////////////////////////////////
/// @brief response
////////////////////////////////////////////////////////////////////////////////

        typedef HttpResponse GeneralResponse;

////////////////////////////////////////////////////////////////////////////////
/// @brief handler creator
////////////////////////////////////////////////////////////////////////////////

        typedef HttpHandler* (*create_fptr) (HttpRequest*, void*);

////////////////////////////////////////////////////////////////////////////////
/// @brief context handler
////////////////////////////////////////////////////////////////////////////////

        typedef bool (*context_fptr) (HttpRequest*, void*);

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructs a new handler factory
////////////////////////////////////////////////////////////////////////////////

        HttpHandlerFactory (std::string const&,
                            int32_t,
                            bool,
                            context_fptr,
                            void*);

////////////////////////////////////////////////////////////////////////////////
/// @brief clones a handler factory
////////////////////////////////////////////////////////////////////////////////

        HttpHandlerFactory (HttpHandlerFactory const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief copies a handler factory
////////////////////////////////////////////////////////////////////////////////

        HttpHandlerFactory& operator= (HttpHandlerFactory const&);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructs a handler factory
////////////////////////////////////////////////////////////////////////////////

        virtual ~HttpHandlerFactory ();

// -----------------------------------------------------------------------------
// --SECTION--                                             static public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief sets maintenance mode
////////////////////////////////////////////////////////////////////////////////

        static void setMaintenance (bool);

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief returns header and body size restrictions
////////////////////////////////////////////////////////////////////////////////

        virtual pair<size_t, size_t> sizeRestrictions () const;

////////////////////////////////////////////////////////////////////////////////
/// @brief authenticates a new request, wrapper method
////////////////////////////////////////////////////////////////////////////////

        virtual HttpResponse::HttpResponseCode authenticateRequest (HttpRequest*);

////////////////////////////////////////////////////////////////////////////////
/// @brief set request context, wrapper method
////////////////////////////////////////////////////////////////////////////////

        virtual bool setRequestContext (HttpRequest*);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the authentication realm
////////////////////////////////////////////////////////////////////////////////

        virtual std::string const& authenticationRealm (HttpRequest*) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new request
////////////////////////////////////////////////////////////////////////////////

        virtual HttpRequest* createRequest (ConnectionInfo const&,
                                            char const*,
                                            size_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a new handler
////////////////////////////////////////////////////////////////////////////////

        virtual HttpHandler* createHandler (HttpRequest*);

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a path and constructor to the factory
////////////////////////////////////////////////////////////////////////////////

        void addHandler (string const& path, create_fptr, void* data = 0);

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a prefix path and constructor to the factory
////////////////////////////////////////////////////////////////////////////////

        void addPrefixHandler (string const& path, create_fptr, void* data = 0);

////////////////////////////////////////////////////////////////////////////////
/// @brief adds a path and constructor to the factory
////////////////////////////////////////////////////////////////////////////////

        void addNotFoundHandler (create_fptr);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief authentication realm
////////////////////////////////////////////////////////////////////////////////

        string _authenticationRealm;

////////////////////////////////////////////////////////////////////////////////
/// @brief minimum compatibility
/// the value is an ArangoDB version number in the following format:
/// 10000 * major + 100 * minor (e.g. 10400 for ArangoDB 1.4)
////////////////////////////////////////////////////////////////////////////////

        int32_t _minCompatibility;

////////////////////////////////////////////////////////////////////////////////
/// @brief allow overriding HTTP request method with custom headers
////////////////////////////////////////////////////////////////////////////////

        bool _allowMethodOverride;

////////////////////////////////////////////////////////////////////////////////
/// @brief set context callback
////////////////////////////////////////////////////////////////////////////////

        context_fptr _setContext;

////////////////////////////////////////////////////////////////////////////////
/// @brief set context data
////////////////////////////////////////////////////////////////////////////////

        void* _setContextData;

////////////////////////////////////////////////////////////////////////////////
/// @brief authentication lock for cache
////////////////////////////////////////////////////////////////////////////////

        basics::ReadWriteLock _authLock;

////////////////////////////////////////////////////////////////////////////////
/// @brief authentication cache
////////////////////////////////////////////////////////////////////////////////

        std::map<std::string, std::string> _authCache;

////////////////////////////////////////////////////////////////////////////////
/// @brief list of constructors
////////////////////////////////////////////////////////////////////////////////

        map<string, create_fptr> _constructors;

////////////////////////////////////////////////////////////////////////////////
/// @brief list of data pointers for constructors
////////////////////////////////////////////////////////////////////////////////

        map<string, void*> _datas;

////////////////////////////////////////////////////////////////////////////////
/// @brief list of prefix handlers
////////////////////////////////////////////////////////////////////////////////

        vector<string> _prefixes;

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor for a not-found handler
////////////////////////////////////////////////////////////////////////////////

        create_fptr _notFound;
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
