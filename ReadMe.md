<img src="./docs/Images/pulse_logo_512.png" alt="Pulse Physiology Engine" width="256" height="106">

The Pulse Physiology engine is a C++ based simulation engine for human and animal physiology.
It is intended to provide accurate and consistent physiology data to medical education, research, and training technologies. 
The libraries built (*.dll/*.so) can be integrated with standalone applications, hardware simulators and sensor interfaces, and other physiology models of all fidelities.

All files are released under the Apache 2.0 license

Along with the build instrcutions below, we provide more technical discussions using Pulse on our <a href="https://gitlab.kitware.com/physiology/engine/wikis/home">wiki</a>

For detailed information about our design and methodology read our <a href="https://physiology.kitware.com/">documentation</a>

Use this <a href="https://discourse.kitware.com/c/pulse-physiology-engine">discourse channel</a> 
to ask or share anything and everything about building, using, or understanding the Pulse engine and welcome to our community!!

## Build Environment

The code provided utilizes C++11, here is a list of popular compilers and their initial version to implement all of C++11 :
- GCC 4.8.1 and later
- Clang 3.3 and later
- MSVC 2015 and later

If you have any questions\comments, don't hesitate to email kitware@kitware.com.

While the provided cmake superbuild automatically pulls many libraries it needs to compile, 
you will still need to have the following tools installed (along with your choice of C++ compiler) :

### CMake
Currently, the code requires CMake 3.7 or greater to properly build
Go to the cmake website, `https://cmake.org/download`, and download the appropriate distribution.
Ensure that cmake bin is on your PATH and available in your cmd/bash shell.

Note on Linux systems, make sure to have curl before you build/install cmake for it to behave properly when downloading 3rd party libraries.
~~~~~~~~~~~~~~~~~~~~~~~~
sudo apt-get install curl
~~~~~~~~~~~~~~~~~~~~~~~~

### Java JDK

The test suite and data generate tools used are written in Java.
While there is no dependency on Java when integrating with your application, it is currently required to build/develop/contribute to the code base.

Add a JAVA_HOME environment variable to point to the Java installation and add it to the system PATH.<br>
There are many ways to do this, here is a simple walk through to get you going with a JDK.

#### Windows
- Download the Windows x64 JDK <a href="http://www.oracle.com/technetwork/java/javase/downloads/index.html">here.</a>
- Run the installer.
- Goto your Control Panel->System and click Advanced system settings on the left. <br>
- Click the 'Environment Variables' button and add JAVA_HOME as a new 'System variables'.<br>
- Set the Value to something like: C:\Program Files\Java\jdk1.8.0_121<br>
    - It's a good idea to add the JDK to the system PATH by adding this to the beginning: %JAVA_HOME%/bin;
- Make sure to start a new cmd window.<br>

#### Linux
- You can find where java is by running `update-alternatives --list java <br>
 - If you don't have a Java SDK, I recommend using an installer like Synaptic
 - Search for 'jdk' by name and install the 'openjdk-8-jdk' 
- You can then add the JAVA_HOME variable to a bash shell by typing
    - export JAVA_HOME=(a path listed by a call to updata-alternatives --list java)
    - For example : export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
- You can also add it to your ~/.bash_profile, or related file (.bashrc, .zshrc, .cshrc, setenv.sh), to get the path in all shells

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64'
export JAVA_HOME
PATH="$JAVA_HOME/bin:$PATH"
export PATH
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Building

The build is directed by CMake to ensure it can be built on various platforms. 
The code is build by a CMake 'superbuild', meaning as part of the build, CMake will download any
dependent libraries and compile those before it builds. 
The build is also out of source, meaning the code base is seperated from the build files.
This means you will need two folders, one for the source code and one for the build files.
Generally, I create a single directory to house these two folders.
Here is the quickest way to pull and build via a cmd/bash shell:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
mkdir physiology
cd physiology
git clone https://gitlab.kitware.com/physiology/engine src
mkdir builds
cd builds
# Feel free to make subfolders here, like msvc2017x64 or something
# Generate a make file/msvc solution for the external dependencies
# Note you need to provide cmake the source directory at the end (relative or absolute)
# Run CMake (it will use the system default compiler if you don't provide options or use the CMake GUI)
cmake -DCMAKE_BUILD_TYPE:STRING=Release ../src
# If you want the build to pull the V&V scenarios and baselines run this (or check the PULSE_DOWNLOAD_BASELINES option in the CMake GUI)
# You can always pull these later if you want (See Running and Testing)
cmake -DCMAKE_BUILD_TYPE:STRING=Release -DPULSE_DOWNLOAD_BASELINES:BOOL=ON ../src
# Build the install target/project
# On Linux/OSX/MinGW 
make install 
# For MSVC
# Open the OuterBuild.sln and build the INSTALL project (It will build everything!)
# When the build is complete, MSVC users can close the OuterBuild solution, and open the Pulse.sln located in the Pulse directory.
# Unix based systems should also cd into this directory for building any changes to the Pulse code base
cd Pulse
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Running and Testing

The code base provides a few driver programs to execute the physiology libraries built.

<b> !! NOTE : The 'bin' directory being refered to below is the bin directory in your <build directory>/install/bin NOT the src/bin directory !! </b>

As part of running and testing Pulse, you will need to download the testing scenarios and their baseline result files.
These files are not part of the standard build. 
If you would like to have these files downloaded during the build, enable the PULSE_DOWNLOAD_BASELINES option when configuring the CMake super build

To download the verificaiton scenarios and baseline results run this command from a bash/cmd prompt from <build directory>/install/bin

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
cmake -DTYPE:STRING=updateBaselines -P run.cmake 
# There is also .bat and .sh scripts to make this a little easier
# You can type
run updateBaselines
# or, on Linux
./run.sh updateBaselines
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This script will download the scenarios and baselines associated with your code version and place them in <build directory>/install/bin/verification 

If you would like to manually pull the verification files you can get them <a href="https://data.kitware.com/#collection/59849c788d777f7d33e9c084/folder/598c95d88d777f7d33e9c1fa">here</a>

Note that each folder will be tied to a git hash where it was introduced, you can search a history of the sourc/verificaiton/verification.zip/sha512 to see all baseline commits hashes.

Simply unzip the archive in the <build directory>/install/bin/verification directory

### Scenario Driver

The Scenario Driver is a simple C++ driver that reads a scenario proto text files (*.pba) and creates a CSV file with results.
See <a href="/_scenario_p_b_a_file.html">here</a> for more info.

To run the driver, change directory in your cmd/bash shell to the build/install/bin directory and execute the following :
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
# First ensure the release directory (or debug if that is what you are running) is on your path, so we can see the executable
PATH=%PATH%;./release # for windows
PATH=$PATH:./release # for linux
ScenarioDriver BasicStandard.xml 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<b> .csv and .log files will be placed in the bin directory </b>

If you are going to run the SceanrioDriver through visual studio follow these steps :
- Right-click the ScenarioDriver project and select 'Set as Startup Project'
- Right-click the Project and select settings
- Make sure you are on the appropriate configuration in the Configuration drop down. The one you intend to run
  - You probably want to do this for all configuration in the solution
- Click on 'Configuration Options->Debugging' on the left tree in the properties dialog
- Put the directory to your bin directory as the 'Working Directory'
- Put in the full path (or relative to the bin directory) of the scenario file you wish to run
  
<b> You will also need to set the working directory for the UnitTestDriver if you want to run that through visual studio as well. </b>

### Java Based Testing Suite

The code base also provides a test harness, written in Java, that will process a ./test/config/*.config file in the source tree by doing the following :
- For each line in the config file :
    - Run the ScenarioDriver or UnitTestDriver (depends on the line)
    - Compare the generated csv results to a baseline csv file and report any differences
    - Generate a plot image file for each data column of the csv file over the time of the scenario

To run the test driver, change directory in your cmd/bash shell to the build/install/bin directory and execute the run.cmake in the following way :

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
cmake -DTYPE:STRING=[option] -P run.cmake 
# There is also .bat and .sh scripts to make this a little easier
# You can type
run [option]
# or, on Linux
./run.sh [option]
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Where [option] can be one of the following :
- genData - Generate xml files from data in the ../data/Data.xlxs file (Patients, Substances, etc.)
- genStates - Generate state file for every patient at the point where the patient completed stabilization (Simulation time 0s)
- Any of the testing configurations found in ../test/config without the suffix
  - For Example : DebugRun, CDMUnitTests, ValidationSystems, VerificationScenarios, etc.
  - With exception of the Characterize.config, this is a legacy testing configuration that will not work  
- SystemValidation - Will run the ../test/config/ValidationSystems configuration plus create validation tables from comparing results to ../test/validation
- PatientValidation - Will run the ../test/config/ValidationPatients configuration plus create validation tables from comparing results to ../test/validation

<b>Results will be place in the build/install/bin/test_results directory </b>

#### Configuration Files

The configuration files, referenced above, provide the testing suite direction on how to execute the test harness and what to do with the results.

## Generating Documentation

The physiology engine includes functionality to generate html documentation.  This including in-code documentation, data model design description, and physiology system methodology reports.  The html files are generated using Doxygen and custom Java applications.

To generate the documentation, perform the following steps:
- Install Doxygen, which can be found <a href="http://www.stack.nl/~dimitri/doxygen/download.html">here</a>
- Install Ghostscript, which can be found <a href="https://www.ghostscript.com/download/">here</a>
- Install a LaTeX generator
  - For windows, use MiKTeX, which can be found <a href="https://miktex.org/download">here</a>
- Execute SystemValidation and PatientValidation as described above
- Execute the doxygen tool using build/install/bin/run.cmake in your cmd/bash shell in the following way :

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bash
cmake -DTYPE:STRING=doxygen -P run.cmake 
# There is also .bat and .sh scripts to make this a little easier
# You can type
run doxygen
# or, on Linux
./run.sh doxygen
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Creating the Software Development Kit (SDK)

In the source code tree is an sdk folder.
This folder provides examples of how to use the physiology engine.
During the build and install, all the header and library files needed to integrate the engine into your application are placed into the build/install folder
External applications only need to reference this directory for headers and libraries for their build.

Note, your application will still need to execute within the build/install/bin directory as it has the binaries and data files in it.

There is a CMakeLists.txt in the sdk folder that you can also run to build and run any of the provided HowTo examples.
You can edit the EngineHowTo.cpp to run a particular example, and even code in the HowToSandbox.cpp to run your own engine!


There is also an example of using a Java based interface with an eclipse project you can explore as well.

