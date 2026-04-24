pacvirus: main.c file_arrays/all_arrays.h lib-utils
	gcc main.c utils.a -o free-vbucks

lib-utils: utils/utils.h utils/utils.c
	gcc -c utils/utils.c
	ar rcs utils.a utils.o
	rm utils.o

.PHONY: clean
clean:
	rm free-vbucks utils.a
