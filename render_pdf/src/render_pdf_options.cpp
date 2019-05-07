#include "render_pdf_options.h"
#include "ProgramOptions.hxx"
#include <unordered_map>
#include <filesystem>

PageRange::PageRange(const std::string& range_str)
{
	if (range_str.empty()) {
		_from = _to = 0;
		return;
	}

	if (range_str == "all") {
		_from = _to = -1;
		return;
	}

	auto dash_pos = range_str.find('-');
	if (dash_pos == std::string::npos) {
		_from = _to = std::stoi(range_str);
	}

	int n = range_str.length();
	_from = std::stoi(range_str.substr(0, dash_pos));
	_to = std::stoi(range_str.substr(dash_pos+1, n-dash_pos-1));
}

bool PageRange::isAll() const
{
	return _from == -1 || _to == -1;
}

std::tuple<int, int> PageRange::get(FPDF_DOCUMENT doc) 
{
	if (isAll()) {
		return { 0, FPDF_GetPageCount(doc) };
	} else {
		return { _from, _to };
	}
}

ImageFormat RenderPdfOptions::getImageFormat(const std::string& format_str, const std::string& output_file) const
{
	static std::unordered_map<std::string, ImageFormat> str_to_fmt = {
		{"png", ImageFormat::PNG}, {"PNG", ImageFormat::PNG}, {"bmp", ImageFormat::BMP}, {"BMP", ImageFormat::BMP},
		{"jpg", ImageFormat::JPEG}, {"JPG", ImageFormat::JPEG}, {"jpeg", ImageFormat::JPEG}, {"JPEG", ImageFormat::JPEG},
		{".png", ImageFormat::PNG}, {".PNG", ImageFormat::PNG}, {".bmp", ImageFormat::BMP}, {".BMP", ImageFormat::BMP},
		{".jpg", ImageFormat::JPEG}, {".JPG", ImageFormat::JPEG}, {".jpeg", ImageFormat::JPEG}, {".JPEG", ImageFormat::JPEG},
		{"", ImageFormat::Unknown},
	};
	auto item = str_to_fmt.find(format_str);
	auto format = (item != str_to_fmt.end()) ? item->second : ImageFormat::Unknown;
	if (format != ImageFormat::Unknown)
		return format;
	if (output_file.empty())
		return ImageFormat::PNG;
	auto extension = std::filesystem::path(output_file).extension().string();
	if (extension.empty() || str_to_fmt.find(extension) == str_to_fmt.end())
		return ImageFormat::PNG;
	return str_to_fmt[extension];
}

std::tuple<std::string, std::string, std::string> RenderPdfOptions::getOutputFile(const std::string& inp, const std::string& out, ImageFormat fmt) const
{
	static std::unordered_map<ImageFormat, std::string> str_to_fmt = {
		{ImageFormat::PNG, ".png"},
		{ImageFormat::BMP, ".bmp"},
		{ImageFormat::JPEG, ".jpg"},
		{ImageFormat::Unknown, ""}
	};

	std::string directory, stem, extension;
	if (!out.empty()){
		auto path = std::filesystem::path(out);
		directory = path.parent_path().string();
		stem = path.stem().string();
		extension = std::filesystem::path(out).extension().string();
		extension = (!extension.empty()) ? extension : str_to_fmt[fmt];
	} else {
		auto path = std::filesystem::path(inp);
		directory = path.parent_path().string();
		stem = std::filesystem::path(inp).stem().string();
		extension = str_to_fmt[fmt];
	}
	return { directory, stem, extension };
}

RenderPdfOptions::RenderPdfOptions(int argc, char** argv) :
	_image_format(ImageFormat::PNG), _scale( 1.0f)
{
	po::parser parser;
	parser[""].type(po::string);
	parser["output-path"].abbreviation('o').description("output file path").type(po::string).fallback("");
	parser["font-path"].abbreviation('t').description("font directory").type(po::string).fallback("");
	parser["scale"].abbreviation('s').description("font directory").type(po::f32).fallback(1.0F);
	parser["format"].abbreviation('f').description("image file format").type(po::string).fallback("");
	parser["page-range"].abbreviation('r').description("page range (\"all\", \"1\", \"3-6\") ").type(po::string).fallback("0");
	parser["password"].abbreviation('p').description("password").type(po::string).fallback("");

	try {
		if (!parser(argc, argv))
			return;

		auto input_opt = parser[""].get().string;
		if (input_opt.empty())
			return;

		auto output_file = parser["output-path"].get().string;
		_input_file = input_opt;
		_image_format = getImageFormat(parser["format"].get().string, output_file);
		std::tie(_output_file_directory, _output_file_stem, _output_file_extension) = getOutputFile(input_opt, output_file, _image_format);
		_scale = parser["scale"].get().f32;
		_range = PageRange(parser["page-range"].get().string);
		_font_directory = parser["font-path"].get().string;
		_password = parser["password"].get().string;
	} catch (...) {
		_input_file = "";
	}
}
std::string RenderPdfOptions::fontDirectory() const
{
	return _font_directory;
}

std::string RenderPdfOptions::outputFile(std::optional<int> page_num) const
{
	std::string path_stem = (!_output_file_directory.empty()) ?
		(std::filesystem::path(_output_file_directory) / _output_file_stem).string() :
		_output_file_stem;

	return (page_num.has_value()) ?
		path_stem + "-" + std::to_string(page_num.value()) + _output_file_extension :
		path_stem + _output_file_extension;
}

std::string RenderPdfOptions::inputFile() const
{
	return _input_file;
}
std::string RenderPdfOptions::password() const
{
	return _password;
}
float RenderPdfOptions::scale() const
{
	return _scale;
}
PageRange RenderPdfOptions::range() const
{
	return _range;
}
ImageFormat RenderPdfOptions::imageFormat() const
{
	return _image_format;
}
bool RenderPdfOptions::invalid() const
{
	return _input_file.empty();
}
