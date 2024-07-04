CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
IMPL = 1

state_encoder: state_encoder.c
	${CC} ${CFLAGS} -o state_encoder state_encoder.c

state_encoder_soln: state_encoder_soln.c
	${CC} ${CFLAGS} -D IMPLEMENTATION=${IMPL} -o state_encoder_soln state_encoder_soln.c

clean:
	rm -f state_encoder_soln state_encoder 
