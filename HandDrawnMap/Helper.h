#pragma once
#include "Engine.h"
#include "Scene.h"
#include <time.h>
#include <sstream> 
#include<iostream>
#include<windows.h> 

std::string ToUTF8(const std::wstring& value); // std::wstringからstd::stringに変換する

std::wstring ToWideString(const std::string& str); // std::stringからstd::wstringに変換する