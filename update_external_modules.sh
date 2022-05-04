#!/usr/bin/env sh

cd "$(dirname ${0})"
git submodule
git submodule update --init
cp -v ./json/single_include/nlohmann/json.hpp ./json.hpp
cp -v ./CImg/CImg.h ./CImg.h
cp -v ./CImg/plugins/jpeg_buffer.h ./jpeg_buffer.h
