#!/bin/sh
set -v
clang -c fvan.c
clang -lm -o fvan.cgi fvan.o
sudo cp fvan.cgi /srv/httpd/cgi-bin
sudo chown apache:apache /srv/httpd/cgi-bin/fvan.cgi
