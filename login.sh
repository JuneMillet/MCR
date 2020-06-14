#!/bin/bash
pd=0
while [ $pd -eq 0 ]
do
	echo "请输入密码："
	read password
	login="passwd"
	if [ $password  == $login ];
	then
		gcc ./MCR/server.c -o ./MCR/server.exe
		sudo ./MCR/server.exe
		gcc ./MCR/client.c -o ./MCR/client.exe
                gnome-terminal -x bash -c "./MCR/client.exe;exec bash;"
		gnome-terminal -x bash -c "./MCR/client.exe;exec bash;"
		exit
	else
		echo -e "\033[31mERROR:\033[0m \033[34m密码错误！\033[0m"
	fi
done
