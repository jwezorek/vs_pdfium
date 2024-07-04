# pdfium_lib
Google's pdfium codebase set up, by hand, as a self-contained Visual Studio solution that compiles to a static library on Windows under Visual Studio 2022.

Notes:
* ~~currently using Pdfium code pulled on 2/25/2021.~~ I pulled this code from Google's repository on 7/3/2024 it is whatever version of Pdfium that that implies.
* As part of the 7/3/2024 update, I changed the project files to be Visual Studio 2022 projects, and it was necessary to bump the C++ dialect up to C++20. (C++17 was not good enough because there is some code that uses desginated initializers.) 
* I have turned off Xfa, javascript, and Skia.
* I build all the 3rd party dependencies from code included herein, except for libjpeg-turbo which I statically link to as an included binary. I couldnt build libjpeg-turbo because it is compiled from assembly language files that are in the NASM dialect of assembly and out-of-the-box Visual Studio only supports MASM.
* I build the 3rd party dependencies directly into the Pdfium library not as separate libs.
# render_pdf

I've included an example command line utility that renders page ranges of PDFs to image files. usage is like

>render_pdf.exe some.pdf -o outputfile_stem -f PNG -s 2.0 -r 3-5

which means render some.pdf to output files outputfile_stem-3.png, outputfile_stem-4.png, and outputfile_stem-5.png, scaled up by 200%. 
