del .\runtests.exe; 
del .\debugdata\*.txt; 
@REM del src\DividedRectangles.o;
g++ -c .\src\DividedRectangles.cpp -o .\src\DividedRectangles.o;

g++ -o runtests.exe .\test\runtests.cpp .\src\DividedRectangles.o;
.\runtests.exe