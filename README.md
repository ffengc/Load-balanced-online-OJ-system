# Load-balanced-online-OJ-system

<a href="https://github.com/ffengc">
    <img src="https://img.shields.io/static/v1?label=Github&message=ffengc&color=blue" alt="ffengc.github">
</a>
<a href="https://ffengc.github.io">
    <img src="https://img.shields.io/static/v1?label=Page&message=ffengc.github.io&color=red" alt="ffengc.github.io">
</a>
<a href="https://ffengc.github.io/gh-blog/">
    <img src="https://img.shields.io/static/v1?label=Blog&message=Blog Page&color=brightgreen" alt="Mutable.ai Auto Wiki">
</a>


- [简体中文](./README-CN.md)
- [English](./README.md)

> [!NOTE]
> If you have any questions about the operation and implementation of this project, or have better optimization suggestions for this project, you can contact me directly or leave a message in the issue of the repository.

- [Load-balanced-online-OJ-system](#load-balanced-online-oj-system)
  - [Basic information of the project](#basic-information-of-the-project)
  - [Project module architecture](#project-module-architecture)
  - [Project operation effect](#project-operation-effect)
  - [Configuration and Run](#configuration-and-run)
    - [Install Boost Library](#install-boost-library)
    - [Install Ctemplate](#install-ctemplate)
    - [Clone the repository and run the project](#clone-the-repository-and-run-the-project)
    - [Configure cr host](#configure-cr-host)
    - [Modify/adjust the topic](#modifyadjust-the-topic)
  - [Project implementation details](#project-implementation-details)


## Basic information of the project

**Project name:** Self-made online programming judging system (Online Judge)

**Project description:** This project aims to create an online programming judging system similar to LeetCode, allowing users to write code in the browser, submit and evaluate the correctness and performance of their code in real time. The system supports multiple programming languages ​​and can provide instant feedback and detailed error analysis.

**Core functions:**
- Question browsing and coding: Users can browse the questions in the question bank, each question has a corresponding description, example and initial code framework.
- Code editing and real-time evaluation: Use Ace editor to edit code in the browser, supporting basic syntax highlighting and code completion.
- Code submission and judging: The code submitted by the user will be sent to the server for compilation and execution, and the server will evaluate the correctness and efficiency of the code according to the preset test cases.
- Load balancing: Use a custom load balancing algorithm to intelligently select the server with the lowest load for code compilation and execution to optimize resource usage and response time.
- Result feedback: Users can receive detailed execution result feedback, including execution output, error information and performance indicators.

**Technology stack:**

- Front-end: HTML, CSS, JavaScript (using Ace editor)
- Back-end: C++, Boost library, JSON for Modern C++, ctemplate (for HTML template rendering)
- Server communication: Use cpp-httplib library to handle HTTP requests
- Data storage: Questions and user codes are stored in the file system
- Load balancing: Self-implemented load balancing algorithm, intelligently distributes requests based on the current active server load

**Project highlights:**
- Self-developed load balancing system: Utilizes simple load monitoring and dynamic server selection mechanism to optimize system resource allocation and processing speed.
- Fully customized coding environment: The integrated Ace code editor provides a coding experience similar to desktop IDE, including syntax highlighting, code folding and automatic completion.
- Modular design: The project is highly modularized, with clear separation of front-end display, back-end logic processing and data management, which is easy to maintain and expand.
- Extensive technical application: Combining the advanced features of modern C++ and multiple open source libraries, it demonstrates the integrated use of cross-language and frameworks in practical applications.


## Project module architecture

![](./README.figs/10.png)

## Project operation effect

![](./README.figs/1.png)
![](./README.figs/2.png)
![](./README.figs/3.png)
![](./README.figs/4.png)
![](./README.figs/5.png)

## Configuration and Run

### Install Boost Library

**Installation on Ubuntu:**

Open the terminal.
Type the following command to update the system's package index:
```
sudo apt update
```
安装 Boost 库：
```
sudo apt install libboost-all-dev
```

**Installation on CentOS:**

Open a terminal.
First install the EPEL repository to ensure all dependencies can be found:
```
sudo yum install epel-release
```
Install the Boost library:
```
sudo yum install boost-devel
```

**Installation on CentOS:**

1. Open a terminal.
2. Make sure the EPEL repository is installed, then install JsonCpp:
```
sudo yum install jsoncpp-devel
```

### Install Ctemplate

**Installation on Ubuntu:**

Open a terminal.
Update the system's package index:
```
sudo apt update
```
Installation of Ctemplate:
```
sudo apt install libctemplate-dev
```

**Installation on CentOS:**

Open a terminal.
Install Ctemplate:
```
sudo yum install ctemplate
```

### Clone the repository and run the project

```sh
git clone https://github.com/ffengc/Load-balanced-online-OJ-system.git;
cd Load-balanced-online-OJ-system/online_judge;
make clean;make;
```

Set the environment variables:
```sh
export LD\_LIBRARY\_PATH=$LD\_LIBRARY\_PATH:/usr/local/lib # set the env
```

run `oj_server`:

```sh
cd oj_server;
./oj_server
```

run `compile_server`:

```sh
cd compile_server;
./compile_server 8081
```

### Configure cr host

Configuration Files: `online_judge/oj_server/conf/service_machine.conf`

```conf
127.0.0.1:8081
127.0.0.1:8082
127.0.0.1:8083
```

### Modify/adjust the topic

dir: `online_judge/oj_server/questions`

Just configure it according to the format in the directory.

## Project implementation details

- **[work.md](./Work.md)** (Chinese only)