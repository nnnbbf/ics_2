# 使用官方的 Ubuntu 镜像作为基础镜像
FROM ubuntu:20.04

# 设置环境变量（解决时区问题和交互问题）
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai

# 设置工作目录
WORKDIR /app

# 一次性安装所有依赖，设置时区
RUN apt-get update && \
    apt-get install -yq --no-install-recommends \
    build-essential \
    g++ \
    libmysqlclient-dev \
    libssl-dev \
    libpthread-stubs0-dev \
    libjsoncpp-dev \
    git \
    cmake \
    wget \
    curl \
    && ln -fs /usr/share/zoneinfo/${TZ} /etc/localtime \
    && echo ${TZ} > /etc/timezone \
    && dpkg-reconfigure -f noninteractive tzdata \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 复制项目源代码到 Docker 容器中
COPY . /app

# 生成 Makefile 和编译项目
RUN make

# 公开 8082 端口
EXPOSE 8083

# 启动服务
CMD ["./server"]