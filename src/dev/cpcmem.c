/*
 * cpcmem.c - Copyright (c) 2001, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Olivier Poncet
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cpcmem.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

G_DEFINE_TYPE(GdevCPCMEM, gdev_cpcmem, GDEV_TYPE_DEVICE)

/**
 * GdevCPCMEM::class_init()
 *
 * @param cpcmem_class specifies the GdevCPCMEM class
 */
static void gdev_cpcmem_class_init(GdevCPCMEMClass *cpcmem_class)
{
}

/**
 * GdevCPCMEM::init()
 *
 * @param cpcmem specifies the GdevCPCMEM instance
 */
static void gdev_cpcmem_init(GdevCPCMEM *cpcmem)
{
  (void) memset(cpcmem->data, 0, 16384);
}

/**
 * GdevCPCMEM::new()
 *
 * @return the GdevCPCMEM instance
 */
GdevCPCMEM *gdev_cpcmem_new(void)
{
  return(g_object_new(GDEV_TYPE_CPCMEM, NULL));
}

/**
 * GdevCPCMEM::new_from_file()
 *
 * @return the GdevCPCMEM instance
 */
GdevCPCMEM *gdev_cpcmem_new_from_file(gchar *filename, gulong offset)
{
  GdevCPCMEM *cpcmem = g_object_new(GDEV_TYPE_CPCMEM, NULL);
  struct stat statbuf;
  int fd = -1;

  if((cpcmem != NULL) && (filename != NULL)) {
    if((fd = open(filename, O_RDONLY | O_BINARY)) == -1) {
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s (open): %s", filename, strerror(errno));
      goto load_error;
    }
    if(fstat(fd, &statbuf) == -1) {
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s (stat): %s", filename, strerror(errno));
      goto load_error;
    }
    if(S_ISREG(statbuf.st_mode) == 0) {
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s (stat): %s", filename, "cannot load !");
      goto load_error;
    }
    if(lseek(fd, (off_t) offset, SEEK_SET) == (off_t) -1) {
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s (seek): %s", filename, strerror(errno));
      goto load_error;
    }
load_retry:
    if(read(fd, cpcmem->data, 16384) == -1) {
      if(errno == EINTR) {
        goto load_retry;
      }
      g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, "%s (read): %s", filename, strerror(errno));
      goto load_error;
    }
load_error:
    if(fd != -1) {
      (void) close(fd);
    }
  }
  return(cpcmem);
}
