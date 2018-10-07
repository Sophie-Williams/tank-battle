# Tank battle
A tank game allow users write their own tank bots using C++ or Cλ language then run them as players in a battle.
![](https://github.com/VincentPT/ffscript/blob/master/doc/images/best-practice.gif)

## Purpose
This is not just a game project, I've created it with two main objectives.
* Demonstrate Cλ language (my created language) in a real project.
* Challenge myself to build a game engine and use it in a real project beforing explore the game industry.

## Dependencies.
* Cinder - Graphics library.
* ImGui - UI library.
* FFscript - Cλ library.

## Compile.
The project has been updated to new version of Cinder (v0.9.1) and ImGui (v1.65). The compiling progress is quite complex. So, I write some script to download externals, patch to fix compile error and compile the external dependencies.
Following these simple step to compile the project.

1. Download the project from github.
   git clone --recursive https://github.com/VincentPT/tank-battle.git

2. Download external projects.  
   ```
   cd tank-battle\src\external
   downloadScripts.bat
   ```
3. Compile extenal projects.
   run following command in 'external' folder above.
   ```
   build-externals.bat <Configuration>
   ```
   Configuration should be Debug or Release.
   
4. Compile the project.
   run following commands.
   ```
   cd tank-battle\build
   cmake -DCMAKE_GENERATOR_PLATFORM=x64 ../src
   build.bat <Configuration>
   ```
   Configuration should be Debug or Release.
   
# Feedback
 The objective of this project is sharing knowledge and contributing a litle of effort to open source community. As a contributor, I am happy if someone use my contribution in their products.  
 So, if you have any issue, any ideal or even a greeting message you can contact me via Github issue management system or my E-mail: minhpta@outlook.com.  
 They will be a motivation source help me complete my work.

## License.
The project is distributed under MIT license.
For more details, check LICENSE file.
