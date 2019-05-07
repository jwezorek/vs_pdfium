# pdfium_lib
Google's pdfium codebase set up, by hand, as a vanilla Visual Studio solution that compiles to a static library on Windows under Visual Studio 2017.

Notes:
* I pulled this code on 4/25/2019. It is whatever version of pdfium that implies.
* I have turned off Xfa, javascript, and Skia. I may eventually enable Skia as a separate build configuration but I have no plans to turn on Xfa or js.
* This solution is intended to be used for x64 only. I believe building x86 will not work because that configuration would necessitate changes to how some of the 3rd party dependencies are built.
* I build all the 3rd party dependencies from code included herein, except for libjpeg-turbo which I statically link to as an included binary. I couldnt build libjpeg-turbo because it is compiled from assembly language files that are in the NASM dialect of assembly and out-of-the-box Visual Studio only supports MASM.
* I build the 3rd party dependencies directly into the Pdfium library not as separate libs.
# render_pdf

I've included an example command line utility that renders page ranges of PDFs to image files. usage is like

>render_pdf.exe some.pdf -o outputfile_stem -f PNG -s 2.0 -r 3-5

which means render some.pdf to output files outputfile_stem-3.png, outputfile_stem-4.png, and outputfile_stem-5.png, scaled up by 200%. There is currently a bug in the image files in which the colors come out wrong. The bug is caused by stb-image-write expecting RGB formatted colors whereas Pdfium is outputting BGR colors.
