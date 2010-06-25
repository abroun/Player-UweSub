/* 
 *  libplayerc : a Player client library
 *  Copyright (C) Andrew Howard 2002-2003
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) Andrew Howard 2003
 *                      
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*  dsPIC Proxy for libplayerc library. 
 *  Structure based on the rest of libplayerc. 
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "playerc.h"
#include "error.h"

// Local declarations
void playerc_dspic_putmsg (playerc_dspic_t* device, player_msghdr_t *header, player_dspic_data_t* data, size_t len);

// Create a new dspic proxy
playerc_dspic_t* playerc_dspic_create(playerc_client_t *client, int index)
{
  playerc_dspic_t* device;

  device = malloc(sizeof(playerc_dspic_t));
  memset(device, 0, sizeof(playerc_dspic_t));
  playerc_device_init(&device->info, client, PLAYER_DSPIC_CODE, index,
                      (playerc_putmsg_fn_t) playerc_dspic_putmsg);

  return device;
}

// Destroy a dspic proxy
void playerc_dspic_destroy(playerc_dspic_t *device)
{
  
  free(device);
}

// Subscribe to the dspic device
int playerc_dspic_subscribe(playerc_dspic_t *device, int access)
{
  return playerc_device_subscribe(&device->info, access);
}

// Un-subscribe from the dspic device
int playerc_dspic_unsubscribe(playerc_dspic_t *device)
{
  return playerc_device_unsubscribe(&device->info);
}

// Process incoming data
void playerc_dspic_putmsg(playerc_dspic_t *device, player_msghdr_t *header,
                player_dspic_data_t *data, size_t len)
{
  if((header->type == PLAYER_MSGTYPE_DATA) &&
     (header->subtype == PLAYER_DSPIC_DATA_STATE))
  {
    device->msgtype        = data->msgtype;
    device->reading        = data->reading;
    device->transducer     = data->transducer;
    device->distance       = data->distance;
    device->intensity      = data->intensity;
    
  }
  else
    PLAYERC_WARN2("skipping dspic message with unknown type/subtype: %s/%d\n",
                 msgtype_to_str(header->type), header->subtype);
  return;
}

/* Set the output for the micron sonar device. */
int playerc_dspic_say(playerc_dspic_t *device, char *str)
{
  player_dspic_cmd_say_t cmd;
  
  memset(&cmd, 0, sizeof(cmd));
  cmd.string = str;
  cmd.string_count = strlen(str) + 1; 
    
  return playerc_client_write(device->info.client, 
                  &device->info, PLAYER_DSPIC_CMD_SAY, &cmd, NULL);
}


