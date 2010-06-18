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
/*  MicronSonar Proxy for libplayerc library. 
 *  Structure based on the rest of libplayerc. 
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "playerc.h"
#include "error.h"

// Local declarations
void playerc_micronsonar_putmsg (playerc_micronsonar_t *device, player_msghdr_t *header,
                 player_micronsonar_data_t *data, size_t len);

// Create a new micron sonar proxy
playerc_micronsonar_t *playerc_micronsonar_create(playerc_client_t *client, int index)
{
  playerc_micronsonar_t *device;

  device = malloc(sizeof(playerc_micronsonar_t));
  memset(device, 0, sizeof(playerc_micronsonar_t));
  playerc_device_init(&device->info, client, PLAYER_MICRONSONAR_CODE, index,
                      (playerc_putmsg_fn_t) playerc_micronsonar_putmsg);

  return device;
}

// Destroy a micron sonar proxy
void playerc_micronsonar_destroy(playerc_micronsonar_t *device)
{
  playerc_device_term(&device->info);
  if ( NULL != device->image )
  {
      free( device->image );
      device->image = NULL;
  }
  free(device);
}

// Subscribe to the micron sonar device
int playerc_micronsonar_subscribe(playerc_micronsonar_t *device, int access)
{
  return playerc_device_subscribe(&device->info, access);
}

// Un-subscribe from the micron sonar device
int playerc_micronsonar_unsubscribe(playerc_micronsonar_t *device)
{
  return playerc_device_unsubscribe(&device->info);
}

// Process incoming data
void playerc_micronsonar_putmsg(playerc_micronsonar_t *device, player_msghdr_t *header,
                player_micronsonar_data_t *data, size_t len)
{
  if((header->type == PLAYER_MSGTYPE_DATA) &&
     (header->subtype == PLAYER_MICRONSONAR_DATA_STATE))
  {
    device->width        = data->width;
    device->height       = data->height;
    device->bpp          = data->bpp;
    device->format       = data->format;
    device->image_count  = data->image_count;
    device->image        = realloc(device->image, sizeof(device->image[0])*device->image_count);
    device->centreX      = data->centreX;
    device->centreY      = data->centreY;
    device->range        = data->range;
    device->numBins      = data->numBins;
    device->startAngle   = data->startAngle;
    device->endAngle     = data->endAngle;
    
    if (device->image)
      memcpy(device->image, data->image, device->image_count);
    else
      PLAYERC_ERR1("failed to allocate memory for image, needed %ld bytes\n", sizeof(device->image[0])*device->image_count);
  }
  else
    PLAYERC_WARN2("skipping micronsonar message with unknown type/subtype: %s/%d\n",
                 msgtype_to_str(header->type), header->subtype);
  return;
}

/* Set the output for the micron sonar device. */
int playerc_micronsonar_say(playerc_micronsonar_t *device, char *str)
{
  player_micronsonar_cmd_say_t cmd;
  
  memset(&cmd, 0, sizeof(cmd));
  cmd.string = str;
  cmd.string_count = strlen(str) + 1; 
    
  return playerc_client_write(device->info.client, 
                  &device->info, PLAYER_MICRONSONAR_CMD_SAY, &cmd, NULL);
}

// Starts a scan
int playerc_micronsonar_scan(playerc_micronsonar_t *device, double startAngle, double endAngle)
{
  player_micronsonar_cmd_scan_t cmd;
  
  memset(&cmd, 0, sizeof(cmd));
  cmd.startAngle = startAngle;
  cmd.endAngle = endAngle;
    
  return playerc_client_write(device->info.client, 
                  &device->info, PLAYER_MICRONSONAR_CMD_SCAN, &cmd, NULL);
}

// Configure the micron sonar.
int playerc_micronsonar_set_config( playerc_micronsonar_t *device,
                             int range, int numBins, double gain )
{
  player_micronsonar_config_t config;

  config.range = range;
  config.numBins = numBins;
  config.gain = gain;

  if ( playerc_client_request( device->info.client, &device->info,
    PLAYER_MICRONSONAR_REQ_SET_CONFIG, (void*)&config, NULL ) < 0 )
  {
    return -1;
  }

  // if the set suceeded copy them locally
  device->range = config.range;
  device->numBins = config.numBins;
  device->gain = config.gain;

  return 0;
}

// Get the configuration of the micron sonar.
int playerc_micronsonar_get_config( playerc_micronsonar_t *device,
                             int* range, int* numBins, double* gain )
{
  player_micronsonar_config_t* config = NULL;

  if ( playerc_client_request( device->info.client, &device->info,
    PLAYER_MICRONSONAR_REQ_GET_CONFIG, NULL, (void**)&config ) < 0 )
  {
    if ( NULL != config )
    {
      player_micronsonar_config_t_free( config );
    }
    return -1;
  }

  // if the get suceeded copy them locally
  *range = device->range = config->range;
  *numBins = device->numBins = config->numBins;
  *gain = device->gain = config->gain;

  player_micronsonar_config_t_free( config );
  
  return 0;
}
