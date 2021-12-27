#pragma once

#define _WIN32_DCOM
#define FAILURE -1
#define CELL_SIZE 25

#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <atlstr.h>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <comdef.h>
#include <Wbemidl.h>
#include <map>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "advapi32.lib")

enum DiskAttrTypes {
	D,
	DINT,
	DLONG,
	DSTR,
	DBOOL,
	DDATE
};

//----------------------task3-------------------------
void task3();

//----------------------task4-------------------------
void task4();

//----------------------main--------------------------
constexpr unsigned int hash(const char* s, int off);

