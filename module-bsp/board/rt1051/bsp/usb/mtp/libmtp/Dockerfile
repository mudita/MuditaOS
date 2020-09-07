# c-template project (https://gitlab.com/arturmadrzak/c-template)
# Copyright (c) 2020 Artur Mądrzak <artur@madrzak.eu>

FROM  gcc

RUN apt-get update --yes && \
    apt-get install --yes cmake git
    
RUN git clone --depth=1 https://github.com/cgreen-devs/cgreen && \
    cd cgreen && \
    mkdir build && cd build && \
    cmake .. && \
    make && make install
    
RUN apt-get remove --yes cmake git
