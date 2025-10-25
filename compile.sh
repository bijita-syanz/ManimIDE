#this scriopt is just for compiling 

echo "strat compiling !"
g++ -std=c++11 -fPIC $(pkg-config --cflags Qt5Widgets) app.cpp -o ManimIDE $(pkg-config --libs Qt5Widgets)