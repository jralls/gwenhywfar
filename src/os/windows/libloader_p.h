/***************************************************************************
  $RCSfile$
                             -------------------
    cvs         : $Id$
    begin       : Fri Nov 22 2002
    copyright   : (C) 2002 by Martin Preuss
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


#ifndef GWENHYFWAR_LIBLOADER_P_H
#define GWENHYFWAR_LIBLOADER_P_H "$Id"

#include <gwenhyfwar/gwenhyfwarapi.h>
#include <gwenhyfwar/error.h>
#include "libloader.h"


#ifdef __cplusplus
extern "C" {
#endif


GWENHYFWAR_API struct GWEN_LIBLOADERSTRUCT {
  void *handle;
};


GWENHYFWAR_API GWEN_ERRORCODE GWEN_LibLoader_ModuleInit();
GWENHYFWAR_API GWEN_ERRORCODE GWEN_LibLoader_ModuleFini();


#ifdef __cplusplus
}
#endif


#endif /* GWENHYFWAR_LIBLOADER_P_H */


