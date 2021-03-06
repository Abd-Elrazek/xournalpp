/*
 * Xournal++
 *
 * Base class for Exports
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include "BlockingJob.h"

#include <XournalType.h>

#include <boost/filesystem/path.hpp>
using boost::filesystem::path;

class Control;

class BaseExportJob : public BlockingJob
{
public:
	BaseExportJob(Control* control, string name);

protected:
	virtual ~BaseExportJob();

public:
	virtual void afterRun();

public:
	virtual bool showFilechooser();
	string getFilterName();

protected:
	void initDialog();
	virtual void addFilterToDialog() = 0;
	void addFileFilterToDialog(string name, string pattern);
	void clearExtensions(path& filename);
	bool checkOverwriteBackgroundPDF(path& filename);
	virtual bool isUriValid(string& uri);

private:
	XOJ_TYPE_ATTRIB;

protected:
	GtkWidget* dialog;

	path filename;

	string errorMsg;

	class ExportType
	{
		public:
			string extension;
			bool withoutBackground;
			ExportType(string ext, bool hideBg) : extension(ext), withoutBackground(hideBg){}
	};
};
