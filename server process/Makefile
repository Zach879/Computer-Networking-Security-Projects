all: server

server: project6.py
	cp project6.py server
	chmod u+x server

.PHONY: submit clean

submit:
	~PROFESSOR_NAME_REDACTED/bin/submit csc328 project6

clean:
	rm -rf server
