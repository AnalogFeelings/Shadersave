# :rainbow: Shadersave
[![GitHub issues](https://img.shields.io/github/issues/analogfeelings/shadersave?style=flat-square&logo=github&label=Issues)](https://github.com/AnalogFeelings/shadersave/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/analogfeelings/shadersave?label=Pull%20Requests&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/pulls)
[![GitHub](https://img.shields.io/github/license/analogfeelings/shadersave?label=License&style=flat-square&logo=opensourceinitiative&logoColor=white)](https://github.com/AnalogFeelings/shadersave/blob/master/LICENSE.txt)
[![GitHub commit activity (branch)](https://img.shields.io/github/commit-activity/m/analogfeelings/shadersave/master?label=Commit%20Activity&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/graphs/commit-activity)
[![GitHub Repo stars](https://img.shields.io/github/stars/analogfeelings/shadersave?label=Stargazers&style=flat-square&logo=github)](https://github.com/AnalogFeelings/shadersave/stargazers)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109309123442839534?domain=https%3A%2F%2Ftech.lgbt%2F&style=flat-square&logo=mastodon&logoColor=white&label=Follow%20Me!&color=6364ff)](https://tech.lgbt/@analog_feelings)

Shadersave is a Windows screensaver that can run Shadertoy shaders locally under OpenGL.

> [!WARNING]
> This is still under development and does not have most features implemented.

# :world_map: Roadmap

- [ ] Get buffers and channels working
- [x] Get basic conversion code working.
- [x] Get OpenGL renderer working.
- [ ] Clean up the code.
- [ ] Add support for loading from files instead of resources.
- [ ] Add config dialogue using Win32.
- [ ] Move to meson and make it cross-platform (?).

# :warning: Limitations

- Audio and VR shaders will not be supported.
- Mouse and/or keyboard input wont be supported for obvious reasons.

# :hammer_and_wrench: Building

To build this, you need Visual Studio 2022 with the C++ workload installed alongside the Windows 11 SDK.  
You must also place the libraries below in a "lib" folder.

- [boost::regex](https://github.com/boostorg/regex)
- [GLEW](https://glew.sourceforge.net/)
- [GLM](https://github.com/g-truc/glm)

I still haven't heard of ways to debug screensavers properly.

# :camera: Screenshots

[*Barber*](https://www.shadertoy.com/view/MsjXDm) by **okro**  
![barber](screenshots/barber.png)

[*Cook-Torrance*](https://www.shadertoy.com/view/XsXXDB) by **xbe**  
![torrance](screenshots/raytracer.png)
