all:
	gcc -o project3 project3.c -pthread -g

.PHONY: submit clean

program_NAME := project3
memcheck: $(program_NAME)
	valgrind  --show-leak-kinds=all --tool=memcheck --gen-suppressions=all --leak-check=full --leak-resolution=med --track-origins=yes ./$(program_NAME) 8 8 8 8

submit:
	~PROFESSOR_NAME_REDACTED/bin/submit csc328 project3

clean:
	rm -rf project3
	rm -rf *.txt
