/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Tue Sep 09 2003
 copyright   : (C) 2003 by Martin Preuss
 email       : martin@libchipcard.de

 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 *
 *   MA  02111-1307  USA                                                   *
 *                                                                         *
 ***************************************************************************/


/** @file gwenhywfar/db.h */

#ifndef GWENHYWFAR_DB_H
#define GWENHYWFAR_DB_H

#include <gwenhywfar/gwenhywfarapi.h>
#include <gwenhywfar/path.h>
#include <gwenhywfar/bufferedio.h>
#include <gwenhywfar/types.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MOD_DB Database
 * @ingroup MOD_PARSER
 *
 * @brief This group contains the definition of a GWEN_DB database.
 *
 * A GWEN_DB database consists of a tree of @ref GWEN_DB_NODE entries.
 * Such a @ref GWEN_DB_NODE node can either be a group node, or a
 * variable node, or a value node. Usually an application programmer
 * will only get in touch with group nodes. The application programmer
 * can iterate through the group nodes with GWEN_DB_GetGroup(),
 * GWEN_DB_Groups_foreach(), and can retrieve variable values with
 * GWEN_DB_GetIntValue(), GWEN_DB_GetCharValue() and so on.
 *
 * <p>
 *  The following graph shows the internal structure of a GWEN_DB: <br>
 *  @image html db2.png "Internal structure of a GWEN_DB"
 * <br>
 * As you can see the GWEN_DB consists of multiple units called NODE. Every
 * node has a pointer to:
 * <ul>
 * <li>its parent</li>
 * <li>its first child (only the <strong>first</strong>)</li>
 * <li>its successor (but not its predecessor!)
 * </ul>
 * Such a node can be either of the following:
 * <ul>
 *  <li>a group containing other groups and variables</li>
 *  <li>a variable containing values (and values only)</li>
 *  <li>a value containing its value data</li>
 * </ul>
 * </p>
 *
 * Each group has a name. Depending on the GWEN_DB_FLAGS given when
 * reading a GWEN_DB from a file or creating it, it may very well be
 * possible to have multiple groups with the same name as children of
 * the same root node. Again: Child group nodes with the same name may
 * very well exist. This also applies to variable nodes.
 *
 * For the interested reader, we again explain the difference of the
 * three kinds of nodes. Depending on either of these cases, you can
 *
 * <ol>
 * <li> Iterate through groups or get a variable with
 *  e.g. GWEN_DB_GetNextGroup(), GWEN_DB_GetNextVar() 
 * <li> Get the type of a variable with e.g. GWEN_DB_GetVariableType() 
 *  -- the value of a variable is retrieved by the shortcut functions 
 *  explained below.
 * <li> Get the type of a value with GWEN_DB_GetValueType(). Again the
 * value itself is retrieved with the shortcut functions below.
 * </ol>
 * 
 * To retrieve or set the value of such a variable, the following
 * "shortcut" functions for all three supported typed exist:
 * GWEN_DB_GetIntValue(), GWEN_DB_GetCharValue(),
 * GWEN_DB_GetBinValue(). These functions only accept a group  and a "path"
 * to the desired variable.
 *
 */
/*@{*/
/** maximum size of a text line when reading/writing from/to streams */
#define GWEN_DB_LINE_MAXSIZE  1024

/** @name DB Flags
 *
 * <p>
 * Please note that the setter functions also take the flags from
 * the module @ref MOD_PATH (e.g. @ref GWEN_PATH_FLAGS_PATHMUSTEXIST)
 * into account. So you most likely need to specify
 * them, too.
 * </p>
 * <p>
 * However, for your conveniance there is a default flag value which suffices
 * in most cases (@ref GWEN_DB_FLAGS_DEFAULT).
 * </p>
 */
