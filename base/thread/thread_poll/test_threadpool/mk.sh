#/bin/bash

rm -rf test/*
gcc test_pool.c -o tt -lthreadpool
