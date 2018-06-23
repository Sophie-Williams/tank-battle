#pragma once

#ifdef _WIN32
#ifdef EXPORT_GAME_CONTROLLER
#define GAME_CONTROLLER_API __declspec(dllexport)
#else
#define GAME_CONTROLLER_API __declspec(dllimport)
#endif // EXPORT_GAME_CONTROLLER
#else
#define GAME_CONTROLLER_API
#endif // _WIN32