/*@{*/
/** Overwrite existing values when assigning a new value to a variable */
#define GWEN_DB_FLAGS_OVERWRITE_VARS         0x00010000
/** Overwrite existing groups when calling @ref GWEN_DB_GetGroup() */
#define GWEN_DB_FLAGS_OVERWRITE_GROUPS       0x00020000
/** quote variable names when writing them to a stream */
#define GWEN_DB_FLAGS_QUOTE_VARNAMES         0x00040000
/** quote values when writing them to a stream */
#define GWEN_DB_FLAGS_QUOTE_VALUES           0x00080000
/** allows writing of subgroups when writing to a stream */
#define GWEN_DB_FLAGS_WRITE_SUBGROUPS        0x00100000
/** adds some comments when writing to a stream */
#define GWEN_DB_FLAGS_DETAILED_GROUPS        0x00200000
/** indents text according to the current path depth when writing to a
 * stream to improve the readability of the created file */
#define GWEN_DB_FLAGS_INDEND                 0x00400000
/** writes a newline to the stream after writing a group to improve
 * the readability of the created file */
#define GWEN_DB_FLAGS_ADD_GROUP_NEWLINES     0x00800000
/** uses a colon (":") instead of an equation mark when reading/writing
 * variable definitions */
#define GWEN_DB_FLAGS_USE_COLON              0x01000000
/** stops reading from a stream at empty lines */
#define GWEN_DB_FLAGS_STOP_ON_EMPTY_LINE     0x02000000
/** normally the type of a variable is written to the stream, too.
 * This flag changes this behaviour */
#define GWEN_DB_FLAGS_OMIT_TYPES             0x04000000
/** appends data to an existing file instead of overwriting it */
#define GWEN_DB_FLAGS_APPEND_FILE            0x08000000

/** These are the default flags which you use in most cases */
#define GWEN_DB_FLAGS_DEFAULT \
  GWEN_DB_FLAGS_QUOTE_VALUES | \
  GWEN_DB_FLAGS_WRITE_SUBGROUPS | \
  GWEN_DB_FLAGS_DETAILED_GROUPS | \
  GWEN_DB_FLAGS_INDEND | \
  GWEN_DB_FLAGS_ADD_GROUP_NEWLINES


/** same like @ref GWEN_DB_FLAGS_DEFAULT except that the produced file
 * (when writing to a stream) is more compact (but less readable)*/
#define GWEN_DB_FLAGS_COMPACT \
  GWEN_DB_FLAGS_QUOTE_VALUES | \
  GWEN_DB_FLAGS_WRITE_SUBGROUPS

/** These flags can be used to read a DB from a HTTP header. It uses a
 * colon instead of the equation mark with variable definitions and stops
 * when encountering an empty line.*/
#define GWEN_DB_FLAGS_HTTP \
  GWEN_DB_FLAGS_USE_COLON |\
  GWEN_DB_FLAGS_STOP_ON_EMPTY_LINE
/*@}*/


/**
 * This is the type used to store a DB. Its contents are explicitly NOT
 * part of the API. 
 *
 * A description of what can be done with this type can be found in
 * @ref db.h
 */
GWENHYWFAR_API
typedef union GWEN_DB_NODE GWEN_DB_NODE;

/**
 * This specifies the type of a value stored in the DB.
 */
GWENHYWFAR_API
typedef enum {
  /** type unknown */
  GWEN_DB_VALUETYPE_UNKNOWN=0,
  /** simple, null terminated C-string */
  GWEN_DB_VALUETYPE_CHAR,
  /** integer */
  GWEN_DB_VALUETYPE_INT,
  /** binary, user defined data */
  GWEN_DB_VALUETYPE_BIN
} GWEN_DB_VALUETYPE;



/** @name Constructing, Destructing, Copying
 *
 */
/*@{*/

/**
 * Creates a new (empty) group with the given name. I.e. this is the
 * constructor.  When finished using this group, you should free it
 * using @ref GWEN_DB_Group_free() in order to avoid memory leaks.
 * @param name name of the group to create
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_Group_new(const char *name);

/**
 * Frees a DB group. I.e. this is the destructor. This is needed to
 * avoid memory leaks.
 * @param n db node
 */
