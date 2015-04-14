SOURCE = src/

INCLUDES =  -I/usr/local/include/ -I./headers/

LIBS =	-L/usr/local/lib/ -L../edu.osu.vision/ -lGL -lglut -lGLU -lGLEW


TARGET =	main

#$(TARGET):	$(OBJS)
$(TARGET):	
	$(CXX) -fPIC $(INCLUDES) $(SOURCE)*.cpp  $(LIBS) -o $(TARGET) $(OBJS)
all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
