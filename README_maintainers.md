
ArangoDB Maintainers manual
---------------------------
---------------------------
---------------------------
This file contains documentation about the build process, documentation generation means, unittests - put short - if you want to hack parts of arangod this could be interesting for you.

Configure
---------
---------
--enable-relative - relative mode so you can run without make install
--enable-maintainer-mode - generate lex/yacc files
--with-backtrace - add backtraces to native code asserts & exceptions
--enable-failure-tests - adds javascript hook to crash the server for data integrity tests

CFLAGS
------
 -DDEBUG_CLUSTER_COMM - Add backtraces to cluster requests so you can easily track their origin

V8 Special flags:
-DENABLE_GDB_JIT_INTERFACE
(enable (broken) GDB intergation of JIT)
At runtime arangod needs to be started with these options:
--javascript.v8-options="--gdbjit_dump"
--javascript.v8-options="--gdbjit_full"

Debugging the Make process
--------------------------
If the compile goes wrong for no particular reason, appending 'verbose=' adds more output. For some reason V8 has VERBOSE=1 for the same effect.

Runtime
-------
 * start arangod with --console to get a debug console
 * Cheapen startup for valgrind: --no-server --javascript.gc-frequency 1000000 --javascript.gc-interval 65536 --scheduler.threads=1 --javascript.v8-contexts=1
 * to have backtraces output set this on the prompt: ENABLE_NATIVE_BACKTRACES(true)

Startup
-------
We now have a startup rc file: ~/.arangod.rc . Its evaled as javascript.
A sample version to help working with the arangod rescue console may look like that:

    ENABLE_NATIVE_BACKTRACES(true);
    internal = require("internal");
    fs = require("fs");
    db = internal.db;
    time = internal.time;
    timed = function (cb) {
      var s  = time();
      cb();
      return time() - s;
    };
    print = internal.print;


JSLint
------
------
(we switched to jshint a while back - this is still named jslint for historical reasons)

Make target
-----------
use

    make gitjslint

to lint your modified files.

    make jslint

to find out whether all of your files comply to jslint. This is required to make contineous integration work smoothly.

if you want to add new files / patterns to this make target, edit js/Makefile.files

to be safe from commiting non-linted stuff add **.git/hooks/pre-commit** with:

    make gitjslint


Use standalone for your js file
-------------------------------
If you want to search errors in your js file, jslint is very handy - like a compiler is for C/C++.
You can invoke it like this:

    bin/arangosh --jslint js/server/modules/org/arangodb/testing.js


ArangoDB Unittesting Framework
------------------------------
------------------------------
Dependencies
------------
 * Ruby, rspec, httparty


Filename conventions
====================
Special patterns in filenames are used to select tests to be executed or skipped depending on parameters:

-cluster
--------
These tests will only run if clustering is tested. (option 'cluster' needs to be true).

-noncluster
-----------
These tests will only run if no cluster is used. (option 'cluster' needs to be false)

-timecritical
-------------
These tests are critical to execution time - and thus may fail if arangod is to slow. This may happen i.e. if you run the tests in valgrind, so you want to avoid them since they will fail anyways. To skip them, set the option 'skipTimeCritical' to true.

-disabled
---------
These tests are disabled. You may however want to run them by hand.

replication
-----------
(only applies to ruby tests)
These tests aren't run automaticaly since they require a manual set up environment.

-spec
-----
These tests are ran using the jasmine framework instead of jsunity.

Test frameworks used
====================
There are several major places where unittests live: 
 - UnitTests/HttpInterface (rspec tests)
 - UnitTests/Basics (boost unittests)
 - UnitTests/Geo (boost unittests)
 - js/server/tests (runneable on the server)
 - js/common/tests (runneable on the server & via arangosh)
 - js/common/test-data
 - js/client/tests (runneable via arangosh)
 - /js/apps/system/aardvark/test


HttpInterface - RSpec Client Tests
---------------------------------
TODO: which tests are these?


jsUnity on arangod
------------------
you can engage single tests when running arangod with console like this:
require("jsunity").runTest("js/server/tests/aql-queries-simple.js");


jsUnity via arangosh
--------------------
arangosh is similar, however, you can only run tests which are intended to be ran via arangosh:
require("jsunity").runTest("js/client/tests/shell-client.js");



jasmine tests
-------------

Jasmine tests cover two important usecase:
  - testing the UI components of aardvark
-spec

aardvark





UnitTests/Makefile.unittests


Invocation methods
==================



Make-targets
------------
(used in travis CI integration)
Most of the tests can be invoked via the main Makefile:
 - unittests
  - unittests-brief
  - unittests-verbose
 - unittests-recovery
 - unittests-config
 - unittests-boost
 - unittests-single
 - unittests-shell-server
 - unittests-shell-server-only
 - unittests-shell-server-aql
 - unittests-shell-client-readonly
 - unittests-shell-client
 - unittests-http-server
 - unittests-ssl-server
 - unittests-import
 - unittests-replication
  - unittests-replication-server
  - unittests-replication-http
  - unittests-replication-data
 - unittests-upgrade
 - unittests-dfdb
 - unittests-foxx-manager
 - unittests-dump
 - unittests-arangob
 - unittests-authentication
 - unittests-authentication-parameters


