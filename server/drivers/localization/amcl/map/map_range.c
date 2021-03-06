/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003
 *     Andrew Howard
 *     Brian Gerkey    
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/**************************************************************************
 * Desc: Range routines
 * Author: Andrew Howard
 * Date: 18 Jan 2003
 * CVS: $Id: map_range.c 8108 2009-07-23 23:03:37Z thjc $
**************************************************************************/

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "map.h"
#include <libplayercommon/playercommon.h>

// Extract a single range reading from the map.  Unknown cells and/or
// out-of-bound cells are treated as occupied, which makes it easy to
// use Stage bitmap files.
double map_calc_range(map_t *map, double ox, double oy, double oa, double max_range)
{
  int i, j;
  int ai, aj, bi, bj;
  double dx, dy;
  map_cell_t *cell;
  
  if (fabs(cos(oa)) > fabs(sin(oa)))
  {
    ai = (int) MAP_GXWX(map, ox);
    bi = (int) MAP_GXWX(map, ox + max_range * cos(oa));
    
    aj = (int) MAP_GYWY(map, oy);
    dy = tan(oa) * map->scale;

    if (ai < bi)
    {
      for (i = ai; i < bi; i++)
      {
        j = (int) MAP_GYWY(map, oy + (i - ai) * dy);
        if (MAP_VALID(map, i, j))
        {
          cell = map->cells + MAP_INDEX(map, i, j);
          if (cell->occ_state >= 0)
            return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
        }
        else
            return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;          
      }
    }
    else
    {
      for (i = ai; i > bi; i--)
      {
        j = (int) MAP_GYWY(map, oy + (i - ai) * dy);
        if (MAP_VALID(map, i, j))
        {
          cell = map->cells + MAP_INDEX(map, i, j);
          if (cell->occ_state >= 0)
            return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
        }
        else
          return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
      }
    }
  }
  else
  {
    ai = (int) MAP_GXWX(map, ox);
    dx = tan(M_PI/2 - oa) * map->scale;
    
    aj = (int) MAP_GYWY(map, oy);
    bj = (int) MAP_GYWY(map, oy + max_range * sin(oa));

    if (aj < bj)
    {
      for (j = aj; j < bj; j++)
      {
        i = (int) MAP_GXWX(map, ox + (j - aj) * dx);
        if (MAP_VALID(map, i, j))
        {
          cell = map->cells + MAP_INDEX(map, i, j);
          if (cell->occ_state >= 0)
            return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
        }
        else
          return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
      }
    }
    else
    {
      for (j = aj; j > bj; j--)
      {
        i = (int) MAP_GXWX(map, ox + (j - aj) * dx);
        if (MAP_VALID(map, i, j))
        {
          cell = map->cells + MAP_INDEX(map, i, j);
          if (cell->occ_state >= 0)
            return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
        }
        else
          return sqrt((i - ai) * (i - ai) + (j - aj) * (j - aj)) * map->scale;
      }
    }
  }
  return max_range;
}

