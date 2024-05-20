# HTTP-Engine
# An open-source comprehensive version of conversing assembly code to CLang. 

# Example
`MOV R1, R2
ADD R1, 5
SUB R3, R1
MUL R2, 3
DIV R4, 2
`

#  Output
`R1 = R2;
R1 += 5;
R3 -= R1;
R2 *= 3;
R4 /= 2;
`

# Compile
`gcc main.c -o http_engine -lmicrohttpd`


# Requesting
`curl -X POST --data-binary @assembly.txt http://localhost:8888/upload`
