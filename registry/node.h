/*
 *   See file ../COPYRIGHT for copying and redistribution conditions.
 *
 *   This header-file specifies the API for registry nodes.
 *   The methods of this module are thread-compatible but not thread-safe.
 */

#ifndef LDM_NODE_H
#define LDM_NODE_H

typedef struct regNode          RegNode;
typedef struct valueThing       ValueThing;
typedef RegStatus               (*NodeFunc)(RegNode* node);
typedef RegStatus               (*ValueFunc)(ValueThing* vt);

#include <search.h>
#include "backend.h"
#include "registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allocates a new, empty node suitable for the root of a tree of nodes.
 *
 * Arguments:
 *      node            Pointer to a pointer to the newly-allocated node.  Set
 *                      upon successful return.  Shall not be NULL.
 * Returns:
 *      0               Success
 *      ENOMEM          System error.  "log_start()" called.
 */
RegStatus rn_newRoot(
    RegNode** const     node);

/*
 * Returns the absolute path name of a node.
 *
 * Arguments:
 *      node            Pointer to the node.  Shall not be NULL.
 * Returns:
 *      Pointer to the absolute path name of the node.  The client shall not
 *      free.
 */
const char* rn_getAbsPath(
    const RegNode* const        node);

/*
 * Returns the parent node of a node.
 *
 * Arguments:
 *      node            Pointer to the node whose parent node is to be returned.
 *                      Shall not be NULL.
 * Returns:
 *      NULL            The node has no parent node (i.e., the node is a
 *                      root-node)
 *      else            Pointer to the parent node
 */
RegNode* rn_getParent(
    const RegNode* const        node);

/*
 * Returns the name of a node.
 *
 * Arguments:
 *      node            Pointer to the node whose name will be returned.  Shall
 *                      not be NULL.
 * Returns:
 *      Pointer to the name of the node.  The client shall not free.
 */
const char* rn_getName(
    const RegNode* const        node);

/*
 * Indicates if a node has been deleted.
 *
 * Arguments:
 *      node            Pointer to the node.  Shall not be NULL.
 * Returns:
 *      0               The node has not been deleted.
 *      else            The node has been deleted.
 */
int rn_isDeleted(
    const RegNode* const        node);

/*
 * Puts a value into a node.  If a ValueThing had to be created, then its
 * status is zero; otherwise, its status is unchanged.
 *
 * Arguments:
 *      node            Pointer to the node.  Shall not be NULL.
 *      name            Pointer to the name of the value.  Shall not be NULL.
 *      value           Pointer to the value.  Shall not be NULL.  The client
 *                      may free upon return.
 *      vt              Pointer to a pointer to the corresponding ValueThing.
 *                      May be NULL.  Set upon successful return if not NULL.
 * Returns:
 *      0               Success.  "*vt" is set if "vt" isn't NULL.
 *      ENOMEM          System error.  "log_start()" called.
 *      EPERM           The node has been deleted.  "log_start()" called.
 *      EEXIST          The value would have the same absolute path name as an
 *                      existing node.  "log_start()" called.
 */
RegStatus rn_putValue(
    RegNode* const      node,
    const char* const   name,
    const char* const   value,
    ValueThing** const  vt);

/*
 * Returns a value given a starting node and a relative path name.
 *
 * Arguments:
 *      node            Pointer to the node at which to start the search for the
 *                      value.  Shall not be NULL.
 *      path            Pointer to the path name of the value to be returned
 *                      relative to the starting node.  Shall not be NULL.
 *      string          Pointer to a pointer to the string representation of
 *                      the value.  Shall not be NULL.  Set upon successful
 *                      return.  The client should call "free(*string)" when
 *                      the string is no longer needed.
 * Returns:
 *      0               Success.  "*string" is not NULL.
 *      EPERM           The node containing the value has been deleted.
 *                      "log_start()" called.
 *      ENOENT          No such value.  "log_start()" called.
 *      ENOMEM          System error.  "log_start()" called.
 */
RegStatus rn_getValue(
    const RegNode* const        node,
    const char* const           path,
    char** const                string);

/*
 * Finds a node given a starting-node and a relative path name.
 *
 * Arguments:
 *      root            Pointer to the node at which to start the search.  Shall
 *                      not be NULL.
 *      path            Pointer to the path name of the desired node relative
 *                      to the starting-node.  Shall not be NULL.
 *      node            Pointer to a pointer to the desired node.  May be NULL.
 *                      Set upon successful return if not NULL.
 * Returns:
 *      0               Success.  "*node" is set if "node" is not NULL.
 *      ENOENT          No such node exists
 */
RegStatus rn_find(
    RegNode* const      root,
    const char* const   path,
    RegNode** const     node);

/*
 * Deletes a value from a node.  The value is only marked as deleted: it is
 * neither removed from the node nor are its resources freed.
 *
 * Arguments:
 *      node            Pointer to the node.  Shall not be NULL.
 *      name            Pointer to the name of the value.  Shall not be NULL.
 * Returns:
 *      0               Success
 *      ENOENT          The node has no such value
 *      ENOMEM          System error.  "log_start()" called.
 *      EPERM           The node has been deleted.  "log_start()" called.
 */
RegStatus rn_deleteValue(
    RegNode* const      node,
    const char* const   name);

