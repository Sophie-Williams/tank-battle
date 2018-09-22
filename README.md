# Tank battle
A tank game allow users write their own tank bots using C++ or Cλ language then run them as players on a battle.
![](https://github.com/VincentPT/ffscript/blob/master/doc/images/best-practice.gif)

## Purpose
This is not just a game project, I've created it with two main objectives.
* Demonstrate Cλ language (my created language) in a real project.
* Challenge myself to build a game engine and use it in a real project before explore the game industry.

## Dependencies.
* Cinder - Graphics library.
* ImGui - UI library.
* FFscript - Cλ library.

## Compile.
1. Download ffscript project version 1.0 in bellow link.  
   https://github.com/VincentPT/ffscript/releases
2. Compile ffscript project.
3. Run configure command
   go to folder ./src/GameControllers/ScriptedPlayer/cmake/ffscript of the game project run following command.
   ```
   cmake.exe -DFFSCRIPT_ROOT=<ffscript downloaded folder> .
   ```
4. Build the project.   
   goto root folder of project then run following commands
   ```
   cd build
   cmake -G "Visual Studio 14 2015 Win64" ../src -DCINDER_INCLUDE_DIR=<cinder headers folder> -DCINDER_LIB_DIR=<cinder prebuilt folder>
   cmake --build . --target INSTALL --config Release
   ```

# Feedback
 The objective of this project is sharing knowledge and contributing a litle of effort to open source community. As a contributor, I am happy if someone use my contribution in their products.  
 So, if you have any issue, any ideal or even a greeting message you can contact me via Github issue management system or my E-mail: minhpta@outlook.com.  
 They will be a motivation source help me complete my work.

## License.
The project is distributed under MIT license.
For more details, check LICENSE file.
