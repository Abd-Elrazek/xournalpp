/*
 * Xournal++
 *
 * PDF Document Export Abstraction Interface
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <PageRange.h>

#include <XournalType.h>

#include <boost/filesystem/path.hpp>
using boost::filesystem::path;

class XojPdfExport
{
public:
	XojPdfExport();
	virtual ~XojPdfExport();

public:
	virtual bool createPdf(path file) = 0;
	virtual bool createPdf(path file, PageRangeVector& range) = 0;
	virtual string getLastError() = 0;

	/**
	 * Export without background
	 */
	virtual void setNoBackgroundExport(bool noBackgroundExport);

private:
	XOJ_TYPE_ATTRIB;
};

