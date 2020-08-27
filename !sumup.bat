@echo off
del !oldsum.txt
copy !sum.txt !oldsum.txt
del !sum.txt
copy src\*.* + include\*.* !sum.txt