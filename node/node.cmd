@echo off
if not exist python\python.exe (
echo Interpreter .\python\python.exe not found!
goto end )
python\python.exe main.py
:end
pause
