all:
	g++ -g project4.cpp -o project4
.PHONY: submit clean

program_NAME := project4
memcheck: $(program_NAME)
	valgrind  --show-leak-kinds=all --tool=memcheck --gen-suppressions=all --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes ./$(program_NAME)

submit:
	~PROFESSOR_NAME_REDACTED/bin/submit csc328 project4
	
clean:
	rm-rf project4
