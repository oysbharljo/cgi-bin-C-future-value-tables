#!/bin/sh
set -v
gcc -O2 -c fvtable.c
gcc -lm -o fvtable.cgi fvtable.o
sudo cp fvtable.cgi /home/http/cgi-bin
sudo chown apache:apache /home/http/cgi-bin/fvtable.cgi
