////////////////////////////////////////////////////////////////////////////////
/// @brief query execution
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2012 triagens GmbH, Cologne, Germany
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
/// @author Jan Steemann 
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include <BasicsC/logging.h>

#include "VocBase/query-execute.h"
#include "VocBase/query-locks.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief apply skip and limit for a query result
////////////////////////////////////////////////////////////////////////////////

static bool TransformDataSkipLimit (TRI_rc_result_t* result,
                                    TRI_voc_size_t skip,
                                    TRI_voc_ssize_t limit) {

  TRI_select_result_t* _result = result->_selectResult;
   
  // return nothing
  if (limit == 0 || _result->_numRows <= skip) {
    return TRI_SliceSelectResult(_result, 0, 0);
  }

  // positive limit, skip and slice from front
  if (limit > 0) {
    if (_result->_numRows - skip < limit) {
      return TRI_SliceSelectResult(_result, skip, _result->_numRows - skip);
    }

    return TRI_SliceSelectResult(_result, skip, limit);
  }

  // negative limit, slice from back
  if (_result->_numRows - skip < -limit) {
    return TRI_SliceSelectResult(_result, 0, _result->_numRows - skip);
  }

  return TRI_SliceSelectResult(_result, _result->_numRows - skip + limit, -limit);
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup VocBase
/// @{
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief executes a query
////////////////////////////////////////////////////////////////////////////////

TRI_query_cursor_t* TRI_ExecuteQueryInstance (TRI_query_instance_t* const instance,
                                              const bool doCount,
                                              const uint32_t max) {
  TRI_select_result_t* selectResult;
  TRI_query_cursor_t* cursor;
  TRI_voc_ssize_t noLimit = (TRI_voc_ssize_t) INT32_MAX;
  
  // create a select result container for the joins
  selectResult = TRI_JoinSelectResult(instance);
  if (!selectResult) {
    TRI_RegisterErrorQueryInstance(instance, TRI_ERROR_OUT_OF_MEMORY, NULL);
    return NULL;
  }

  cursor = TRI_CreateQueryCursor(instance, selectResult, doCount, max);
  if (!cursor) {
    selectResult->free(selectResult);
    return NULL;
  }

  if (!instance->_query._isEmpty) {
    TRI_voc_size_t skip;
    TRI_voc_ssize_t limit;
    bool applyPostSkipLimit;

    skip = instance->_query._limit._offset;
    limit = instance->_query._limit._count;
    applyPostSkipLimit = false;

    if (limit < 0) {
      limit = noLimit;
      skip = 0;
      applyPostSkipLimit = true;
    }

    if (instance->_query._order._type == QLQueryOrderTypeMustEvaluate && 
        (instance->_query._limit._offset > 0 || 
         instance->_query._limit._count != noLimit)) {
      // query has an order by. we must postpone limit to after sorting
      limit = noLimit;
      skip = 0;
      applyPostSkipLimit = true;
    }

    TRI_ReadLockCollectionsQueryInstance(instance);
    if (!TRI_AddCollectionsBarrierQueryInstance(instance, cursor)) {
      selectResult->free(selectResult);
      cursor->free(cursor);
      return NULL;
    }

    // Execute joins
    TRI_ExecuteJoins(instance, selectResult, skip, limit);

    TRI_ReadUnlockCollectionsQueryInstance(instance);

    // order by
    if (instance->_query._order._type == QLQueryOrderTypeMustEvaluate) {
      cursor->_result._orderContext = TRI_CreateExecutionContext(instance->_query._order._functionCode);
      if (cursor->_result._orderContext) {
        LOG_DEBUG("performing order by");
        TRI_OrderDataQuery(&cursor->_result);
        TRI_FreeExecutionContext(cursor->_result._orderContext);
      }
    }

    // apply a negative limit or a limit after ordering
    if (applyPostSkipLimit) {
      LOG_DEBUG("applying post-order skip/limit");
      TransformDataSkipLimit(&cursor->_result, 
                             instance->_query._limit._offset, 
                             instance->_query._limit._count);
    }
  }
      
  if (instance->_doAbort) {
    selectResult->free(selectResult);
    cursor->_result._selectResult = NULL;
    cursor->free(cursor);
    return NULL;
  }
  
  // adjust cursor length
  cursor->_length = selectResult->_numRows; 
  cursor->_currentRow = 0;

  if (cursor->_length > 0 && !instance->_query._select._isConstant) {
    // we have a result set. the cursor now becomes responsible
    // for freeing any locks we still have on the underlying collections
    TRI_HandoverLocksQueryInstance(instance, cursor);
  }

  return cursor;
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
