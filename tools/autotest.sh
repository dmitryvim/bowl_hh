#!/bin/sh

while inotifywait -qq -e modify -r . ; do
    make -q test || make run-test | tee test.log
done

