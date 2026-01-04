find_package(Qt6 REQUIRED COMPONENTS  Widgets)

# Enable Qt MOC to be invoked on source files
set(CMAKE_AUTOMOC ON)
# Enable Qt UIC to generate C++ header out of UI forms
set(CMAKE_AUTOUIC ON)

# Add library with user interface
add_library(gui STATIC
  include/gui/Factory.h
  impl/interfaces/Dialogs.h

  impl/Factory.cpp
  impl/DialogsImpl.h
  impl/DialogsImpl.cpp
  impl/PlainTextMatchCollector.h
  impl/PlainTextMatchCollector.cpp
  impl/SearchWorker.h
  impl/SearchWorker.cpp
  impl/WorkerThread.h
  impl/WorkerThread.cpp

  impl/MainWindow.h
  impl/MainWindow.cpp
)

target_sources(gui
  PUBLIC
   FILE_SET HEADERS
   BASE_DIRS 
    include
   FILES
    include/gui/Factory.h
)

# Add the form file into the list of sources and make it possible
# for CMake invoke Qt UIC for it
target_sources(gui PRIVATE impl/ui/mainwindow.ui)

# Link `files-search` privately
target_link_libraries(gui PRIVATE files-search)

# Link `Qt::Widgets` publicly
target_link_libraries(gui  PUBLIC Qt::Widgets)

# Make implementation folder available only internal for the library
target_include_directories(gui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
