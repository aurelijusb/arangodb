////////////////////////////////////////////////////////////////////////////////
/// @brief a basis class for concrete implementations for a shell
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
/// @author Esteban Lombeyda
/// @author Copyright 2014, ArangoDB GmbH, Cologne, Germany
/// @author Copyright 2011-2014, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_UTILITIES_READLINE_SHELL_H
#define ARANGODB_UTILITIES_READLINE_SHELL_H 1

#include "Basics/Common.h"

#include "ShellImplementation.h"
#include "Completer.h"

namespace triagens {

  class Completer;

  class ReadlineShell : public ShellImplementation {

    public:

////////////////////////////////////////////////////////////////////////////////
///                                                           public constructor
////////////////////////////////////////////////////////////////////////////////

      ReadlineShell (std::string const& history, Completer*);

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor open
////////////////////////////////////////////////////////////////////////////////

      bool open (bool autoComplete) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief line editor shutdown
////////////////////////////////////////////////////////////////////////////////

      bool close () override;

////////////////////////////////////////////////////////////////////////////////
/// @brief get the history file path
///
/// The path is "$HOME" plus _historyFilename, if $HOME is set. Else
/// the local file _historyFilename.
////////////////////////////////////////////////////////////////////////////////

      std::string historyPath () override;

////////////////////////////////////////////////////////////////////////////////
/// @brief add to history
////////////////////////////////////////////////////////////////////////////////

      void addHistory (char const*) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief save the history
////////////////////////////////////////////////////////////////////////////////

      bool writeHistory () override;

////////////////////////////////////////////////////////////////////////////////
/// @brief get the entered line
////////////////////////////////////////////////////////////////////////////////

      char* getLine (char const *) override;

////////////////////////////////////////////////////////////////////////////////
/// @brief prefill the input
////////////////////////////////////////////////////////////////////////////////

      void prefill (char const*) override;

  };

}

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
