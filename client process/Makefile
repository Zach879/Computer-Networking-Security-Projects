all: client

client: project5.py
	cp project5.py client
	chmod u+x client


.PHONY: submit clean
submit:
	~schwesin/bin/submit csc328 project5

clean:
	rm -rf client
