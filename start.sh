#!/bin/bash

cd server/
cargo run &
server_PID=$!
cd ../SDL
./build/pime_SDL
kill $server_PID
