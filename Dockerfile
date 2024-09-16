FROM debian:latest
RUN apt-get update
RUN apt-get install pkg-config gcc libtool-bin make ruby time libffi-dev libbsd-dev libevent-dev
RUN git clone https://git.kmx.io/kc3-lang/kc3.git
RUN cd kc3
RUN git submodule init
RUN git submodule update
RUN cd fonts
RUN git submodule init
RUN git submodule update
RUN cd ..
RUN ./configure
RUN make
RUN . ./env
RUN make lib_links
RUN make test
