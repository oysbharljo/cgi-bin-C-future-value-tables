#!/bin/sh
set -v
clang -c fvai.c
clang -lm -o fvai.cgi fvai.o
sudo cp fvai.cgi /srv/httpd/cgi-bin
sudo chown apache:apache /srv/httpd/cgi-bin/fvai.cgi
