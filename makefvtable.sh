#!/bin/sh
set -v
gcc -O2 -c fvtable.c
gcc -lm -o fvtable.cgi fvtable.o
sudo cp fvtable.cgi /srv/httpd/cgi-bin
sudo chown apache:apache /srv/httpd/cgi-bin/fvtable.cgi
