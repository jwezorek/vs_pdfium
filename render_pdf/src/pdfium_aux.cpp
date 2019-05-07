#include "pdfium_aux.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void InitPdfium(const RenderPdfOptions& options)
{
	FPDF_LIBRARY_CONFIG config;
	config.version = 2;
	config.m_pUserFontPaths = nullptr;
	config.m_pIsolate = nullptr;
	config.m_v8EmbedderSlot = 0;

	auto fd = options.fontDirectory();
	if (!fd.empty()) {
		static const char* path_array[2];
		static char buff[1024];
		snprintf(buff, 1024, "%s", fd.c_str());
		path_array[0] = buff;
		path_array[1] = nullptr;
		config.m_pUserFontPaths = path_array;
	}

	FPDF_InitLibraryWithConfig(&config);
}

ScopedFPDFBitmap RenderPage(FPDF_PAGE page, float scale)
{
	auto width = static_cast<int>(FPDF_GetPageWidth(page) * scale + 0.5f);
	auto height = static_cast<int>(FPDF_GetPageHeight(page) * scale + 0.5f);
	int alpha = FPDFPage_HasTransparency(page) ? 1 : 0;
	ScopedFPDFBitmap bitmap(FPDFBitmap_Create(width, height, alpha));

	if (!bitmap)
		throw std::runtime_error("unable to create bitmap.");

	FPDF_DWORD fill_color = alpha ? 0x00000000 : 0xFFFFFFFF;
	FPDFBitmap_FillRect(bitmap.get(), 0, 0, width, height, fill_color);

	FPDF_RenderPageBitmap(bitmap.get(), page, 0, 0, width, height, 0, FPDF_ANNOT);
	return bitmap;
}

int GetNumChannels(FPDF_BITMAP bmp)
{
	switch (FPDFBitmap_GetFormat(bmp)) {
	case FPDFBitmap_Gray:
		return 1;
	case FPDFBitmap_BGR:
		return 3;
	case FPDFBitmap_BGRx:
	case FPDFBitmap_BGRA:
		return 4;
	case FPDFBitmap_Unknown:
		return -1;
	}
	return -1;
}

void WriteImage(FPDF_BITMAP bitmap, ImageFormat format, const std::string& output_filename)
{
	int result = 0;
	int wd = FPDFBitmap_GetWidth(bitmap);
	int hgt = FPDFBitmap_GetHeight(bitmap);
	int num_channels = GetNumChannels(bitmap);
	int stride = FPDFBitmap_GetStride(bitmap);
	auto data = FPDFBitmap_GetBuffer(bitmap);

	if (num_channels < 0)
		throw std::runtime_error("invalid bitmap format");

	switch (format) {
	case ImageFormat::PNG:
		result = stbi_write_png(output_filename.c_str(), wd, hgt, num_channels, data, stride);
		break;
	case ImageFormat::BMP:
		result = stbi_write_bmp(output_filename.c_str(), wd, hgt, num_channels, data);
		break;
	case ImageFormat::JPEG:
		result = stbi_write_jpg(output_filename.c_str(), wd, hgt, num_channels, data, 85);
		break;
	}
	if (!result)
		throw std::runtime_error("Error writing iamge file.");
}
