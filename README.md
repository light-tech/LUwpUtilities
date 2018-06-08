Lightech UWP Utilities
======================

This is our essential library to facilitate developments of UWP applications.

Problems and Goal
-----------------

  1. As developers, we are all dreaded of Visual Studio: super heavy (23GB for UWP development workload) and super bloated IDE by Microsoft.
But that is not all.
Visual Studio consumes a huge amount of RAM (more than 500MB) at runtime as well.
Our development machine (Dell duo-core with 2GB RAM) run into

    Compiler out of heap space.

error all the time we makes use of heavy template library such as `collection.h` or `ppltasks.h`.
Note also that each instance of the template method `create_task` (almost always) adds 1.5MB to the object file.

Our **LUwpUtilities** is an attempt to fix all of those inefficiency.
UWP development is now a breeze on our limited Dell tablet.

  2. The use of XAML in XML also constantly lead to
  
    Compiler out of heap space.

Thus, there is a need to build user interface from code instead of XML. The problem is that the code behind generated by XAML compiler is very complicated to compile.

The object files from XAML auto-generated code is also very heavy. LawTeX's MainPage, for instance, originally cost 15MB.

XAML XML is also undesirable due to the fact that it cascades recompilation. For example, changing the `.xaml` file leads to changing all auto-generated code `*.g.h`, leading to recompilation of your code `*.xaml.cpp` as well as dependency such as `App.xaml.cpp`.

  3. The lack of a viable Settings framework.

Due to its design, there is no need for pre-compiled header (which costs 150MB in big projects like LawTeX).

The use of this library reduces the compiled object files of LawTeX from ~60MB to 9.34MB. (And that is not to count its dependency library for TeX compilation and the code editor.)

The Idea and Design
-------------------

LUwpUtilities consists of a collection of header-only files that provides commonly-used static methods so that one does not have to include heavy headers `ppltasks.h` and `collection.h`. In particular,

 * `CollectionHelper.h` provides `MakeIntVector`, `MakeStringVector` and `MakeObjectVector` that produces empty `IVector<int>^`, `IVector<Platform::String^>^` and `IVector<Platform::Object^>^`

 * `TaskHelper.h` provides various `RunAsync` to run code in background in lieu of `create_task.then` mechanism

 * `HttpHelper.h` provides common Http Get and response processing

 * `StorageHelper.h` provide method to read files, list folders, etc.
 
To address our XAML need, we have

 * `XamlHelper.h` provides simple UI building methods such as `MakeGrid`, `MakeTextBlock`, `MakeListView`, etc.

 * `CustomPropertyBase.h` and `CustomPropertyProviderHelper.h` for code macro to implement `ICustomPropertyProvider` for XAML data binding (e.g. in `DataTemplate`)

 * `IncrementalLoadingList.h` is modified from Microsoft's sample to allow for easy implementation of infinite loading

Finally, to implement an easy-to-use settings mechanism, we have
 
 * `SettingsHelper.h` provides `ToggleSwitchSetting`, `CheckBoxSetting` for boolean settings, `AutoSuggestBoxSetting` for string settings, `ComboBoxStringSetting` and `ComboBoxIntSetting` for settings from a selection list.

See the sample app for usage.
See also our app [ReddditQuick](https://github.com/light-tech/RedditQuick.git) for an example real-life application.

How to Use
----------

Either add the project as reference to your project

    <ProjectReference Include="LUwpUtilities\LUwpUtilities.vcxproj">
        <Project>{90310c59-030b-48a3-96ca-61db66512bba}</Project>
    </ProjectReference>

or reference the pre-built DLL

    <Reference Include="LUwpUtilities">
        <HintPath>Libs\LUwpUtilities.winmd</HintPath>
        <IsWinMDFile>true</IsWinMDFile>
        <Implementation>LUwpUtilities.dll</Implementation>
    </Reference>

in your app's project file. (Here, I assume the `LUwpUtilities.winmd` and `LUwpUtilities.dll` are located/symlinked in subfolder `Libs` in your project folder.)

We recommend that you use the library directly in the release version of the app. In that case, define `LUU_EXPORT=` to your project configuration and `#include` the LUwpUtilities headers in your program.

As an example, in [RedditQuick](https://github.com/light-tech/RedditQuick.git), we have the development project `RedditQuickDev.vcxproj` and the release project `RedditQuick.vcxproj`. The former references LUwpUtilities as a library while the latter (usually built with VSTS) embeds all LUwpUtilities implementation.

License
-------

 * Lightech Public License:
   You are free to use this library however you want as long as you give attribution to us.
   In particular, simply inform the user that your program uses LUwpUtilities at https://github.com/light-tech/LUwpUtilities.git in binary or source distribution of your program.
   Note that it is required to inform the user our repository URL.