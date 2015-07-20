#!/bin/bash

`fswebcam -q -r 10202x1210 -d /dev/video0 --no-banner ftp/pictures/$1`
`fswebcam -q -r 640x480 -d /dev/video1 --no-banner ftp/pictures/$2`
`fswebcam -q -r 640x480 -d /dev/video2 --no-banner ftp/pictures/$3`
#touch ftp/pictures/$1
#touch ftp/pictures/$2
#touch ftp/pictures/$3
