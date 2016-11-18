@echo on
vcredist_x86.exe
mkdir "C:\ProgramData\koguopai"
copy t1.exe "C:\ProgramData\koguopai"
copy liblept168.dll "C:\ProgramData\koguopai"
copy liblept168d.dll "C:\ProgramData\koguopai"
copy libtesseract302.dll "C:\ProgramData\koguopai"
copy libtesseract302d.dll "C:\ProgramData\koguopai"
copy ok.bmp "C:\ProgramData\koguopai"
copy refresh.bmp "C:\ProgramData\koguopai"
copy cancel.bmp "C:\ProgramData\koguopai"
copy t1.chm "C:\ProgramData\koguopai"
pause