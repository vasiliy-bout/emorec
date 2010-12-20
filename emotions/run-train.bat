@echo off

call :dotrain dcim jaffe
call :dotrain dcim vk
call :dotrain dcim yale

call :dotrain jaffe dcim
call :dotrain jaffe	vk
call :dotrain jaffe yale

call :dotrain vk dcim
call :dotrain vk jaffe
call :dotrain vk yale

call :dotrain yale dcim
call :dotrain yale jaffe
call :dotrain yale vk

pause
exit /B 0


:dotrain

Release\train.exe --classes="d:\work\shkod\data\classes.xml" ^
	--out="D:\work\shkod\data\eigen(%~1)-train_on(%~2).out" --pca="d:\work\shkod\data\%~1.eigen"   "d:\work\shkod\data\faces.%~2.idx"

exit /B 0
