#/*
# *  Player - One Hell of a Robot Server
# *  Copyright (C) 2004
# *     Andrew Howard
# *                      
# *
# *  This library is free software; you can redistribute it and/or
# *  modify it under the terms of the GNU Lesser General Public
# *  License as published by the Free Software Foundation; either
# *  version 2.1 of the License, or (at your option) any later version.
# *
# *  This library is distributed in the hope that it will be useful,
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# *  Lesser General Public License for more details.
# *
# *  You should have received a copy of the GNU Lesser General Public
# *  License along with this library; if not, write to the Free Software
# *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# */

# Desc: Test the laser
# Author: Andrew Howard
# Date: 15 Sep 2004
# CVS: $Id: test_laser.py 8114 2009-07-24 11:28:20Z thjc $

from playerc import *


def test_laser(client, index, context):
    """Basic test of the laser interface."""

    laser = playerc_laser(client, index)
    if laser.subscribe(PLAYERC_OPEN_MODE) != 0:
        raise playerc_error_str()    

    for i in range(10):

        while 1:
            id = client.read()
            if id == laser.info.id:
                break

        if context:
            print context,            
        print "laser: [%14.3f] [%d]" % (laser.info.datatime, laser.scan_count),
        for i in range(3):
            print "[%06.3f, %06.3f]" % (laser.scan[i][0], laser.scan[i][1]),
        print

    laser.unsubscribe()    
    return
