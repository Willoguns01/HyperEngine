#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <list>
#include <mutex>
#include <chrono>
#include <functional>

#include <daxa/daxa.hpp>
#include <daxa/utils/task_graph.hpp>
#include <daxa/utils/imgui.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <GLFW/glfw3.h>
// this needs defines depending on platform, TODO
//#include <GLFW/glfw3native.h>

#include <imgui_impl_glfw.h>
#include <imgui.h>

#include <Engine/Logger.hpp>
#include <Engine/Window.hpp>
