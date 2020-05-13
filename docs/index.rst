.. Cultsim documentation master file, created by
   sphinx-quickstart on Fri Jan 24 22:53:46 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. Role Definitions

.. role:: lua(code)
   :language: lua

.. role:: cpp(code)
   :language cpp

Welcome to Cultsim's documentation!
===================================

Look at that, you found your way to CultSim! Really nice. Now, let's get started reading some docs so you can get up to speed and become
a simulation God!

.. toctree::
   :maxdepth: 2
   :caption: Contents:

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

Lua Documentation
=================

Welcome to the CultSim Lua API user guide. In the following sections you will be introduced to the Lua API in
CultSim by creating your first very simple scenario. Hopefully, along the way, you'll pick up some skills that
are needed to develop your own scenarios in the future, and getting the most out of CultSim!

A sample simulation
-------------------

This introductory tutorial style text will introduce you to CultSim by creating a basic scenario. It will not go in depth
on every single topic, but instead focus on diving head first into the tool so you have a baseline from which you can create
more interesting scenarios, or simply get a feel for the framework.

.. note:: This section assumes that you already have CultSim installed and ready to run. If you have been able to run the default scenarios that are bundled, you are good to go!

To begin, we will create a very simple scenario using the default assets available to you in CultSim. First, let's
create a new folder in the user data directory which is:

:code:`%appdata%\g107\cultsim` on Windows or :code:`.local/share/cultsim` on Linux. This is the folder where you'll be doing all user
modifications and customizations. You can also mod existing scenarios here, or change the preferences and key bindings.

So, the scenario we are going to create will be something of the following:

 - We will have blobs that are thirsty
 - The blobs should be looking for water
 - When they find water they should drink it
 - Whenever a blob drinks water, it shall split into two blobs

Then we can see how many blobs our environment can handle based on the spawn rate of water, and the number of starting blobs, but
also how quick they get hungry. The blobs can be thought of as cells that need some energy source in order to clone themselves if
you want a real world connection to these rules.

For this scenario, we are going to use the old action system to make agents go looking for water, which is bundled as part of the
native tools (written in C++) and systems shipping with CultSim.


Lua API Reference
-----------------

In this section you'll find a complete reference over all the tools available to you in the CultSim Lua API.

C++ API Reference
=================

This section contains an API dump of all types and methods documented in the C++ implementation of CultSim, and is not
recommeneded to read by people mainly interested in the Lua API and scripting as it will not give you very much. If
you are very interested in the inner workings of CultSim, you may of course read it, or check out the source code directly.

The following is a dump of everything in the CultSim (cs) namespace, good luck:

.. doxygennamespace:: cs
    :members:

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
