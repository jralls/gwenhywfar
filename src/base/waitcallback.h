/***************************************************************************
 $RCSfile$
 -------------------
 cvs         : $Id$
 begin       : Sat Jan 17 2004
 copyright   : (C) 2004 by Martin Preuss
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


#ifndef GWEN_WAIT_CALLBACK_H
#define GWEN_WAIT_CALLBACK_H

#include <gwenhywfar/gwenhywfarapi.h>


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MOD_WAITCALLBACK Long Term Operation Wait Callback
 * @ingroup MOD_BASE
 *
 * @brief This file contains the definition of a GWEN_WAITCALLBACK.
 *
 * WaitCallbacks are called during long term operations to give the user
 * the opportunity to abort the operation.
 * The default implementation does nothing. But an application can use such
 * a callback to:
 * <ul>
 *   <li>Check whether the user wants to abort</li>
 *   <li>Update a GUI thus preventing X toolkits such as QT from freezing
 * </ul>
 * <p>
 * Wait callbacks are organized in callback-contexts which must be entered
 * and left.
 * </p>
 * <p>
 * In an application you would map a context to a progress dialog or something
 * similar. To support this the callback gets informed about when a context
 * is entered and leaved.
 * </p>
 * <p>
 * The following is an axample of how you could make use of this callbak in
 * a GUI application:
 * </p>
 * @code
 * void doSomethingVeryEnduring(WINDOW *myData) {
 *   GWEN_WAITCALLBACK_CTX *ctx;
 *   unsigned int i;
 *
 *   ctx=GWEN_WaitCallback_Context_new(myCallBackFunction, (void*)myData);
 *   GWEN_WaitCallback_Enter(ctx);
 *
 *   for (i=0; i<10000000; i++) {
 *     if (GWEN_WaitCallback(i)==GWEN_WaitCallbackResult_Abort) {
 *       fprintf(stderr, "User aborted.\n");
 *     }
 *   }
 *   GWEN_WaitCallback_Leave();
 * }
 * @endcode
 *
 * <p>
 * The callback function itself might be implemented like this:
 * </p>
 *
 * @code
 *
 * GWEN_WAITCALLBACK_RESULT myCallBackFunction(int count,
 *                                             GWEN_WAITCALLBACK_MODE m,
 *                                             GWEN_WAITCALLBACK_CTX *ctx){
 *
 *  WINDOW *w;
 *   w=GWEN_WaitCallback_Context_GetData(ctx);
 *
 *   switch(m) {
 *     case GWEN_WaitCallbackMode_Enter:
 *       Window_Open(w);
 *       break;
 *     case GWEN_WaitCallbackMode_Leave:
 *       Window_Close(w);
 *       break;
 *     default:
 *       return do_something_to_ask_the_user_whether_he_wishes_to_abort();
 *   }
 *   return GWEN_WaitCallbackResult_Continue;
 * }
 *
 * @endcode
 *
 * <p>
 * Please note that the type @e WINDOW is a fictive type, it is just a
 * placeholder representing a GUI window or dialog.
 * </p>
 * <p>
 * As you can see the callback function checks for entering and leaving of
 * callback contexts. So the application should call
 * @ref GWEN_WaitCallback_Enter at the beginning of the long term operation
 * to give the callback function the opportunity to create a window.
 * At the end of the operation you should call @ref GWEN_WaitCallback_Leave
 * in order to close the window.
 * </p>
 */
/*@{*/


#include <time.h>

/**
 *
 */
GWENHYWFAR_API
typedef enum {
  /** normal callback */
  GWEN_WaitCallbackMode_Normal=0,
  /** this mode is used when entering callback context */
  GWEN_WaitCallbackMode_Enter,
  /** this mode is used when leaving callback context */
  GWEN_WaitCallbackMode_Leave
} GWEN_WAITCALLBACK_MODE;


/**
 *
 */
GWENHYWFAR_API
typedef enum {
  /** tells the caller that it is ok to continue */
  GWEN_WaitCallbackResult_Continue=0,
  /** tells the caller to abort */
  GWEN_WaitCallbackResult_Abort
} GWEN_WAITCALLBACK_RESULT;


GWENHYWFAR_API
typedef struct GWEN_WAITCALLBACK_CTX GWEN_WAITCALLBACK_CTX;

/**
 * This is the callback function of a context.
 * @param count the number of the call to this function.
 * @param m callback mode (see @ref GWEN_WaitCallbackMode_Normal and
 * following)
 * @param ctx callback context used. The application can store private
 * data within such a context
 */
GWENHYWFAR_API typedef
GWEN_WAITCALLBACK_RESULT (*GWEN_WAITCALLBACK_FN)(int count,
                                                 GWEN_WAITCALLBACK_MODE m,
                                                 GWEN_WAITCALLBACK_CTX *ctx);


/**
 * An application can call this function from within long term operations.
 * The callback function of the last callback context entered will be
 * called. This allows for the caller to be independant of the context
 * actually used.
 * @param count optional number, the value is only interpreted by the
 * implemented callback function
 */
GWENHYWFAR_API
GWEN_WAITCALLBACK_RESULT GWEN_WaitCallback(int count);


/**
 * Creates a new waitcallback context. This is not automatically selected.
 * @param fn your callback function for this context
 * @param data optional private data for the callback function.
 */
GWENHYWFAR_API
GWEN_WAITCALLBACK_CTX *GWEN_WaitCallback_Context_new(GWEN_WAITCALLBACK_FN fn,
                                                     void *data);

/**
 * Destructor for a context. You MUST call this for every created context
 * to avoid memory leaks.
 */
GWENHYWFAR_API
void GWEN_WaitCallback_Context_free(GWEN_WAITCALLBACK_CTX *ctx);

/**
 * Returns the private data given when the context was created (see
 * @ref GWEN_WaitCallback_Context_new)
 */
GWENHYWFAR_API
void *GWEN_WaitCallback_Context_GetData(GWEN_WAITCALLBACK_CTX *ctx);

/**
 * Enters a waitcallback context. After calling this all calls to
 * @ref GWEN_WaitCallback will use the given context until
 * @ref GWEN_WaitCallback_Leave is called. This functions calls the
 * callback function of this context with the value
 * @ref GWEN_WaitCallbackMode_Enter
 */
GWENHYWFAR_API
void GWEN_WaitCallback_Enter(GWEN_WAITCALLBACK_CTX *ctx);

/**
 * Leaves a waitcallback context.
 * This functions calls the callback function of the current context with the
 * value @ref GWEN_WaitCallbackMode_Leave and restores the context used just
 * before @ref GWEN_WaitCallback_Enter was called.
 */
GWENHYWFAR_API
void GWEN_WaitCallback_Leave();


/**
 * Returns the time when the callback function was last called
 * (or 0 if it has never been called)
 */
GWENHYWFAR_API
time_t GWEN_WaitCallback_LastCalled(GWEN_WAITCALLBACK_CTX *ctx);

/*@}*/


#ifdef __cplusplus
}
#endif


#endif

