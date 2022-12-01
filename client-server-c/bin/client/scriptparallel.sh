#!/bin/sh

# Setup
echo Hello, World! > test

# Start
../server/./server

# Test
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client 
echo "-s test" || ./client 
echo "-s test" || ./client 
echo "-s test" || ./client 
echo "-s test" || ./client

# Stop
killall server