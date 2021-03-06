﻿#pragma once

#include "ILogQuery.h"

class LogQueryResult;
namespace mrl { namespace utility { class SimpleTaskMessageWindow; } }
namespace boost { class thread; }

class LogQueryImpl : public ILogQuery {
public:
	LogQueryImpl();

protected:
	virtual ~LogQueryImpl();

	virtual bool load(const tstring& filePath);

	virtual const tstring& getFilePath() const;

	virtual LogQueryResult* query(const tstring& criteria);

	virtual void setSelected(const LogItem* item);

	virtual LogItem* getSelected() const;

	virtual LogQueryResult* getCurQueryResult() const;

	virtual void scrollTo(int y);

	void reset(const vector<LogItem*>& logItems);

	void reset();

private:
	void loadFile(vector<LogItem*>& logItems);

	void setCurQueryResult(LogQueryResult* curQueryResult);

	tstring filePath;
	vector<LogItem*> logItems;
	tstring curQueryCriteria;
	boost::basic_regex<TCHAR>* curQueryRegex;
	LogQueryResult* curQueryResult;

	mrl::utility::SimpleTaskMessageWindow* taskWnd;

	// UNDONE: 抽出复用
	void startMonitor();

	bool monitoring;
	boost::thread* monitorThread;
};