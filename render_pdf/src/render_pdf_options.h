#pragma once

#include <optional>
#include <string>

#include "public/fpdfview.h"

enum class ImageFormat {
	PNG,
	BMP,
	JPEG,
	Unknown
};

struct PageRange {

private:
	int _from;
	int _to;
public:
	PageRange(const std::string& range_str = "");
	bool isAll() const;
	std::tuple<int, int> get(FPDF_DOCUMENT doc);
};


struct RenderPdfOptions {

private:
	std::string _input_file;
	std::string _output_file_directory;
	std::string _output_file_stem;
	std::string _output_file_extension;
	float _scale;
	PageRange _range;
	ImageFormat _image_format;
	std::string _font_directory;
	std::string _password;

	ImageFormat getImageFormat(const std::string& format_str, const std::string& output_file) const;
	std::tuple<std::string, std::string, std::string> getOutputFile(const std::string& inp, const std::string& out, ImageFormat fmt) const;

public:
	RenderPdfOptions(int argc, char** argv);
	std::string fontDirectory() const;
	std::string outputFile( std::optional<int> page_num ) const;
	std::string inputFile() const;
	std::string password() const;
	float scale() const;
	PageRange range() const;
	ImageFormat imageFormat() const;
	bool invalid() const;
};