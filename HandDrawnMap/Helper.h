#pragma once
#include "Engine.h"
#include "Scene.h"
#include <time.h>
#include <sstream> 
#include<iostream>
#include<windows.h> 

std::string ToUTF8(const std::wstring& value); // std::wstring‚©‚çstd::string‚É•ÏŠ·‚·‚é

std::wstring ToWideString(const std::string& str); // std::string‚©‚çstd::wstring‚É•ÏŠ·‚·‚é