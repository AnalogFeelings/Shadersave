# :rainbow: Shadersave
[![GitHub issues](https://img.shields.io/github/issues/analogfeelings/shadersave?style=flat-square&logo=github&label=Issues)](https://github.com/AnalogFeelings/shadersave/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/analogfeelings/shadersave?label=Pull%20Requests&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/pulls)
[![GitHub](https://img.shields.io/github/license/analogfeelings/shadersave?label=License&style=flat-square&logo=opensourceinitiative&logoColor=white)](https://github.com/AnalogFeelings/shadersave/blob/master/LICENSE.txt)
[![GitHub commit activity (branch)](https://img.shields.io/github/commit-activity/m/analogfeelings/shadersave/master?label=Commit%20Activity&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/graphs/commit-activity)
[![GitHub Repo stars](https://img.shields.io/github/stars/analogfeelings/shadersave?label=Stargazers&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/stargazers)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109309123442839534?domain=https%3A%2F%2Ftech.lgbt%2F&style=flat-square&logo=mastodon&logoColor=white&label=Follow%20Me!&color=6364ff)](https://tech.lgbt/@analog_feelings)

Shadersave is a Windows screensaver that can run Shadertoy shaders locally under OpenGL 4.3 core, without using web technologies.

This is done by replicating the uniforms shadertoy provides to shaders, and wrapping each shader in a valid GLSL shader.  
You can find more information by reading the source code at [Shader.cpp](https://github.com/AnalogFeelings/Shadersave/blob/master/src/Classes/Shader.cpp).

# :warning: Limitations

- Audio and VR shaders will not be supported.
- Mouse and/or keyboard input wont be supported for obvious reasons.
- Non-image and non-buffer channel inputs won't be supported.
- Cubemap shaders will not be supported.

# :package: Installing

Due to a Windows design flaw, you must place screensavers in **C:\Windows\System32** for them to be recognized by the control panel
applet.

If you don't trust this project to be safe to place in system32, you can read through the code and build it yourself.  
The program won't even have permissions to modify files in system32 unless it runs as admin.

> [!WARNING]
> Some shaders make heavy use of your system's GPU and thus can increase power usage significantly.  
> Please take your power bill into consideration.

# :hammer_and_wrench: Building

To build this, you will need the following:
- Visual Studio 2022.
- The **Desktop development with C++** workload.
- The **Windows SDK**.

You must also place [GLEW](https://glew.sourceforge.net/) and [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) under the **"libs"** folder.

# :beetle: Debugging

Debugging screensavers is kind of a hassle, but it can be done.

1. Open a Win32 window, for example Control Panel.
2. Grab its window handle with Spy++ or some other software.
3. Convert the handle from hexadecimal to decimal.
4. Go into **Shadersave -> Properties -> Debugging**.
5. On **Command Arguments**, write **/p (window handle)** and hit **Apply**.
6. You can now debug the screensaver as long as you don't close the window you opened.

> [!TIP]
> **Renderdoc** can be used this way as well, if you want to debug the graphics side.

To debug the configuration dialog, do the following:

1. Do steps 1 to 4 from above.
2. On **Command Arguments**, write **/c:(window handle)** and hit **Apply**.
3. You can now debug the config dialog.

# :camera: Screenshots

[*pow(The Shining, 2.0)*](https://www.shadertoy.com/view/tlyfDV) by **dean_the_coder**  
![shining](screenshots/shining.png)

[*isovalues 3*](https://www.shadertoy.com/view/ldfczS) by **FabriceNeyret2**  
![isovalues](screenshots/isovalues.png)

[*Cook-Torrance*](https://www.shadertoy.com/view/XsXXDB) by **xbe**  
![torrance](screenshots/raytracer.png)

[*Splitting DNA*](https://www.shadertoy.com/view/4d2cWd) by **BigWIngs**  
![dna](screenshots/dna.png)
