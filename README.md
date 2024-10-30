# Drone Collective Intelligence System

A software system aimed at the modeling of self-organizing swarm of drones equipped with 
collective intelligence for area monitoring and tasks performance is planned to promote.

* [Drone Collective Intelligence System Video Link](https://www.youtube.com/watch?v=MJZwPcIx7gw)

## Setup instructions

This project depends on some packages, which need to be installed prior to build and run it.

* [Qt6 libraries installation](https://doc.qt.io/qt-6/get-and-install-qt.html)
* [CMake installation](https://cmake.org/install/)


## Build and run

``` bash
> cd DCIS && mkdir build && cd build
> cmake .. && cmake --build .
> ./core
> ./dcis
```

## Run with Docker

This example shows how to run project with command line interface (Cli) using docker-compose.

``` bash
sudo docker-compose up --build
```
