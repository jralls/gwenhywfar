/***************************************************************************
 $RCSfile$
                             -------------------
    cvs         : $Id$
    begin       : Mon Feb 09 2004
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

#ifndef GWEN_UI_EDITBOX_H
#define GWEN_UI_EDITBOX_H

#include <gwenhywfar/misc.h>
#include <gwenhywfar/inherit.h>
#include <gwenhywfar/widget.h>

#define GWEN_EDITBOX_FLAGS_MULTILINE 0x00010000
#define GWEN_EDITBOX_FLAGS_DYNAMIC   0x00020000
#define GWEN_EDITBOX_FLAGS_CLEARALL  0x00040000
#define GWEN_EDITBOX_FLAGS_INSERT    0x00080000


GWEN_WIDGET *GWEN_EditBox_new(GWEN_WIDGET *parent,
                              GWEN_TYPE_UINT32 flags,
                              const char *name,
                              const char *text,
                              int x, int y,
                              int width, int height,
                              int maxLen);



#endif




