#!/bin/sh

# Setup
echo Hello, World! > test

# Start
../server/./my_server

# Test
echo "-s test" | ./client
echo "-s test" | ./client
echo "-s test" | ./client 
echo "-s test" | ./client 
echo "-s test" | ./client 
echo "-s test" | ./client 
echo "-s test" | ./client

# Stop
killall my_server