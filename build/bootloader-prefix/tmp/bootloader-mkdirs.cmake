# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/marcoscapistrano/esp/esp-idf/components/bootloader/subproject"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix/tmp"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix/src/bootloader-stamp"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix/src"
  "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marcoscapistrano/projects/ausyx/qm-test/fmw-torrador-qm-v3/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
