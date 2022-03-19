
OBJS = main.o chunk.o

cpplox: $(OBJS)
	clang++ $(OBJS) -g -o cpplox

%.o: %.cpp %.h
	clang++ -g -c $< -o $@

%.o: %.cpp
	clang++ -g -c $< -o $@

clean:
	rm $(OBJS) cpplox