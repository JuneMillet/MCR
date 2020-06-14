#!/bin/bash
pd=0
while [ $pd -eq 0 ]
do
                gcc ./MCR/client.c -o ./MCR/client.exe
		gnome-terminal -x bash -c "play ./MCR/music.mp3;exec bash;"
                gnome-terminal -x bash -c "./MCR/client.exe;exec bash;"
                gnome-terminal -x bash -c "./MCR/client.exe;exec bash;"
                exit
done

