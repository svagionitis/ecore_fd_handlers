ecore_fd_handlers
=================

It is a sample application that explores the behavior of monitoring file descriptors, here stdin, using ecore fd handlres. This example is based on the example in http://docs.enlightenment.org/auto/ecore/ecore_fd_handler_example_8c-example.html

Install ecore libraries
=======================

If you don't have installed the ecore libraries (libecore1 - Core abstraction layer for enlightenment DR 0.17) you can install them in Ubuntu using the following command

`sudo apt-get install libecore1`

This will install the dependecy libraries as well. You have to install the development package as well (libecore-dev - Ecore headers and static libraries)

`sudo apt-get install libecore-dev`

Compile the .c file
===================

Use the following command to compile this file

`gcc -I/usr/include/ecore-1/ -I/usr/include/eina-1/ -I/usr/include/eina-1/eina/ -o ecore_fd ecore_fd.c -lecore`

In my box, the include files are located in /usr/include, in your box you might have a different location.
