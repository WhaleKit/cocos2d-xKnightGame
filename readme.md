A little game about chess knight in maze I made to learn about cocos2d-x.

because project generated by cocos2d-x is too big (almost 0.5 GB), this repository only contains "Classes" and "Resources" folders, cmakefile and 1 file from cocos2d-x with small bugfix in it.

#how to build: with mingw on winfows#
build cocos2d-x with Mingw following instructions from this post:
http://discuss.cocos2d-x.org/t/guide-compiling-cocos2d-x-with-clion/22541/23

create a project and put content of this repository there, resolving overwriting conflicts in favor of this repo. If you're ussing version of cocos2d-x newer than (cocos2d-x-3.13.1), rewriting CCDirector.cpp might not be needed, because bug in it might be fixed in newer version of cocos

if you already have built coco2d-x 3.x I believe you can just create a project, put Classes and Resources dirs from this repo in project dir and add files from "Classes" dir into your build system.