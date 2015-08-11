'use strict';

////////////////////////////////////////////////////////////////////////////////
/// @brief Foxx Sessions
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2013 triagens GmbH, Cologne, Germany
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
/// @author Jan Steemann, Lucas Dohmen
/// @author Copyright 2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

const joi = require('joi');
const Foxx = require('org/arangodb/foxx');
const paramSchema = joi.string().optional().description('Foxx session ID');

// -----------------------------------------------------------------------------
// --SECTION--                                                  helper functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief decorates the controller with session logic
////////////////////////////////////////////////////////////////////////////////

function decorateController(auth, controller) {
  var cfg = auth.configuration;

  if (cfg.param) {
    controller.allRoutes.queryParam(cfg.param, paramSchema);
  }

  controller.before('/*', function (req) {
    var sessions = auth.getSessionStorage();
    var sid;
    if (cfg.param) {
      sid = req.params(cfg.param);
    }
    if (!sid && cfg.cookie) {
      sid = req.cookie(cfg.cookie.name, cfg.cookie.secret ? {
        signed: {
          secret: cfg.cookie.secret,
          algorithm: cfg.cookie.algorithm
        }
      } : undefined);
    }
    if (!sid && cfg.header) {
      sid = req.headers[cfg.header.toLowerCase()];
    }
    if (sid) {
      try {
        req.session = sessions.get(sid);
      } catch (e) {
        if (!(e instanceof sessions.errors.SessionNotFound)) {
          throw e;
        }
      }
    }
    if (!req.session && cfg.autoCreateSession) {
      req.session = sessions.create();
    }
  });

  controller.after('/*', function (req, res) {
    if (req.session) {
      var sid = req.session.forClient();
      if (cfg.cookie) {
        res.cookie(cfg.cookie.name, sid, {
          ttl: req.session.getTTL() / 1000,
          signed: cfg.cookie.secret ? {
            secret: cfg.cookie.secret,
            algorithm: cfg.cookie.algorithm
          } : undefined
        });
      }
      if (cfg.header) {
        res.set(cfg.header, sid);
      }
    }
  });
}

////////////////////////////////////////////////////////////////////////////////
/// @brief convenience wrapper for session destruction logic
////////////////////////////////////////////////////////////////////////////////

function createDestroySessionHandler(auth, opts) {
  if (!opts) {
    opts = {};
  }
  if (typeof opts === 'function') {
    opts = {after: opts};
  }
  if (!opts.after) {
    opts.after = function (req, res) {
      res.json({message: 'logged out'});
    };
  }
  var cfg = auth.configuration;
  return function (req, res, injected) {
    if (typeof opts.before === 'function') {
      opts.before(req, res, injected);
    }
    if (req.session && typeof req.session.delete === 'function') {
      req.session.delete();
    }
    if (cfg.autoCreateSession) {
      req.session = auth.getSessionStorage().create();
    } else {
      if (cfg.cookie) {
        res.cookie(cfg.cookie.name, '', {
          ttl: -(7 * 24 * 60 * 60),
          sign: cfg.cookie.secret ? {
            secret: cfg.cookie.secret,
            algorithm: cfg.cookie.algorithm
          } : undefined
        });
      }
      delete req.session;
    }
    if (typeof opts.after === 'function') {
      opts.after(req, res, injected);
    }
  };
}

// -----------------------------------------------------------------------------
// --SECTION--                                                     FOXX SESSIONS
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

var sessionTypes = ['cookie', 'header'];

class Sessions {

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

  constructor(opts) {
    if (!opts) {
      opts = {};
    }

    if (opts.cookie) {
      if (opts.cookie === true) {
        opts.cookie = {};
      } else if (typeof opts.cookie === 'string') {
        opts.cookie = {name: opts.cookie};
      } else if (typeof opts.cookie !== 'object') {
        throw new Error('Expected cookie settings to be an object, boolean or string.');
      }
      if (!opts.cookie.name) {
        opts.cookie.name = 'sid';
      } else if (typeof opts.cookie.name !== 'string') {
        throw new Error('Cookie name must be a string or empty.');
      }
      if (opts.cookie.secret && typeof opts.cookie.secret !== 'string') {
        throw new Error('Cookie secret must be a string or empty.');
      }
    }
    if (opts.header) {
      if (opts.header === true) {
        opts.header = 'X-Session-Id';
      } else if (typeof opts.header !== 'string') {
        throw new Error('Header name must be true, a string or empty.');
      }
    }
    if (opts.param) {
      if (opts.param === true) {
        opts.param = 'FOXXSID';
      } else if (typeof opts.param !== 'string') {
        throw new Error('Param name must be true, a string or empty.');
      }
    }
    if (opts.autoCreateSession !== false) {
      opts.autoCreateSession = true;
    }
    if (!opts.sessionStorage) {
      opts.sessionStorage = '/_system/sessions';
    }
    this.configuration = opts;
  }

// -----------------------------------------------------------------------------
// --SECTION--                                                  public functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief fetches the session storage
////////////////////////////////////////////////////////////////////////////////

  getSessionStorage() {
    if (typeof this.configuration.sessionStorage !== 'string') {
      return this.configuration.sessionStorage;
    }
    return Foxx.requireApp(this.configuration.sessionStorage).sessionStorage;
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                    module exports
// -----------------------------------------------------------------------------

exports.sessionTypes = sessionTypes;
exports.Sessions = Sessions;
exports.decorateController = decorateController;
exports.createDestroySessionHandler = createDestroySessionHandler;

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

/// Local Variables:
/// mode: outline-minor
/// outline-regexp: "/// @brief\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}\\|/\\*jslint"
/// End:
