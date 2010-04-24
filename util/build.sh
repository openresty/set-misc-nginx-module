#!/bin/bash

# this file is mostly meant to be used by the author himself.

rm ~/work/nginx-0.8.33/objs/addon/ndk/ndk.o ~/work/nginx-0.8.33/objs/addon/ndk-nginx-module/ndk.o

root=`pwd`
home=~
cd ~/work
version=$1
opts=$2
if [ ! -f "nginx-$version.tar.gz" ]; then
    lwp-mirror "http://sysoev.ru/nginx/nginx-$version.tar.gz" nginx-$version.tar.gz
fi
tar -xzvf nginx-$version.tar.gz
cd nginx-$version/
if [[ "$BUILD_CLEAN" -eq 1 || ! -f Makefile || "$root/config" -nt Makefile || "$root/util/build.sh" -nt Makefile ]]; then
    ./configure --prefix=/opt/nginx \
          --add-module=$root/../echo-nginx-module \
          --add-module=$root/../ndk-nginx-module \
          --add-module=$root $opts \
          --with-debug
          #--add-module=$home/work/ndk \
  #--without-http_ssi_module  # we cannot disable ssi because echo_location_async depends on it (i dunno why?!)

fi
if [ -f /opt/nginx/sbin/nginx ]; then
    rm -f /opt/nginx/sbin/nginx
fi
if [ -f /opt/nginx/logs/nginx.pid ]; then
    kill `cat /opt/nginx/logs/nginx.pid`
fi
make -j3
make install

