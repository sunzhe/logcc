﻿#pragma once

#include <mrl/common/ObserverContainer.h>
#include "ILogQueryObserver.h"

struct LogItem;
class LogQueryResult;

/**
 * 日志查询，每个对象对应一个日志文件
 * @author CaoShuhao
 * @date 2014-1-12
 */
class ILogQuery : public mrl::common::ObserverContainer<ILogQueryObserver> {
public:
	virtual ~ILogQuery() { }

	/**
	 * 同步读取文件并按分行
	 * @param filePath [in] 文件路径
	 * @author CaoShuhao
	 * @date 2014-1-12
	 */
	virtual bool load(const tstring& filePath) = 0;

	/**
	 * 获取日志路径
	 * @return 文件路径
	 * @author CaoShuhao
	 * @date 2014-1-12
	 */
	virtual const tstring& getFilePath() const = 0;

	/**
	 * 设置某行为选中，其他所有行为非选中
	 * @param item [in] 日志项
	 * @author CaoShuhao
	 * @date 2014-1-15
	 */
	virtual void setSelected(const LogItem* item) = 0;

	/**
	 * 返回当前选中行，没有选中时返回NULL
	 * @return 选中行
	 * @author CaoShuhao
	 * @date 2014-1-15
	 */
	virtual LogItem* getSelected() const = 0;

	/**
	 * 条件查询
	 * @param criteria [in] 条件
	 * @return 查询结果集
	 * @author CaoShuhao
	 * @date 2014-1-18
	 */
	virtual LogQueryResult* query(const tstring& criteria) = 0;

	/**
	 * 获取当前查询结果
	 * @return 查询结果集
	 * @author CaoShuhao
	 * @date 2014-1-19
	 */
	virtual LogQueryResult* getCurQueryResult() const = 0;

	/**
	 * 滚动视图
	 * @param y [in] y轴坐标
	 * @author CaoShuhao
	 * @date 2014-1-20
	 */
	virtual void scrollTo(int y) = 0; // UNDONE: 奇怪的接口方法

protected:
	void notifyGeneralDataChanged() const {
		forEachObserver([] (ILogQueryObserver* p) { p->onGeneralDataChanged(); });
	}

	void notifyQueryResultChanged() const {
		forEachObserver([] (ILogQueryObserver* p) { p->onQueryResultChanged(); });
	}

	void notifyScrollPositionChanged(int y) const {
		forEachObserver([y] (ILogQueryObserver* p) { p->onScrollPositionChanged(y); });
	}
};
