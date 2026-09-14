# Conway's Game of Life — Parallel Edition

A new implementation of **Conway's Game of Life**, rebuilt from one of my previous C++ projects.

The goal of this version is to take the original simulation further by introducing a graphical interface with **Raylib** and experimenting with **parallelism** to improve simulation performance.

## About

Conway's Game of Life is a cellular automaton created by mathematician **John Conway**.

The simulation consists of a grid of cells, where each cell can either be alive or dead. The state of each cell evolves at every generation according to a simple set of rules based on its neighbors.

Despite its simple rules, the Game of Life can produce surprisingly complex behavior.

## From the Original Project

This project is a continuation of an older Game of Life implementation I developed in **C++**.

The original version focused mainly on implementing the simulation itself. This new version aims to improve the project in several areas:

* Better visualization
* Interactive controls
* Improved performance
* Parallel simulation
* Cleaner project architecture

The purpose is also to explore how parallelism can be applied to a problem where each generation can be computed from the previous one.

## Features

* 🧬 Conway's Game of Life simulation
* 🎮 Interactive graphical interface
* 🖥️ Rendering with **Raylib**
* ⚡ Parallel computation of generations
* ▶️ Start / pause / resume simulation
* 🔄 Reset and generate new grids
* 🎨 Visual representation of living and dead cells
* 📊 Performance experimentation and comparison

## Conway's Rules

Each cell interacts with its eight neighboring cells.

The rules are:

1. **Underpopulation**

   A living cell with fewer than two neighbors dies.

2. **Survival**

   A living cell with two or three neighbors survives.

3. **Overpopulation**

   A living cell with more than three neighbors dies.

4. **Reproduction**

   A dead cell with exactly three neighbors becomes alive.

These rules are applied simultaneously to every cell to produce the next generation.

## Technologies

* **C++**
* **Raylib**
* **Multithreading / Parallelism**
* **Makefile**

## Building

### Requirements

* C++ compiler supporting C++17 or later
* Make
* Raylib

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
cd YOUR_REPOSITORY
```

Build the project:

```bash
make
```

Then run the generated executable:

```bash
./conway
```

The exact executable name may depend on the Makefile configuration.

## Previous Version

This project is based on an earlier implementation of Conway's Game of Life that I developed in C++.

The new version is intended to serve both as an improved implementation and as an opportunity to explore **graphics programming and parallel computing**.