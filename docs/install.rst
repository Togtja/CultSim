Installing CultSim
==================

Installing CultSim is very simple, and does not require much effort to get it to run regardless of what system you're on (*except Mac*).
The only thing you need is a computer with a graphics card, and a decent multicore CPU. We recommend at least a **GeForce MX150** or better.
You also need support for OpenGL 4.5, which should not be a problem unless you have a really old computer or no GPU. This obviously means
there is no support for Mac OS, so if you're on Mac, no luck. Windows 10 and Linux is fine, though.

Windows
-------

On Windows, simply download the :code:`.zip` file with CultSim, and ensure you have updated your graphics drivers to the latest version. You
can put unzipped folder wherever you like, and moving it around later is not a problem as CultSim will save all user data in an install location
agnostic folder. If you delete, and re install CultSim later, these config files are still kept around. If running CultSim does not work after 
downloading, you are likely missing the MSVC redistributables, which you can download and install `from here <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`_.
If this doesn't solve it, see if any error messages appear that make sense to you, and hope for the best.

Linux
-----

CultSim should run without issue on Linux, but there might be dependency issues. You can always build from source as long as you have a C++17 compliant compiler.
Internally, we built on GCC 9, so any equivalent should be fine. This is likely to vary depending on your distro, though. Of libraries, you need :code:`pthreads` and :code:`dl`,
the rest is bundled. Yet, if you are on Linux, you should be computer literate enough to figure out why it doesn't compile or run.
