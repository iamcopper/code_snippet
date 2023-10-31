CFLAGS = -g -Wall
OBJS = *.c 
LIBS = -lpthread -lm
CXX = aarch64-linux-gnu-gcc
all:gpio_test_demo
gpio_test_demo: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o $@ $(LIBS) 
clean:
	rm gpio_test_demo
