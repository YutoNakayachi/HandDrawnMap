#pragma once
#include "Engine.h"
#include "Scene.h"
#include <time.h>
#include <sstream> 
#include<iostream>
#include<windows.h> 

std::string ToUTF8(const std::wstring& value); // std::wstring����std::string�ɕϊ�����

std::wstring ToWideString(const std::string& str); // std::string����std::wstring�ɕϊ�����