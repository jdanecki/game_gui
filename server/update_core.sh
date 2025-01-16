#!/bin/bash

bindgen core_headers.h -o core.rs -- -x c++
sed -i '1s/^/#![allow(warnings)]\n/' src/core.rs
