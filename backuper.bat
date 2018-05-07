@ECHO off
SETLOCAL enableDelayedExpansion

REM current working directory (ends with slash)
SET cur_dir=%~dp0
REM directory to make backups to
SET backup_dir=%cur_dir%DailyNewsBackup

SET hour=%time:~,2%
SET min=%time:~3,2%
SET sec=%time:~6,2%

REM creating new dir for current backup with the current time in name
SET new_dir=%backup_dir%\%date% %hour%.%min%.%sec%
IF NOT EXIST "%new_dir%" MKDIR "%new_dir%"

REM looping through argv
FOR %%i IN (%*) DO (
   REM full path to and just the name of the file to copy
   SET full_path=%%i
   SET name_part=%%~nxi
   REM copying file with name in the argv to the freshly created directory
   COPY "!full_path!" "!new_dir!\!name_part!"
)
PAUSE