GWENHYWFAR_API 
void GWEN_DB_Group_free(GWEN_DB_NODE *n);

/**
 * Creates a deep copy of the given node. This copy will then be owned
 * by the caller and MUST be freed after using it by calling @ref
 * GWEN_DB_Group_free().
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_Group_dup(const GWEN_DB_NODE *n);

/*@}*/



/** @name Iterating Through Groups 
 *
 */
/*@{*/
/**
 * Returns the first group node below the given one.
 *
 * If there is no group node then NULL is returned. This can either
 * mean that this node does not have any children or the only
 * children are variables instead of groups.
 *
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetFirstGroup(GWEN_DB_NODE *n);

/**
 * Returns the next group node following the given one, which has the
 * same parent node.
 *
 * This function works absolutely independently of the group nodes'
 * names -- the returned node may or may not have the same name as the
 * specified node. The only guarantee is that the returned node will
 * be a group node.
 *
 * If there is no group node then NULL is returned. This can either
 * mean that the parent node does not have any further
 * children, or that the other children are variables instead
 * of groups.
 *
 * @note This is the only function where the returned node is @e not
 * the child of the specified node, but instead it is the next node
 * with the same parent node. In other words, this function is an
 * exception. In all other functions the returned node is a child of
 * the specified node.
 *
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetNextGroup(GWEN_DB_NODE *n);

/** Callback function type for GWEN_DB_Groups_Foreach(),
 * GWEN_DB_Variables_Foreach(), and GWEN_DB_Values_Foreach().
 *
 * @param node The current node element 
 *
 * @param user_data The arbitrary data passed to the foreach function.
 *
 * @return NULL if the iteration should continue, or non-NULL if the
 * iteration should stop and that value be returned.
 */
typedef void *(*GWEN_DB_NODES_CB)(GWEN_DB_NODE *node, void *user_data);

/** Iterates through all group nodes that are @e direct children
 * of the given node, calling the callback function 'func' on each
 * group node.  Traversal will stop when 'func' returns a non-NULL
 * value, and the routine will return with that value. Otherwise the
 * routine will return NULL.
 *
 * If no nodes that are groups are found as children, then
 * this function will simply do nothing.
 *
 * @param node The group node whose children shall be iterated through.
 * @param func The function to be called with each group node.
 * @param user_data A pointer passed on to the function 'func'.
 * @return The non-NULL pointer returned by 'func' as soon as it
 * returns one. Otherwise (i.e. 'func' always returns NULL)
 * returns NULL.
 * @author Christian Stimming <stimming@tuhh.de> */
GWENHYWFAR_API 
void *GWEN_DB_Groups_Foreach(GWEN_DB_NODE *node, GWEN_DB_NODES_CB func,
			     void *user_data);

/** Returns the number of group nodes that are @e direct children of
 * the given node. In other words, this is the number of group nodes
 * that will be reached in the GWEN_DB_Groups_foreach() function. */
GWENHYWFAR_API 
unsigned int GWEN_DB_Groups_Count(const GWEN_DB_NODE *node);
/*@}*/




