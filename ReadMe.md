# Hot Corners

Display customizable buttons or bars when moving the mouse to the corners of the screen, inspired from Windows 8 way to access the `Start screen` and its `Charms bar`.

<p float="left">
  <img src="./Screenshots/Button.png" width="400" alt="Button" />
  <img src="./Screenshots/CharmsBarWith2CharmsButtons.png" width="400" alt="Charms bar with 2 Charms buttons" />
</p>

By default, the button is displayed in the top-left corner of the screen and a left-click will show `Start menu` while a right-click will act as `Show desktop` function.
Another alternative is to display a `Charms bar` from the bottom-right corner.
Touch screens are not yet supported.

## Installation

Download: see https://github.com/lebarsfa/HotCorners/releases.
Extract and copy the folder to `C:\Program Files\`.
Move the provided shortcuts to `C:\Users\YOUR USER NAME\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup`. This will make them run automatically after you restart.

## Customization

In the shortcut properties, you can add parameters to customize the buttons and bars. You can create more shortcuts with different parameters to have different buttons.
Illustrative example of some possible command-line parameters:

	"C:\Program Files\HotCorners\HotCorners.exe" --HotCornerWindowType=1 --HotCornerType=3 --lclick= --rclick= --help= --image=

would create a Charms bar background.

	"C:\Program Files\HotCorners\HotCorners.exe" --HotCornerWindowType=2 --HotCornerType=3 --lclick="::0" --rclick= --help= --image=Start.bmp --offset_y=-300

would create a Charms button on the Charms bar to generate WIN key press ("::0" is a hardcoded predefined action).

	"C:\Program Files\HotCorners\HotCorners.exe" --HotCornerWindowType=2 --HotCornerType=3 --lclick="::1" --rclick= --help= --image=Show_desktop.bmp --offset_y=-414

would create a Charms button on the Charms bar to generate WIN+D key press ("::1" is a hardcoded predefined action).

	"C:\Program Files\HotCorners\HotCorners.exe" --HotCornerWindowType=2 --HotCornerType=3 --blCmdOrShellExecute=0 --lclick="C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Accessories\Notepad.lnk" --brCmdOrShellExecute=1 --rclick="start \"\" cmd /c \"dir %SystemDrive% && pause\"" --help="Notepad" --image=Notepad.bmp --offset_y=-528

would create a Charms button on the Charms bar with some custom actions.

See the top of https://github.com/lebarsfa/HotCorners/blob/main/HotCorners.cpp for more information about the possible parameters.

## Rationale

The idea is that the default location of the `Show desktop` button on Windows 10-11 `Taskbar` is small and can be easily missed. In the same time, we may want to limit the number of fixed elements on the `Taskbar` and the `Start menu` button might be used to open a different menu than the default one (e.g. Open-Shell `Start menu`), or possibly we may want to fully hide the `Start menu` button while we still want to access the default `Start menu` and `Show desktop` functionalities.

[//]: # (
This is a multi-line comment.
You can write as many lines as you want in this comment.
Just make sure you keep everything within the parentheses.
)

## Developer information

Built with Visual Studio 2022 with "Desktop development with C++" workload. It is a C++ "Windows Desktop Application" project.