Javascript framework
--------------------
(used in Jenkins integration; required for running cluster tests)
Invoked like that:

    scripts/unittest all

calling it without parameters like this:

    scripts/unittest

will give you a extensive usage help which we won't duplicate here.

Choosing facility
_________________

The first parameter chooses the facility to execute.
Available choices include:
 - all: (calls multiple) This target is utilized by most of the jenkins builds invoking unit tests.
 - single_client: (see Running a single unittestsuite)
 
 - single_server: (see Running a single unittestsuite)
 - single_localserver: (see Running a single unittestsuite)
 - many more - call without arguments for more details.
     

Passing Options
_______________

(Old way: Options are passed in as one json; Please note that formating blanks may cause problems.
Different facilities may take different options. The above mentioned usage output contains
the full detail.

A commandline for running a single test (-> with the facility 'single_server') using
valgrind could look like this: 

 scripts/run scripts/unittest.js single_server \
    '{"test":"js/server/tests/aql-escaping.js",'\
    '"extraargs":["--server.threads=1",'\
        '"--scheduler.threads=1",'\
        '"--javascript.gc-frequency","1000000",'\
        '"--javascript.gc-interval","65536"],'\
    '"valgrind":"/usr/bin/valgrind",'\
    '"valgrindargs":["--log-file=/tmp/valgrindlog.%p"]}'
)
Options are passed as regular long values in the syntax --option value --sub:option value.
Using Valgrind could look like this:

    ./scripts/unittest single_server --test js/server/tests/aql-escaping.js \
      --extraargs:server.threads 1 \
      --extraargs:scheduler.threads 1 \
      --extraargs:javascript.gc-frequency 1000000 \
      --extraargs:javascript.gc-interval 65536 \
      --valgrind /usr/bin/valgrind \
      --valgrindargs:log-file /tmp/valgrindlog.%p

 - we specify the test to execute
 - we specify some arangod arguments via --extraargs which increase the server performance
 - we specify to run using valgrind (this is supported by all facilities
 - we specify some valgrind commandline arguments

Running a single unittestsuite
------------------------------
Testing a single test with the frame work directly on a server:

    scripts/unittest single_server --test js/server/tests/aql-escaping.js

Testing a single test with the frame work via arangosh:

    scripts/unittest single_client --test js/server/tests/aql-escaping.js

scripts/unittest is mostly only a wrapper; The backend functionality lives in:
**js/server/modules/org/arangodb/testing.js**


Running foxx tests with a fake foxx Repo
----------------------------------------
Since downloading fox apps from github can be cumbersome with shaky DSL
and DOS'ed github, we can fake it like this:

    export FOXX_BASE_URL="http://germany/fakegit/"
    ./scripts/unittest single_server --test 'js/server/tests/shell-foxx-manager-spec.js'

arangod Emergency console
-------------------------

require("jsunity").runTest("js/server/tests/aql-escaping.js");

arangosh client
---------------

require("jsunity").runTest("js/server/tests/aql-escaping.js");


arangod commandline arguments
-----------------------------

bin/arangod /tmp/dataUT --javascript.unit-tests="js/server/tests/aql-escaping.js" --no-server


make unittest

js/common/modules/loadtestrunner.js

Windows debugging
-----------------
-----------------
For the average *nix user windows debugging has some awkward methods.

Coredump generation
-------------------
Coredumps can be created using the task manager; switch it to detail view, the context menu offers to *create dump file*; the generated file ends in a directory that explorer hides from you - AppData - you have to type that in the location bar. This however only for running processes which is not as usefull as having dumps of crashing processes. While its a common feature to turn on coredumps with the system facilities on *nix systems, its not as easy in windows. You need an external program [from the Sysinternals package: ProcDump](https://technet.microsoft.com/en-us/sysinternals/dd996900.aspx). First look up the PID of arangod, you can finde it in the brackets in its logfile. Then call it like this:

    procdump -accepteula -e -ma < PID of arangod >

It will keep on running and monitor arangod until eventually a crash happenes. You will then get a core dump if an incident occurs or *Dump count not reached.* if nothing happened, *Dump count reached.* if a dump was written - the filename will be printed above.

Debugging symbols
-----------------
Releases are supported by a public symbol server so you will be able to debug cores.
Releases starting with 2.5.6, 2.6.3 onwards are supported.
Either [WinDbg](http://go.microsoft.com/fwlink/p/?linkid=84137) or Visual studio support setting the symbol path
via the environment variable or in the menu. Given we want to store the symbols on *e:\symbol_cach* we add the arangodb symbolserver like this:

    set _NT_SYMBOL_PATH=cache*e:\symbol_cache\cache;srv*e:\symbol_cache\arango*https://www.arangodb.com/repositories/symsrv/;SRV*e:\symbol_cache\ms*http://msdl.microsoft.com/download/symbols

You then will be able to see stack traces.

Documentation
-------------
-------------
Dependencies:
 * swagger
 * gitbook (https://github.com/GitbookIO/gitbook)
 * markdown-pp (https://github.com/triAGENS/markdown-pp)
 * cURL if you want to cut'n'paste execute the examples

Where...
--------
 - js/action/api/* - markdown comments in source with execution section
 - Documentation/Books/Users/SUMMARY.md - index of all sub documentations
 - Documentation/Scripts/generateSwaggerApi.py - list of all sections to be adjusted if

generate
--------
 - make examples - on toplevel to generate Documentation/Examples
 - make examples FILTER_EXAMPLE=geoIndexSelect will only produce one example - *geoIndexSelect*
 - make examples FILTER_EXAMPLE='MOD.*' will only produce the examples matching that regex
 - make examples server.endpoint=tcp://127.0.0.1:8529 will utilize an existing arangod instead of starting anown.
   this does seriously cut down the execution time.
 - make swagger - on toplevel to generate the documentation interactively with the server
 - cd Documentation/Books; make - to generate the HTML documentation

write markdown
--------------
mdpp files are used for the structure. To join it with parts extracted from the programm documentation
you need to place hooks:
  - `@startDocuBlock &ltdocuBlockName&gt;` is replaced by a Docublock extracted from source.
  - `@startDocuBlockInline &lt;docuBlockName&gt;` till `@endDocuBlock &lt;docuBlockName&gt;`
     is replaced in with its own evaluated content - so  *@EXAMPLE_ARANGOSH_OUTPUT* sections are executed
     the same way as inside of sourcecode documentation.

read / use the documentation
----------------------------
 - file:///Documentation/Books/books/Users/index.html contains the generated documentation
 - JS-Console - Tools/API - Interactive documentation which you can play with.

arangod Example tool
--------------------
--------------------
`make example` picks examples from the source code documentation, executes them, and creates a transcript including their results.

To update 
Heres how its details work:
  - all ending with *.cpp*, *.js* and *.mdpp* are searched.
  - all lines inside of sourcecode starting with '///' are matched, all lines in .mdpp files.
  - an example start is marked with *@EXAMPLE_ARANGOSH_OUTPUT* or *@EXAMPLE_ARANGOSH_RUN*
  - the example is named by the string provided in brackets after the above key
  - the output is written to *Documentation/Examples/<name>.generated*
  - examples end with *@END_EXAMPLE_[OUTPUT|RUN]*
  - all code inbetween is executed as javascript in the **arangosh** while talking to a valid **arangod**.

OUTPUT and RUN Specialities
---------------------------
By default, Examples should be self contained and thus not depend on each other. They should clean up the collections they create.
Building will fail if resources aren't cleaned.
However, if you intend a set of OUTPUT and RUN to demonstrate interactively, you have to use an alphabeticaly sorteable
namingscheme so they're executed in sequence. Using <modulename>_<sequencenumber>[abcd]_thisDoesThat seems to be a good scheme.

  - OUTPUT is intended to create samples that the users can cut'n'paste into their arangosh. Its used for javascript api documentation.
    * wrapped lines:
      Lines starting with a pipe ('/// |') are joined together with the next following line.
      You have to do this, if you want to execute loops, functions or commands which shouldn't be torn appart by the framework.
    * Lines starting with *var*:
      The command behaves similar to the arangosh: the server reply won't be printed.
      Hovever, the variable will be in the scope of the other lines - else it won't.
    * Lines starting with a Tilde ('/// ~'):
      These lines can be used for setup/teardown purposes. They are completely invisible in the generated example transscript.

    * ~removeIgnoreCollection("test") - the collection test may live across several tests.
    * ~addIgnoreCollection("test") - the collection test will again be alarmed if left over.

  - it is excuted line by line. If a line is intended to fail (aka throw an exception),
    you have to specify *// xpError(ERROR_ARANGO_DOCUMENT_KEY_UNEXPECTED)* so the exception will be caught;
    else the example is marked as broken.
    If you need to wrap that line, you may want to make the next line like this to suppress an empty line:
    
        /// | someLongStatement()
        /// ~ // xpError(ERROR_ARANGO_DOCUMENT_KEY_UNEXPECTED)
    
 - RUN is intended to be pasted into a unix shell with *cURL* to demonstrate how the REST-HTTP-APIs work.
   The whole chunk of code is executet at once, and is expected **not to throw**.
   You should use **assert(condition)** to ensure the result is what you've expected.

    * Send the HTTP-request: `var response = logCurlRequest('GET', url);`
    * check its response:    `assert(response.code === 200);`
    * output a JSON server Reply: `logJsonResponse(response);`
    * output HTML or text to dump to the user: `logRawResponse(response);`
    * dump the reply to the errorlog for testing (will mark run as failed): `logErrorResponse(response);`


