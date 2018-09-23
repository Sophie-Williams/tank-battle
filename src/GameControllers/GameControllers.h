/******************************************************************
* File:        GameControllers.h
* Description: define common macro that export plugged-in APIs.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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