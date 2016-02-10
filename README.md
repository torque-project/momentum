# Momentum

A C++ port of the Clojure data structures and core functions


# Introduction

This library provides a replacement for most of the data structures found in 
the C++ standard library and a set of functions to work on them. 
While the data structures are not intended as a drop in replacement 
(since they mimic the Clojure API), the library contains a compatability layer, 
so that STL data structures can be used with Momentum's algorithms and vice versa.

The core intention of the library is to bring a set of immutable data structures
and functional algorithms to C++.

# Getting Started

The libraries are header only and don't need to be built. You can either check 
out this repository in your source tree, or download the project's tarball.

If you would like to run the unit tests, make sure to check out this repository
with

    git checkout --recursive https://github.com/jkrueger/momentum

This will fetch the project's build files in addition to the library headers.
Simply running make in the project root will build all test binaries. The
output location for the build is the **target/** directory.

# Documentation

The project is document via a set of tutorials in the project wiki as well as
an API documentation.

* [Momentum Wiki](https://github.com/jkrueger/momentum/wiki)
* [API Docs](http://jkrueger.github.io/momentum)

Otherwise the Clojure documentation is a good resource, since the API tries
to be as close to Clojure as possible.

* [Clojure Documentation](http://clojure.github.io/clojure/)


# Contributing

If you would like to contribute a fix or additional features, the easiest way to 
do so is to create a pull request against the development branch of this repository.

Issues can be posted in the issue tracker of this repository.


# Roadmap

The following items are currently under development and will be added soon.

* Transient data structures.
* Sets
* Lazy Sequences
