@echo off
if not exist pyton\python.exe (
echo ./python/python.exe not found!
goto end )
python\python.exe main.py
:end
pause