/** @name Variable Getter and Setter
 *
 * These getter functions check for the existence of the given variable and
 * return the value specified by an index.
 * Under the following circumstances the also given default value will be
 * returned:
 * <ul>
 *  <li>the variable does not exist</li>
 *  <li>the variable exists but has no values (should not occur)</li>
 *  <li>the variable exists but the given value index is out of range (e.g.
 *  specifying index 1 with a variable that has only one value)</li>
 *  <li>a string value is expected but the variable is not of that type.
 *  However, if you want an integer value but the variable only has a char
 *  value then the getter functions try to convert the char to an integer.
 *  Other conversions do not take place.</li>
 * </ul>
 *
 * The setter functions either replace an existing variable, create a missing
 * variable, add a value or return an error if the variable does not exist
 * (see description of the flags).
 * All setter functions make deep copies of the given values, so you may
 * free the params after calling the setter function.
 *
 * All getter functions return a const pointer to the variable's retrieved
 * value.
 * All setter functions return Zero if ok and Nonzero on error.
 *
 * This module knows about the following types (see @ref GWEN_DB_VALUETYPE):
 * <ul>
 *  <li>char (simple null terminated C strings)</li>
 *  <li>int (integer values)</li>
 *  <li>bin (binary, user specified data)</li>
 * </ul>
 *
 * @note The value returned by a getter function is only valid as long as the
 * corresponding variable (node) exists.<br>
 * So if you retrieve the value of a variable and delete the variable (or even
 * the whole DB) afterwards the pointer becomes invalid and using it will most
 * likely crash your program.<br>
 * If you want to use such a value even after the corresponding variable
 * has been deleted you need to make a copy.
 *

 */
/*@{*/
/**
 * Returns the variable's retrieved value.
 * @param n db node
 * @param path path and name of the variable
 * @param idx index number of the value to return
 * @param defVal default value to return in case there is no real value
 */
GWENHYWFAR_API 
const char *GWEN_DB_GetCharValue(GWEN_DB_NODE *n,
                                 const char *path,
                                 int idx,
                                 const char *defVal);
/**
 * @return 0 on success, nonzero on error
 * @param n db node
 * @param flags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 * @param path path and name of the variable
 * @param val The string value that is copied into the DB
 */
GWENHYWFAR_API 
int GWEN_DB_SetCharValue(GWEN_DB_NODE *n,
                         GWEN_TYPE_UINT32 flags,
                         const char *path,
                         const char *val);


/**
 * Returns the variable's retrieved value.
 * @param n db node
 * @param path path and name of the variable
 * @param idx index number of the value to return
 * @param defVal default value to return in case there is no real value
 */
GWENHYWFAR_API 
int GWEN_DB_GetIntValue(GWEN_DB_NODE *n,
                        const char *path,
                        int idx,
                        int defVal);

/**
 * @return 0 on success, nonzero on error
 * @param n db node
 * @param flags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 * @param path path and name of the variable
 * @param val new value
 */
GWENHYWFAR_API 
int GWEN_DB_SetIntValue(GWEN_DB_NODE *n,
                        GWEN_TYPE_UINT32 flags,
                        const char *path,
                        int val);


/**
 * Returns the variable's retrieved value. The size of the binary
 * data is written into the int pointer argument returnValueSize.
 * @param n db node
 * @param path path and name of the variable
 * @param idx index number of the value to return
 * @param defVal default value to return in case there is no real value
 * @param defValSize size of the default value
 * @param returnValueSize pointer to a variable to receive the length
 * of the data returned
 */
GWENHYWFAR_API 
const void *GWEN_DB_GetBinValue(GWEN_DB_NODE *n,
                                const char *path,
                                int idx,
                                const void *defVal,
                                unsigned int defValSize,
                                unsigned int *returnValueSize);

/**
 * @param n db node
 * @param path path and name of the variable
 * @param flags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 * @param val The binary data that is copied into the DB.
 * @param valSize The number of bytes in the binary data value.
 *
 * @return 0 on success, nonzero on error
 */
GWENHYWFAR_API 
int GWEN_DB_SetBinValue(GWEN_DB_NODE *n,
			GWEN_TYPE_UINT32 flags,
			const char *path,
                        const void *val,
			unsigned int valSize);
/*@}*/



/** @name Group Handling
 *
 */
/*@{*/

/**
 * This function either creates a new group, returns an existing one or
 * returns an error if there is no group but the caller wanted one (depending
 * on the flags given).
 * @param n db node
 * @param flags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 * @param path path and name of the group to be created/located
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetGroup(GWEN_DB_NODE *n,
                               GWEN_TYPE_UINT32 flags,
                               const char *path);

/**
 * Returns the name of the given group.
 */
