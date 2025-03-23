# TouchSwitch
 A widget for Windows that enables and disables devices, primarily touch screens.

This is an effort to make my spending on a tablet seems worthwhile, which in turn saves my soul.

One use case of this widget is that if you are like me who bought an ARM64 Windows tablet with a decent oomph however downright shitty touch screen and digitizer (yes you got it right, it is Xiaomi Book 12.4 Inch. Xiaomi again. *\*sigh\**), this time before you yet again try to let the stylus reach the screen before your hand does but still fail miserably due to the exceptionally short detection range and unproportionally long detection latency of the digitizer which of course renders the palm rejection feature pratically useless, you could touch this widget to disable the touch screen (it turns red!), write with the stylus, then poke the widget with the stylus till it turns green again to re-enable the touch screen.

Update: I have swapped it for a Samsung. What a breeze.

----------------

This is literally code shitposting. I mean who tf uses MFC in 2025?

It's not like I'm familiar with it or what, I learned everything on the go. (albeit to a limited extent)

The sole reason I chose to use MFC is that it's fast and small in size. (that's what ur mom said)

Which is also intrinsic to C++ programs. (Apparently when you apply this criteria, Qt is no longer written in C++.)

----------------

TODO: (it's not like I'm gonna do it anyway)
- Implement "Start with Windows" button in settings (which adds an entry in Task Scheduler to run TouchSwitch as Administrator)
- Fix flickering when resizing
- Fix incorrect widget position and size restoration upon launch (desktop scaling related MFC issue?)
- General code polishing
