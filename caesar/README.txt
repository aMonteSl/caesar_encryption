Compilar y ejecutar caesar.c:
    gcc -Wall -Wshadow -Wvla -g -c caesar.c
    gcc -g -o caesar caesar.o
    ./caesar [key] < [inputfile] > [outputfile]

Compilar breakcaesar.c:
    gcc -Wall -Wshadow -Wvla -g -c breakcaesar.c
    gcc -g -o breakcaesar breakcaesar.o -lm
    ./breakcaesar < [inputfile]