GWENHYWFAR_API 
const char *GWEN_DB_GroupName(GWEN_DB_NODE *n);

/**
 * Renames the given group.
 * @param n db node
 * @param newname new name for the group
 */
GWENHYWFAR_API 
void GWEN_DB_GroupRename(GWEN_DB_NODE *n, const char *newname);

/**
 * Adds the given group node as a new child of the given parent group node.
 *
 * The group name has no influence on what will happen in this
 * function. In other words, if the parent group already has a child
 * group with the same name as 'node', then after this function two
 * child group nodes with the same name will exist.
 *
 * @note This function takes over the ownership of the new group, so
 * you MUST NOT free it afterwards.
 *
 * @param parent Some group node that will be the parent of the added node
 * @param node Group node to add
 */
GWENHYWFAR_API 
int GWEN_DB_AddGroup(GWEN_DB_NODE *parent, GWEN_DB_NODE *node);

/**
 * This function adds all children of the second node as new children to
 * the first given one.
 *
 * @note This function does NOT take over ownership of the new
 * group. The caller is still the owner of the given group.
 * @param n db node
 * @param nn node whose children are to be added (makes deep copies)
 */
GWENHYWFAR_API 
int GWEN_DB_AddGroupChildren(GWEN_DB_NODE *n, GWEN_DB_NODE *nn);

/**
 * Unlinks a group (and thereby all its children) from its parent and
 * brothers.
 *
 * This function DOES NOT free the group, it just unlinks it. You can then use
 * it with e.g. @ref GWEN_DB_AddGroup or other functions to relink it at any
 * other position of even in other DBs.
 * @param n db node
 */
GWENHYWFAR_API 
void GWEN_DB_UnlinkGroup(GWEN_DB_NODE *n);

/**
 * Locates and removes the group of the given name.
 * @return 0 on success, nonzero on error
 * @param n db node
 * @param path path to the group to delete
 */
GWENHYWFAR_API 
int GWEN_DB_DeleteGroup(GWEN_DB_NODE *n,
                        const char *path);
/**
 * Frees all children of the given node thereby clearing it.
 * @return 0 on success, nonzero on error
 * @param n db node
 * @param path path to the group under the given node to clear
 * (if 0 then clear the given node)
 */
GWENHYWFAR_API 
int GWEN_DB_ClearGroup(GWEN_DB_NODE *n,
                       const char *path);

/** Predicate: Returns nonzero (TRUE) or zero (FALSE) if the given
 * NODE is a Group or not. Usually these group nodes are the only
 * nodes that the application gets in touch with.
 *
 * @param n db node
 */
GWENHYWFAR_API 
int GWEN_DB_IsGroup(const GWEN_DB_NODE *n);

/*@}*/



/** @name Reading and Writing From/To Streams
 *
 * These functions read or write a DB from/to GWEN_BUFFEREDIO.
 * This allows to use any source or target supported by GWEN_BUFFEREDIO
 * for data storage (these are currently sockets, files and memory buffers).
 * The flags determine how to read/write the data (e.g. if sub-groups are
 * to be written etc).
 */
/*@{*/

/**
 * Read a DB from GWEN_BUFFEREDIO.
 * @param n db node
 * @param bio buffered IO to use
 * @param dbflags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 */
GWENHYWFAR_API 
int GWEN_DB_ReadFromStream(GWEN_DB_NODE *n,
                           GWEN_BUFFEREDIO *bio,
                           GWEN_TYPE_UINT32 dbflags);

/**
 * Read a DB from a file.
 * @param n db node
 * @param fname path and name of the file to load
 * @param dbflags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 */
GWENHYWFAR_API 
int GWEN_DB_ReadFile(GWEN_DB_NODE *n,
                     const char *fname,
                     GWEN_TYPE_UINT32 dbflags);

