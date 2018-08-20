CC="gcc"

EXE="scan"

scan: scan.c
	$(CC) scan.c -o $(EXE)

clean:
	rm $(EXE)
