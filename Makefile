CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc -I/opt/homebrew/include -I/usr/local/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lglfw -lGLEW -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

SRCS = src/main.cpp \
       src/Window/Window.cpp \
       src/Shader/Shader.cpp \
       src/Mesh/Mesh.cpp \
       src/Camera/Camera.cpp \
       src/Terrain/Terrain.cpp \
       src/Texture/Texture.cpp \
       src/Lake/Lake.cpp \
       src/Vendor/stb_impl.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = mountain-lake-aurora

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
