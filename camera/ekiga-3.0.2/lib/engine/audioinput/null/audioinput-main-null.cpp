
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2008 Damien Sandras
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * Ekiga is licensed under the GPL license and as a special exception,
 * you have permission to link or otherwise combine this program with the
 * programs OPAL, OpenH323 and PWLIB, and distribute the combination,
 * without applying the requirements of the GNU GPL to the OPAL, OpenH323
 * and PWLIB programs, as long as you do follow the requirements of the
 * GNU GPL for all the rest of the software thus combined.
 */


/*
 *                         audionput-main-null.cpp  -  description
 *                         ------------------------------------------
 *   begin                : written in 2008 by Matthias Schneider
 *   copyright            : (c) 2008 by Matthias Schneider
 *   description          : code to hook the NULL audioinput manager 
 *                          into the main program
 *
 */

#include "audioinput-main-null.h"
#include "audioinput-core.h"
#include "audioinput-manager-null.h"

bool
audioinput_null_init (Ekiga::ServiceCore &core,
	    int */*argc*/,
	    char **/*argv*/[])
{
  bool result = false;
  Ekiga::AudioInputCore *audioinput_core = NULL;

  audioinput_core
    = dynamic_cast<Ekiga::AudioInputCore*>(core.get ("audioinput-core"));

  if (audioinput_core != NULL) {

    GMAudioInputManager_null *audioinput_manager = new GMAudioInputManager_null(core);

    audioinput_core->add_manager (*audioinput_manager);
    result = true;
  }

  return result;
}
