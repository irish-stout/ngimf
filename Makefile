run:
	@gcc \
	./src/rws.c \
	./src/rws_utils.c \
	./src/rws_recv.c \
	./src/rws_server.c \
	./src/rws_resp.c \
	&& ./a.out
