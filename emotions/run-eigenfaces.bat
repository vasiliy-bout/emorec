@echo off

call :doeigen dcim
call :doeigen jaffe
call :doeigen vk
call :doeigen yale

pause
exit /B 0


:doeigen
@Release\eigenfaces.exe --size=80 --out="d:\work\shkod\data\%~1.eigen" d:\work\shkod\data\faces.%~1.idx 
exit /B 0
