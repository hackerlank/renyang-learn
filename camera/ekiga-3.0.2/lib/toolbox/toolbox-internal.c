
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
 *                         toolbox-internal.h  -  description 
 *                         ------------------------------------------
 *   begin                : Aug 2008
 *   copyright            : (C) 2008 by Julien Puydt
 *   description          : Declaration of internal-use functions
 */

#include "toolbox-internal.h"

void
gm_open_uri_fallback (const gchar *uri)
{
  gchar *commandline = NULL;
  gboolean success = FALSE;

  if (!success && g_getenv("KDE_FULL_SESSION") != NULL) {

    commandline = g_strdup_printf ("kfmclient exec %s", uri);
    success = g_spawn_command_line_async (commandline, NULL);
    g_free (commandline);
  }

  if (!success) {

    commandline = g_strdup_printf ("sensible-browser %s", uri);
    success = g_spawn_command_line_async (commandline, NULL);
    g_free (commandline);
  }

  if (!success) {

    commandline = g_strdup_printf ("firefox %s", uri);
    success = g_spawn_command_line_async (commandline, NULL);
    g_free (commandline);
  }

  if (!success) {

    commandline = g_strdup_printf ("konqueror %s", uri);
    success = g_spawn_command_line_async (commandline, NULL);
    g_free (commandline);
  }
}
