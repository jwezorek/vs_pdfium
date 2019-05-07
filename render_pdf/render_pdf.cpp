#include "src/pdfium_aux.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) 
{
	try {
		RenderPdfOptions options(argc, argv);
		if (options.invalid()) {
			std::cout << "usage:\nrender_pdf.exe inputfile.pdf [output_file.png]" << std::endl;
			return -1;
		}
		InitPdfium(options);
		ScopedFPDFDocument doc(FPDF_LoadDocument(options.inputFile().c_str(), options.password().c_str()));

		std::cout << "rendering " << std::filesystem::path(options.inputFile()).filename() << "...\n";

		auto[from_page_index, to_page_index] = options.range().get(doc.get());
		int num_pages = to_page_index - from_page_index + 1;
		for (auto index = from_page_index; index <= to_page_index; index++) {

			ScopedFPDFPage page(FPDF_LoadPage(doc.get(), index));
			auto bitmap = RenderPage(page.get(), options.scale());
			auto out_filename = options.outputFile( (num_pages > 1) ? std::optional<int>(index) : std::nullopt);
			WriteImage( bitmap.get(), options.imageFormat(), out_filename );
			std::cout << "    generated " << std::filesystem::path(out_filename).filename() << ".\n";
		}
		std::cout << "complete.\n";
	} catch (std::runtime_error e) {
		std::cout << "error: " << e.what() << std::endl;
		return -1;
	} catch (...) {
		std::cout << "error: unkown fatal error occurred" << std::endl;
		return -1;
	}

	return 0;
}




