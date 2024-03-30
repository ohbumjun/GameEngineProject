#pragma once

// Network

#ifdef HZ_PLATFORM_WINDOWS
#endif

// Imgui
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

// Standard
#include <stdlib.h>
#include <stdio.h>

// Network-related variables
#define TEST_SERVER_PORT "9190" // PORT ��ȣ ���ڿ� ����
// #define SERVER_IP_ADDRESS "127.0.0.1"
#define TEST_SERVER_IP_ADDRESS "211.217.168.13" // IP �ּ� ���ڿ� ����

enum class NetworkType
{
	SERVER,
	CLIENT, 
	ECO_SERVER,
	ECO_CLIENT
};
