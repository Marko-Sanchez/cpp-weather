# cpp-weather ![networking](https://img.shields.io/badge/networking-cpp--httplib-orange) ![GUI](https://img.shields.io/badge/GUI-raylib-lightgrey)
A C++ GUI desktop application showing weather forecast. Designed to be cross-platform.

## Details
Applications uses c++-23 and cmake version (2.25.0)+. Using open-source Weather forecast API [Open-Meteo](https://open-meteo.com/)
uses: user provided location, location based on IP, or defaults location to Redmond, Washington. Once data is received it is parsed
and displayed; either on terminal or via the GUI. User argument '--no-gui' displays the current days hourly forecast to the terminal,
default application behavior displays current days hourly and next four days average forecast to GUI.
