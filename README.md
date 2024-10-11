# Snout AI in Background
This program acts as a background process for windows to interact with ai models in 3 main ways.

This project was made for fun as a way to learn about the windows API and get more fond of making c++ applications without an object oriented approach. Unfortunatly, it is not a cross-platform program as for many things i.e. screenshots & clipboard interaction, I could not find a cross-platform solution to these.

I made this project as having ai in the world is becoming more and more common, and I wanted to be able to interact with it in a more efficient way I am familiar with.

### 1. Clipboard interaction
When  ``` ` ``` is pressed shortly, the program will copy the text from the clipboard and send it off to gemini for processing. The result will be pasted back into the clipboard.

### 2. Keystroke interaction
When ``` shift + ` ``` is pressed, the program will start recording keystrokes. When ``` shift + ` ``` is pressed again, the program will stop recording and send the keystrokes off to gemini for processing. The result will be pasted into the clipboard.

### 3. Screenshot + OCR interaction
When ``` ` ``` is pressed down, the programm will log your cursors position until ``` ` ``` is released. The area your cursor traversed will be captured as a screenshot and text will be grabbed via OCR with tesseract. The result will be pasted into the clipboard.

## Status codes
### Colours are assosiated with status codes, which appear in system tray
* ``` green ``` - The program is running and ready to interact with ai models
* ``` red ``` - An error has occured, the program will not work as expected
* ``` yellow ``` - Awaiting a response from gemini
* ``` cyan ``` - Processing a screenshot (very fast)
* ``` blue ``` - Processing a screenshot (slow)
* ``` magenta ``` - Recording keystrokes

## Dependencies
In order to build and run the program, I used MSVC along with VCPKG on a 64-bit windows operating system. The following dependencies are required:
* [VCPKG](https://github.com/microsoft/vcpkg) - Package manager of choice for C++
* [Tesseract](https://github.com/tesseract-ocr/tesseract) - OCR engine
* [OpenCV](https://github.com/opencv/opencv) - Computer vision library, used for image storing and processing
* [Nlohmann/JSON](https://github.com/nlohmann/json) - JSON library, used to deal with the response from gemini
* [Glm](https://github.com/g-truc/glm) - OpenGL mathematics library, although this isn't a OpenGL project, I've grown fond of using it
* [Curl](https://github.com/curl/curl) - Used to send requests to gemini

These should be the right commands to install libraries with vcpkg, although I havn't checked enough:
```bash 
vcpkg install opencv
vcpkg install tesseract
vcpkg install nlohmann-json
vcpkg install glm
vcpkg install curl
```