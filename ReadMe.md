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
Extract and copy the folder to `C:\Program Files\`. Make sure the full path to the executable looks like `C:\Program Files\HotCorners\HotCorners.exe`.
Move the provided shortcuts to `C:\Users\YOUR USER NAME\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup`. This will make them run automatically after you restart.

The default parameters assume a screen of 1920x1080 resolution.

## Customization

In the shortcut `Target` property, you can add parameters to customize the buttons and bars. You can create more shortcuts with different parameters to have different buttons.
Illustrative example of some possible command-line parameters:

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=1 --HCType=3 --lclick= --rclick= --help= --image=

would create a Charms bar background.

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=2 --HCType=3 --lclick="::0" --rclick= --help= --image=Start.bmp --immulhy=-0.25

would create a Charms button on the Charms bar to generate WIN key press ("::0" is a hardcoded predefined action).

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=2 --HCType=3 --lclick="::1" --rclick= --help= --image=Show_desktop.bmp --imofsy=-100 --immulhy=-0.25

would create a Charms button on the Charms bar to generate WIN+D key press ("::1" is a hardcoded predefined action).

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=2 --HCType=3 --lclick= --rclick= --help= --image=Date.bmp --imofsx=541 --immulwx=-1 --imofsy=-48 --BGRed=22 --BGGreen=26 --BGBlue=29 --dispcmd="::0"

would create a Charms button associated to the Charms bar to show the date like Windows 8 ("::0" is a hardcoded value, otherwise command to output if not empty).

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=2 --HCType=3 --lclick= --rclick= --help= --image=Date.bmp --imofsx=541 --immulwx=-1 --imofsy=-236 --BGRed=22 --BGGreen=26 --BGBlue=29 --FSize=30 --textx=5 --texty=5 --dispcmd="powershell -Command \"[System.Threading.Thread]::CurrentThread.CurrentCulture = 'en-US' ; Get-Date -Format 'HH:mm ddd dd MMM'\" > temp.txt" --imPeriod=0

would create a Charms button associated to the Charms bar to show the date using the output of a PowerShell command and without refreshing it periodically.

	"C:\Program Files\HotCorners\HotCorners.exe" --HCWType=2 --HCType=3 --blCmdOrSE=0 --lclick="C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Accessories\Notepad.lnk" --brCmdOrSE=1 --rclick="start \"\" cmd /c \"dir %SystemDrive% && pause\"" --help="Notepad" --image=Notepad.bmp --imofsy=-200 --immulhy=-0.25

would create a Charms button on the Charms bar with some custom actions. 

Shortcut `Target` property might not directly allow very long commands, so a workaround can be to create an environment variable (e.g. `HCNOTEPADBUTTON` with `--HCWType=2...`) which contains the parameters and then use it in the shortcut `Target` property (e.g. `"C:\Program Files\HotCorners\HotCorners.exe" %HCNOTEPADBUTTON%`). Ensure also that the `Start in` property of the shortcut is set to the necessary folder if relative paths are used for some parameters.

For `lclick` and `rclick` parameters, it is also possible to specify "::FF", "::FF:FE", "::FF:FE:FD", "::FF:FE:FD:FC" where FF, FE, etc. are the hexadecimal values of keys to press (see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).

Note also that the `image` parameter is reloaded regularly (depends on `imPeriod` parameter), so you can change the image file while the program is running (therefore, it could act somehow like a live tile...).

See the top of https://github.com/lebarsfa/HotCorners/blob/main/HotCorners.cpp and default shortcuts for more information about the possible parameters.

## Rationale

The idea is that the default location of the `Show desktop` button on Windows 10-11 `Taskbar` is small and can be easily missed. In the same time, we may want to limit the number of fixed elements on the `Taskbar` and the `Start menu` button might be used to open a different menu than the default one (e.g. Open-Shell `Start menu`), or possibly we may want to fully hide the `Start menu` button while we still want to access the default `Start menu` and `Show desktop` functionalities.

[//]: # (
This is a multi-line comment.
You can write as many lines as you want in this comment.
Just make sure you keep everything within the parentheses.
)

## Developer information

Built with Visual Studio 2022 with "Desktop development with C++" workload. It is a C++ "Windows Desktop Application" project.
