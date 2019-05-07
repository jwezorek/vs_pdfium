#pragma once

#include <string>
#include <optional>
#include "render_pdf_options.h"
#include "public/fpdfview.h"
#include "public/cpp/fpdf_scopers.h"

void InitPdfium(const RenderPdfOptions& options);
std::string GetImageFilename(const RenderPdfOptions& options, std::optional<int> page_number);
ScopedFPDFBitmap RenderPage(FPDF_PAGE page, float scale);
void WriteImage(FPDF_BITMAP bitmap, ImageFormat format, const std::string& output_filename);