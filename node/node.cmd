@echo off
if exist python\python.exe (
set python=python\python.exe
goto run)
set errorlevel=
where python.exe > NUL 2>&1
if errorlevel 1 (
echo Python interpreter not found in '.\python\python.exe' or PATH!
goto end) else (
set python=python.exe
)
:run
%python% main.py
:end
pause