/**
 * Write a DB to a GWEN_BUFFEREDIO.
 * @param n db node
 * @param bio buffered IO to use
 * @param dbflags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 */
GWENHYWFAR_API 
int GWEN_DB_WriteToStream(GWEN_DB_NODE *n,
                          GWEN_BUFFEREDIO *bio,
                          GWEN_TYPE_UINT32 dbflags);

/**
 * Write a DB to a file.
 * @param n db node
 * @param fname path and name of the file to create. If it already exists
 * it will be overwritten
 * @param dbflags see @ref GWEN_DB_FLAGS_OVERWRITE_VARS and others which
 * can all be OR-combined to form the flags to use.
 */
GWENHYWFAR_API 
int GWEN_DB_WriteFile(GWEN_DB_NODE *n,
                      const char *fname,
                      GWEN_TYPE_UINT32 dbflags);
/*@}*/


/** @name Iterating Through Variables and variable handling
 *
 */
/*@{*/
/**
 * Returns the first variable below the given group.
 * If there is no variable then NULL is returned.
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetFirstVar(GWEN_DB_NODE *n);


/**
 * Returns the next variable node following the given one, which has
 * the same parent node.
 *
 * This function works absolutely independently of the variable nodes'
 * names -- the returned node may or may not have the same name as the
 * specified node. The only guarantee is that the returned node will
 * be a variable node.
 *
 * If there is no variable node then NULL is returned. This can either
 * mean that the parent node does not have any further children, or
 * that the other children are groups instead of variables.
 *
 * @note This is the only function where the returned node is @e not
 * the child of the specified node, but instead it is the next node
 * with the same parent node. In other words, this function is an
 * exception. In all other functions the returned node is a child of
 * the specified node.
 *
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetNextVar(GWEN_DB_NODE *n);

/** Iterates through all variable nodes that are @e direct children
 * of the given node, calling the callback function 'func' on each
 * variable node.  Traversal will stop when 'func' returns a non-NULL
 * value, and the routine will return with that value. Otherwise the
 * routine will return NULL.
 *
 * If no nodes that are variables are found as children, then
 * this function will simply do nothing.
 *
 * @param node The group node whose children shall be iterated through.
 * @param func The function to be called with each group node.
 * @param user_data A pointer passed on to the function 'func'.
 * @return The non-NULL pointer returned by 'func' as soon as it
 * returns one. Otherwise (i.e. 'func' always returns NULL)
 * returns NULL.
 * @author Christian Stimming <stimming@tuhh.de> */
GWENHYWFAR_API 
void *GWEN_DB_Variables_Foreach(GWEN_DB_NODE *node, GWEN_DB_NODES_CB func,
                                void *user_data);

/** Returns the number of variable nodes that are @e direct children
 * of the given node. In other words, this is the number of variable
 * nodes that will be reached in the GWEN_DB_Variables_Foreach()
 * function. */
GWENHYWFAR_API 
unsigned int GWEN_DB_Variables_Count(const GWEN_DB_NODE *node);
    
/**
 * Returns the type of the first value of the given variable
 * @param n root node of the DB
 * @param p path of the variable to inspect
 */
GWENHYWFAR_API 
GWEN_DB_VALUETYPE GWEN_DB_GetVariableType(GWEN_DB_NODE *n,
                                          const char *p);

/**
 * Deletes the given variable by removing it and its values from the DB.
 * @param n root of the DB
 * @param path path to the variable to remove
 * @return Zero on success, nonzero on error
 */
GWENHYWFAR_API 
int GWEN_DB_DeleteVar(GWEN_DB_NODE *n,
                      const char *path);

/**
 * Checks whether the given variable exists.
 * @return Zero if variable not found, nonzero otherwise
 * @param n root of the DB
 * @param path path to the variable to check for
 */
