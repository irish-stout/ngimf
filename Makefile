run:
	@gcc \
	./src/ngimf.c \
	./src/ngf_utils.c \
	./src/ngf_recv.c \
	./src/ngf_server.c \
	./src/ngf_resp.c \
	&& ./a.out
