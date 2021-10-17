
build: sudoku.c cJSON.c
	gcc sudoku.c cJSON.c -o sudoku -Wall


clean:sudoku
	rm -f sudoku
	

run: sudoku
	./sudoku
	