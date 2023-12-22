FROM node:20.10
LABEL maintainer="ZhuangYumin <zhuangyumin@sjtu.edu.cn>"
COPY . /bookstore/
RUN apt-get update && apt-get install -y g++ && apt-get install -y cmake
WORKDIR /bookstore/
RUN mkdir data
RUN mkdir build && cd build && cmake -B . -S .. && cmake --build . && cd ../frontend/Web && npm install
WORKDIR /bookstore/frontend/Web
EXPOSE 3000
CMD ["node","index.js","/bookstore/data/"]