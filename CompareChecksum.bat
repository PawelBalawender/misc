REM This module takes as an input a filename, ex. .iso and a file
REM with a corresponding .md5 sum and checks them if they match
@ECHO OFF
@SETLOCAL EnableDelayedExpansion

REM no args: print help
IF "%1"=="" (GOTO :no_args) ELSE (GOTO :main)

:no_args
ECHO Usage: md5_compare.bat [File to calculate md5 sum] [File .md5 with the correct md5 sum]
GOTO :EOF

:main
SET calculated=""
SET correct=""

REM if there's only 1 arg: assume that the .md5 file has the same name
REM as the first one, but with trailing .md5 extension
IF [%2]==[] (SET sum_file=%1.md5) ELSE (SET sum_file=%2)

REM calculate md5 checksum of a file and get the output's second line
FOR /F %%i IN ('certutil -hashfile "%1" md5 ^| findstr /V ":"') DO SET calculated=%%i
REM TYPE is like CAT in Unix
FOR /F %%i IN ('TYPE %sum_file%') DO SET correct=%%i

IF %calculated%=="" GOTO :EOF

IF %calculated%==%correct% (ECHO Hash sums match correctly) ELSE (^
ECHO Hash sums do not match
ECHO 1: Hash sum of file %1
ECHO 2. Hash sum from %sum_file%
ECHO 1. %calculated%
ECHO 2. %correct%)

@ECHO ON