# Makefile for ECE1387 assignment2 Analytical Placement - Zhiyuan Gu(1004920400)

CC := g++
RM := rm -f

CFLAGS := -I./SuiteSparse/include -L./SuiteSparse/lib -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x
LDFLAGS :=
LDLIBS := -lX11 -lumfpack

OBJFILES := main.o Placing.o Block.o Net.o draw.o graphics.o
TARGET := AP

all:
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o main.o main.cpp
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o Placing.o Placing.cpp
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o Net.o Net.cpp
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o Block.o Block.cpp
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o draw.o draw.cpp
	$(CC) -I./SuiteSparse/include $(CFLAGS) -o graphics.o graphics.cpp

	$(CC) -L./SuiteSparse/lib -o $(TARGET) $(OBJFILES) $(LDLIBS)

clean:
	$(RM) $(OBJFILES) $(TARGET)
