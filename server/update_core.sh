#!/bin/bash

bindgen core_headers.h -o src/core.rs -- -x c++
sed -i '1s/^/#![allow(warnings)]\n/' src/core.rs
