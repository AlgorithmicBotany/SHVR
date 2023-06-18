# Qt Debugging Cheat List

* If Qt cannot compile, then make sure to download Qt VS Tool. 

* If Qt is having the problem like:

  ```
  1>Moc'ing includes\ThreadChecker.h...
  1>Missing value after '-I'.
  ```

  Then in the file "\<*Project Name*\>.vcxproj", eliminate all instance of "*" -I$(NOINHERIT)"*".

  * This may happen at random, when Qt VS Tool decides to reload ".vcxproj".

* If compiling with haptics give "_ITERATOR_DEBUG_LEVEL" mismatch (or release compiles but debug doesn't):
  (SOURCE : https://stackoverflow.com/questions/7668200/error-lnk2038-mismatch-detected-for-iterator-debug-level-value-0-doesnt)

  I make a little update on this issue, as I just had the same error today on an application which is linking against a static lib, after I migrated the old Visual 6 project to Visual Studio 2012.

  In my case the error was that I mistakenly compiled the Release version of the static lib with **/MDd** instead of **/MD**, whereas the application is **/MD** in release. Setting the correct /MD in the static lib project solved the issue.

  This is done in **Project properties**

  - Select **Configuration Properties / C C++ / Code Generation** in the tree
  - and the option **Runtime Library** set to the same on all your dependencies projects and application.
  
* Add Qt directory to PATH

* Make sure the version number matches the Qt Directory

  