GWENHYWFAR_API 
int GWEN_DB_VariableExists(GWEN_DB_NODE *n,
                           const char *path);

/** Predicate: Returns nonzero (TRUE) or zero (FALSE) if the given
 * NODE is a Variable or not. Usually the Application does not get in
 * touch with such Nodes but only with nodes that are Groups.
 *
 * @param n db node
 */
GWENHYWFAR_API 
int GWEN_DB_IsVariable(const GWEN_DB_NODE *n);
/*@}*/


/** @name Iterating Through Values and value handling
 *
 */
/*@{*/
/**
 * Returns the first value below the given variable.
 * If there is no value then NULL is returned.
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetFirstValue(GWEN_DB_NODE *n);

/**
 * Returns the next value node following the given one, which has the
 * same parent node.
 *
 * This function works absolutely independently of the value nodes'
 * names (FIXME: is there such thing as a value name? If not, then
 * this paragraph should be removed) -- the returned node may or may
 * not have the same name as the specified node. The only guarantee is
 * that the returned node will be a value node.
 *
 * If there is no value node then NULL is returned. This can either
 * mean that the parent node does not have any further children, or
 * that the other children are variables instead of values (FIXME: Is
 * the last sentence true? If this can't be the case then it can be
 * deleted.)
 *
 * @note This is the only function where the returned node is @e not
 * the child of the specified node, but instead it is the next node
 * with the same parent node. In other words, this function is an
 * exception. In all other functions the returned node is a child of
 * the specified node.
 *
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_NODE *GWEN_DB_GetNextValue(GWEN_DB_NODE *n);

/** Iterates through all value nodes that are @e direct children
 * of the given node, calling the callback function 'func' on each
 * value node.  Traversal will stop when 'func' returns a non-NULL
 * value, and the routine will return with that value. Otherwise the
 * routine will return NULL.
 *
 * If no nodes that are values are found as children, then
 * this function will simply do nothing.
 *
 * @param node The variable node whose children shall be iterated through.
 * @param func The function to be called with each group node.
 * @param user_data A pointer passed on to the function 'func'.
 * @return The non-NULL pointer returned by 'func' as soon as it
 * returns one. Otherwise (i.e. 'func' always returns NULL)
 * returns NULL.
 * @author Christian Stimming <stimming@tuhh.de> */
GWENHYWFAR_API 
void *GWEN_DB_Values_Foreach(GWEN_DB_NODE *node, GWEN_DB_NODES_CB func,
                             void *user_data);

/** Returns the number of value nodes that are @e direct children of
 * the given node. In other words, this is the number of value nodes
 * that will be reached in the GWEN_DB_Values_foreach() function. */
GWENHYWFAR_API 
unsigned int GWEN_DB_Values_Count(const GWEN_DB_NODE *node);

/**
 * Returns the type of the given value.
 * @param n db node
 */
GWENHYWFAR_API 
GWEN_DB_VALUETYPE GWEN_DB_GetValueType(GWEN_DB_NODE *n);

/** Predicate: Returns nonzero (TRUE) or zero (FALSE) if the given
 * NODE is a Value or not. Usually the Application does not get in
 * touch with such Nodes but only with nodes that are Groups.
 *
 * @param n db node
 */
GWENHYWFAR_API 
int GWEN_DB_IsValue(const GWEN_DB_NODE *n);
/*@}*/






/** @name Debugging
 *
 * These functions are for debugging purposes only. You should NOT consider
 * them part of the API.
 */
/*@{*/

/**
 * Dumps the content of the given DB to the given file (e.g. stderr).
 * @param n node to dump
 * @param f destination file (e.g. stderr)
 * @param insert number of blanks to insert at every line
 */
GWENHYWFAR_API 
void GWEN_DB_Dump(GWEN_DB_NODE *n, FILE *f, int insert);
/*@}*/


/*@}*/


#ifdef __cplusplus
}
#endif


#endif



