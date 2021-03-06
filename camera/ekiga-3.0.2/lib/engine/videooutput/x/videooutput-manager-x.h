
/* Ekiga -- A VoIP and Video-Conferencing application
 * Copyright (C) 2000-2006 Damien Sandras
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
*                         videooutput-manager-x.h -  description
 *                         ----------------------------------
 *   begin                : Sun Nov 19 2006
 *   copyright            : (C) 2006-2008 by Matthias Schneider
 *                          (C) 2000-2008 by Damien Sandras
 *   description          : Class to allow video output to a X/XVideo
 *                          accelerated window
 
 */


#ifndef _VIDEOOUTPUT_MANAGER_X_H_
#define _VIDEOOUTPUT_MANAGER_X_H_

#include "videooutput-manager-common.h"
#include "xwindow.h"

/**
 * @addtogroup videooutput
 * @{
 */

class GMVideoOutputManager_x
   : public  GMVideoOutputManager
{
public:
  GMVideoOutputManager_x (Ekiga::ServiceCore & _core);

  virtual ~GMVideoOutputManager_x ();

  virtual bool frame_display_change_needed ();

  virtual void setup_frame_display ();

  virtual void close_frame_display ();

  virtual void display_frame (const char *frame,
                             unsigned width,
                             unsigned height);

  virtual void display_pip_frames (const char *local_frame,
                                 unsigned lf_width,
                                 unsigned lf_height,
                                 const char *remote_frame,
                                 unsigned rf_width,
                                 unsigned rf_height);

protected:

  virtual void sync(UpdateRequired sync_required);

  XWindow *lxWindow;
  XWindow *rxWindow;

  Display *lDisplay;
  Display *rDisplay;

  bool pip_window_available;
};

/**
 * @}
 */

#endif /* _VIDEOOUTPUT_MANAGER_X_H_ */
