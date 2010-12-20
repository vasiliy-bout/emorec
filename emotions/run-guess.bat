@echo off

rem call :doguess vk dcim jaffe

call :doguess vk jaffe dcim "--start-scale=5"


pause
exit /B 0



:doguess

Release\guess.exe --classes="d:\work\shkod\data\classes.xml" ^
	--model="D:\work\shkod\data\eigen(%~1)-train_on(%~2).out" --pca="d:\work\shkod\data\%~1.eigen"  "d:\work\shkod\data\orig.%~3.idx" %~4

exit /B 0