/*
 * Ensures that a node in a node-tree exists.  Creates it and any missing
 * ancestors if it doesn't exist.
 *
 * Arguments:
 *      root            Pointer to the node at which to start the search.
 *                      Shall not be NULL.
 *      path            Pointer to the path name of the node relative to
 *                      the starting-node.  Shall not be NULL.
 *      node            Pointer to a pointer to the node.  Shall not be NULL.
 *                      Set upon successful return.
 * Returns:
 *      0               Success.  "*node" is not NULL.
 *      EEXIST          A node would have to be created with the same absolute
 *                      path name as an existing value.  "log_start()" called.
 *      ENOMEM          System error.  "log_start()" called.
 */
RegStatus rn_ensure(
    RegNode* const      root,
    const char* const   path,
    RegNode** const     node);

/*
 * Frees a node and all its descendents.
 *
 * Arguments:
 *      node            Pointer to the node to be freed along with all its
 *                      descendents.  May be NULL.
 */
void rn_free(
    RegNode* const      node);

/*
 * Finds the node closest to a desired node that is not a descendent of the
 * desired node.
 *
 * Arguments:
 *      root            Pointer to the node at which to start the search.
 *                      Shall not be NULL.
 *      initPath        Pointer to the path name of the desired node relative
 *                      to the starting-node.  Shall not be NULL.
 *      node            Pointer to a pointer to the node closest to the desired
 *                      node that is not a descendent of the desired node.
 *                      Shall not be NULL.  Set upon successful return.  Can
 *                      point to the desired node.
 *      remPath         Pointer to a pointer to the path name of the desired
 *                      node relative to the returned node.  The client
 *                      should call "free(*remPath)" when this path name is no
 *                      longer needed.
 * Returns:
 *      0               Success.  "*node" is set.
 *      ENOMEM          System error.  "log_start()" called.
 */
RegStatus rn_getLastNode(
    RegNode* const              root,
    const char* const           initPath,
    RegNode** const             node,
    char** const                remPath);

/*
 * Visits a node and all its descendents in the natural order of their path
 * names.
 *
 * Arguments:
 *      node            Pointer to the node at which to start.  Shall not be
 *                      NULL.
 *      func            Pointer to the function to call at each node.  Shall
 *                      not be NULL.
 * Returns:
 *      0               Success
 *      else            The first non-zero value returned by "func".
 */
RegStatus rn_visitNodes(
    RegNode* const      node,
    const NodeFunc      func);

/*
 * Visits all the values of a node in the natural order of their names.
 *
 * Arguments:
 *      node            Pointer to the node whose values are to be visited.
 *                      Shall not be NULL.
 *      extant          Pointer to the function to call for each extant value.
 *                      Shall not be NULL.
 *      deleted         Pointer to the function to call for each deleted value.
 *                      May be NULL.
 * Returns:
 *      0               Success
 *      ENOMEM          System error.  "log_start()" called.
 *      else            The first non-zero value returned by "extant" or
 *                      "deleted".
 */
RegStatus rn_visitValues(
    RegNode* const      node,
    const ValueFunc     extant,
    const ValueFunc     deleted);

/*
 * Frees the deleted values of a node.  This actually removes the values from
 * the node and frees their resources.
 *
 * Arguments:
 *      node            Pointer to the node.  Shall not be NULL.
 */
void rn_freeDeletedValues(
    RegNode* const      node);

/*
 * Marks a node and all its descendents as being deleted.  Deleted nodes are
 * only marked as such: they are not actually removed from the node-tree nor
 * are their resources freed.
 *
 * Arguments:
 *      node            Pointer to the node to be deleted along with all its
 *                      descendents.  Shall not be NULL.
 */
void rn_delete(
    RegNode* const      node);

/*
 * Clears a node.  The values of the node are freed and all descendents of
 * the node are freed.
 *
 * Arguments:
 *      node            Pointer to the node to be cleared.  Shall not be NULL.
 */
void rn_clear(
    RegNode* const      node);

/*
 * Sets the status of a ValueThing.  Returns the previous status.
 *
 * Arguments:
 *      vt              Pointer to the ValueThing to have its status set.
 *                      Shall not be NULL.
 *      status          The status for the ValueThing.
 * Returns:
 *      The previous value of the status.
 */
int vt_setStatus(
    ValueThing* const   vt,
    const int           status);

/*
 * Returns the status of a ValueThing.
 *
 * Arguments:
 *      vt              Pointer to the ValueThing.  Shall not be NULL.
 * Returns:
 *      The status of the ValueThing.
 */
int vt_getStatus(
    const ValueThing* const   vt);

/*
 * Returns the name of a ValueThing.
 *
 * Arguments:
 *      vt              Pointer to the ValueThing.  Shall not be NULL.
 * Returns:
 *      Pointer to the name of the ValueThing.  The Client shall not free.
 */
const char* vt_getName(
    const ValueThing* const   vt);

/*
 * Returns the string-value of a ValueThing.
 *
 * Arguments:
 *      vt              Pointer to the ValueThing.  Shall not be NULL.
 * Returns:
 *      Pointer to the string-value of the ValueThing.  The Client shall not
 *      free.
 */
const char* vt_getValue(
    const ValueThing* const     vt);

#ifdef __cplusplus
}
#endif

#endif