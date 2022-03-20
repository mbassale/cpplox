
OBJS = main.o chunk.o debug.o value.o

cpplox: $(OBJS)
	clang++ -std=c++17 $(OBJS) -g -o cpplox

%.o: %.cpp %.h
	clang++ -std=c++17 -g -c $< -o $@

%.o: %.cpp
	clang++ -std=c++17 -g -c $< -o $@

clean:
	rm -f $(OBJS) cpplox